#include "vtkDrillholeGA.h"

//vtk standard requestdata stuff
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"

//used by this file
#include "vtkCellArray.h"
#include "vtkDataSet.h"
#include "vtkMath.h"
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"
#include "vtkPointData.h"
#include "vtkPointLocator.h"
#include "vtkPolyData.h"
#include "time.h"
#include "vtkSQLiteQuery.h"
#include "vtkSQLiteDatabase.h"
#include "vtkSQLQuery.h"

#include <sstream>
#include <algorithm>

#include "GA.h"

//_MSC_VER means we are compiling on MSVC++ or Intel's C++ compiler
//but nobody here uses Intel's compiler, so this is good enough to be
//sure that we're compiling on windows
#ifdef _MSC_VER 
#include "windows.h"
#else
//#include "linux.h" //lol - find some other way of getting time from linux
#endif


vtkCxxRevisionMacro(vtkDrillholeGA, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkDrillholeGA);

//This is used for a UI hack to force updating of a property. See the pq class
void vtkDrillholeGA::DoSomething()
  {
  this->Modified();
  }

//----------------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------------
vtkDrillholeGA::vtkDrillholeGA()
  {
  const int input_ports = 2;
  const int output_ports = 2;
  this->SetNumberOfInputPorts(input_ports);
  this->SetNumberOfOutputPorts(output_ports);
  
  //ensure a new seed on every run
  srand(time(NULL));

  this->CollarFile = 0;
  this->DrillholeDatabase = 0;
  this->ResumeFromC2S = 0;
  this->ResumeFromS2C = 0;
  this->RefreshPlease = 0;
  this->Resume = false;

  for (int i = 0; i < 3; i++)
    {
    this->Ranges[i] = 0.0;
    this->Angles[i] = 0.0;
    }

  //ga parameters
  this->PopulationSize = 50;
  this->NumGenerations = 50;
  this->IndividualSize = 20;
  this->MutationProb = 0.1;
  this->StagnancyMutationProb = 0.0;
  this->MutationGeneProb = 0.1;
  this->StagnancyMutationGeneProb = 0.0;

  // note that the first three chromosome ranges (x,y,z) are pretty much useless
  // while this code can only accomplish choosing from a set of collar points
  this->ChromosomeRangeMin[0] = 9178;
  this->ChromosomeRangeMin[1] = 9576;
  this->ChromosomeRangeMin[2] = 10700;
  this->ChromosomeRangeMin[3] = -50;
  this->ChromosomeRangeMin[4] = -45;
  this->ChromosomeRangeMin[5] = 150;

  this->ChromosomeRangeMax[0] = 9268;
  this->ChromosomeRangeMax[1] = 9754;
  this->ChromosomeRangeMax[2] = 10712;
  this->ChromosomeRangeMax[3] = 50;
  this->ChromosomeRangeMax[4] = 70;
  this->ChromosomeRangeMax[5] = 1000;
  this->SegmentLength = 35.0;
  this->HillClimbLength = 5.0;

  //metric tensor stuff (the ability to measure ellipsoidal distances)
  this->MetricTensor = new double[6]; 
  this->MetricTensor[0]=1.0;
  this->MetricTensor[1]=0.0;
  this->MetricTensor[2]=0.0;
  this->MetricTensor[3]=1.0;
  this->MetricTensor[4]=0.0;
  this->MetricTensor[5]=1.0;

  //elite population
  this->Elite = new Population();
  }

//----------------------------------------------------------------------------
// Destructor
//----------------------------------------------------------------------------
vtkDrillholeGA::~vtkDrillholeGA()
  {
  this->SetDrillholeDatabase(0);
  this->SetCollarFile(0);
  this->RefreshPlease = 0;
  this->ResumeFromS2C = 0;
  this->ResumeFromC2S = 0;
  delete this->Elite;
  delete [] this->MetricTensor;
  //this->Locator->Delete();  
  }

//----------------------------------------------------------------------------
// Fill Input Port Information (Source and Input here)
//----------------------------------------------------------------------------
int vtkDrillholeGA::FillInputPortInformation ( int , vtkInformation* info )
  {
  info->Set ( vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataSet", 0 );
  info->Set ( vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataSet", 1 );
  info->Set ( vtkAlgorithm::INPUT_IS_OPTIONAL(), 1);
  return 1;
  }

//----------------------------------------------------------------------------
// Fill Output Port Information (Best Result and the grid with properties)
//----------------------------------------------------------------------------
int vtkDrillholeGA::FillOutputPortInformation( int port, vtkInformation *info)
  {
  if (port == 0)
    info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkPolyData");
  else if(port == 1)
    info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkDataSet");
  return 1;
  }

//----------------------------------------------------------------------------
// SetSourceConnection
//----------------------------------------------------------------------------
void vtkDrillholeGA::SetSourceConnection( vtkAlgorithmOutput *algOutput )
	{
	this->SetInputConnection( 1, algOutput );
	}

//----------------------------------------------------------------------------
// PrintSelf
//----------------------------------------------------------------------------
void vtkDrillholeGA::PrintSelf(ostream& os, vtkIndent indent)
  {
  this->Superclass::PrintSelf(os, indent);
  }

//----------------------------------------------------------------------------
// GetMS - returns number of milliseconds since some base. You should be able
//       - to subtract subsequent calls to get a time difference between calls
//----------------------------------------------------------------------------
unsigned int vtkDrillholeGA::GetMS()
  {
#ifdef _MSC_VER //see top of file
  return GetTickCount();
#else
  return 0; //figure out the equivalent linux thing
#endif
  }

//----------------------------------------------------------------------------
// Overridden "Set" methods to provide special functionality
//----------------------------------------------------------------------------
void vtkDrillholeGA::SetMutationProb(double value)
  {
  //this->MutationProb = value / 100.0; //so user can use 0-100%
  this->MutationProb = value;
  this->Modified();
  }
void vtkDrillholeGA::SetMutationGeneProb(double value)
  {
  //this->MutationGeneProb = value / 100.0; //so user can use 0-100%
  this->MutationGeneProb = value;
  this->Modified();
  }

void vtkDrillholeGA::SetEMajor(double value)
  {
  this->EMajor = value;
  this->Ranges[0] = value;
  this->Modified();
  }
void vtkDrillholeGA::SetEMedium(double value)
  {
  this->EMedium = value;
  this->Ranges[1] = value;
  this->Modified();
  }
void vtkDrillholeGA::SetEMinor(double value)
  {
  this->EMinor = value;
  this->Ranges[2] = value;
  this->Modified();
  }

void vtkDrillholeGA::SetEAzimuth(double value)
  {
  this->EAzimuth = value;
  this->Angles[0] = value;
  this->Modified();
  }
void vtkDrillholeGA::SetEDip(double value)
  {
  this->EDip = value;
  this->Angles[1] = value;
  this->Modified();
  }
void vtkDrillholeGA::SetERake(double value)
  {
  this->ERake = value;
  this->Angles[2] = value;
  this->Modified();
  }

void vtkDrillholeGA::SetDAzimuthMin(double value)
  {
  this->DAzimuthMin = value;
  this->ChromosomeRangeMin[3] = value;
  this->Modified();
  }
void vtkDrillholeGA::SetDAzimuthMax(double value)
  {
  this->DAzimuthMax = value;
  this->ChromosomeRangeMax[3] = value;
  this->Modified();
  }

void vtkDrillholeGA::SetDDipMin(double value)
  {
  this->DDipMin = value;
  this->ChromosomeRangeMin[4] = value;
  this->Modified();
  }
void vtkDrillholeGA::SetDDipMax(double value)
  {
  this->DDipMax = value;
  this->ChromosomeRangeMax[4] = value;
  this->Modified();
  }

void vtkDrillholeGA::SetDLengthMin(double value)
  {
  this->DLengthMin = value;
  this->ChromosomeRangeMin[5] = value;
  this->Modified();
  }
void vtkDrillholeGA::SetDLengthMax(double value)
  {
  this->DLengthMax = value;
  this->ChromosomeRangeMax[5] = value;
  this->Modified();
  }

//----------------------------------------------------------------------------
// RequestInformation
//----------------------------------------------------------------------------
int vtkDrillholeGA::RequestInformation(vtkInformation* info, vtkInformationVector** vector, vtkInformationVector* vector2)
  {
  /*
    The notion behind the "RefreshPlease" variable is to be able to click a button on the user interface
    panel that causes RequestInformation to fire. When this happens, a combo box on the user interface
    panel is updated with the data in ResumeFromS2C
  */
  if (this->RefreshPlease == 1)
    {
    if (this->DatabaseValid())
      {
      vtkSQLiteDatabase *db = vtkSQLiteDatabase::New();
      this->OpenDatabase(db);
      //create a query instance
      vtkSQLQuery *query = (vtkSQLiteQuery*)db->GetQueryInstance();
      query->SetQuery("select distinct hole_id from collar");
      query->Execute();
      //read in the results
      vtkStdString s("");
      while( query->NextRow() ) 
		    {  
		    // it's okay that there will be an extra "|" at the end of the string
        // because on the client side, Qt will ignore "empty" tokens
		    s.append(query->DataValue(0).ToString());
        s.append("|");
        }
      //stringify the results
      int size = s.size();
      /*if (this->ResumeFromS2C) //for whatever reason, deleting either the pointer on the array[] fails
        {
        delete this->ResumeFromS2C;
        }*/
      this->ResumeFromS2C = new char[size+1];
      strncpy(this->ResumeFromS2C, s.c_str(), size);
      this->ResumeFromS2C[size] = '\0';
      //cleanup
	    query->SetQuery("");
	    query->Execute();
	    query->Delete(); 		
      db->Close();
      db->Delete();
      }
    else
      {
      this->ResumeFromS2C = "";
      vtkErrorMacro(<<"Could not open database to populate previous GA runs.");
      }    
    }
  return 1;
  }

//----------------------------------------------------------------------------
// RequestData
//----------------------------------------------------------------------------
int vtkDrillholeGA::RequestData(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector)
  {
  //wasn't a real Apply click, so just return
  if (this->RefreshPlease == 1)
    {
    return 1;
    }

  //used to measure runtime
  unsigned int time1 = this->GetMS();

  // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *sourceInfo = inputVector[1]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);
  vtkInformation *outInfo2 = outputVector->GetInformationObject(1);

	// Get the input (pointset) and optionally the source (collar points)
  this->InputGrid = vtkDataSet::SafeDownCast (inInfo->Get(vtkDataObject::DATA_OBJECT()));
  if (!this->InputGrid)
    {
    vtkErrorMacro(<<"The Input Grid (Input) is null.");
    return 0;
    }
  if (sourceInfo)
    {
	  this->InputCollarPoints = vtkDataSet::SafeDownCast (sourceInfo->Get(vtkDataObject::DATA_OBJECT()));
    }
  else
    {
    this->InputCollarPoints = NULL;
    }
	
  // get the two outputs ready
  vtkPolyData *output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkDataSet *output2 = vtkDataSet::SafeDownCast(outInfo2->Get(vtkDataObject::DATA_OBJECT()));
  output2->DeepCopy(this->InputGrid);

  //read in the collar points from wherever they may be
  if (!this->ReadCollarPoints())
    {
    return 1;
    }

  //initialize the metric tensor for ellipsoid distances
  this->SetMetric();

  //initialize the point locator based on the input grid
  this->Locator = vtkPointLocator::New();
  this->Locator->SetDataSet(this->InputGrid);

  //run GA
  this->InitialNeighbors();
  int status = this->MainLoop(output, output2);
  this->neighbors.erase(this->neighbors.begin(), this->neighbors.end());
  if (status == 0)
    {
    //cleanup
    this->Locator->Delete();
    return 0; 
    }
  //write the elite population to the database
  this->WriteElite();

  /*
    TODO: this report information should really be stored in an output file or database
  */
  //report the best and worst of the runs
  unsigned int time2 = this->GetMS();
  vtkErrorMacro(<< "\nBest Score: " << this->Elite->best() << "\n" 
                << "Worst Score: " << this->Elite->worst() << "\n"
                << "The GA ran for about about " << (time2 - time1) / 1000.0 << " seconds.\n");
  //report lengths of drillholes
  Individual best = *(this->Elite->best_element());
  std::stringstream s;
  s<<"\nX,Y,Z,Az,Dip,Length\n";
  for (Individual::iterator i = best.begin(); i < best.end(); ++i)
    {
    //*i is a drillhole, length is at (*i)[5]
    s << (*i)[0] << "," << (*i)[1] << "," << (*i)[2] << "," 
      << (*i)[3] << "," << (*i)[4] << "," << (*i)[5] << "\n";
    }
  vtkErrorMacro(<< s.str().c_str());

  //cleanup
  this->Locator->Delete();
  return status;
  }

//----------------------------------------------------------------------------
// InitialNeighbors - calculates the neighbors of the pointset to itself
//----------------------------------------------------------------------------
void vtkDrillholeGA::InitialNeighbors()
  {
  //for each point in the input grid, find its neighbours
  int npts = this->InputGrid->GetNumberOfPoints();
  for (int i = 0; i < npts; ++i)
    {
    int row = i;
    for (int j = 0; j < npts; ++j)
      {
      int col = j;
      double p[3];
      double q[3];
      this->InputGrid->GetPoint(i, p);
      this->InputGrid->GetPoint(j, q);
      if (this->EvaluateDist2(p,q) <= 1)
        {
        //the point is inside the ellipsoid, so mark it as a neighbour
        this->SetNeighbor(row, col, 1);
        }
      else
        {
        //the point is not inside the ellipsoid, so mark it as not neighbor
        this->SetNeighbor(row, col, 0);
        }
      }
    }
  }

//----------------------------------------------------------------------------
// SetNeighbor - sets whether a point is a neighbor of another point
//----------------------------------------------------------------------------
void vtkDrillholeGA::SetNeighbor(int row, int col, int is_neighbor)
  {
  //You're probably wondering why if it's a vector of neighbours that
  //I bother with "is_neighbor" at all. It's because in the past the
  //data structure for neighbors was not a map of vectors, and this way
  //you can define a new way of storing neighbours that may be more
  //efficient which knows what isn't a neighbour
  if (is_neighbor)
    {
    this->neighbors[row].push_back(col);
    }
  }

//----------------------------------------------------------------------------
// IsNeighbor - tells you whether a point is a neighbor of another point
//----------------------------------------------------------------------------
bool vtkDrillholeGA::IsNeighbor(int row, int col)
  {
  std::vector<int> row_ = this->neighbors[row];
  return std::find(row_.begin(), row_.end(), col) != row_.end();
  }

//----------------------------------------------------------------------------
// MainLoop - GA runs here
//----------------------------------------------------------------------------
int vtkDrillholeGA::MainLoop(vtkPolyData *output, vtkDataSet *output2)
  {
  /// Important GA Implementation Details Follow

  /// - Create initial population
  /// - Evaluate initial population
  /// - Initialize the elite list
  /// - If we are resuming, load existing elite population
  /// - For each pass
  ///   - For each generation
  ///     - Call crossover (will handle selection)
  ///     - Call mutation
  ///     - Call fitness
  ///     - Fill the elite list
  ///     - Deal with stagnancy
  ///   - Do Length Hill Climbing (if enabled)
  /// - Do final Length Hill Climb

  Population population = this->InitialPopulation();
  this->Fitness(population); //stores the scores for each individual inside
  *(this->Elite) = population;
  if (this->Resume)
    {
    Individual in = this->ResumeElite();
    if (in.score != NO_SCORE)
      {
      //this will have the effect of giving a 'half-converged' elite list
      for (int i = 0; i < this->Elite->size() / 2; ++i)
        {
        (*(this->Elite))[i] = in;
        }
      }
    }

  // start the main loop
  // number of iterations is equal to NumPasses*NumGenerations
  // that is not exactly standard, so make sure you understand the effects
  for (int passes = 0; passes < this->NumPasses; ++passes)
    {
    for (int gen = 0; gen < this->NumGenerations; ++gen)
      {
      this->Crossover(population);
      this->Mutation(population);
      this->Fitness(population);
      this->Elite->Update(population);
      this->Stagnancy(population);
      }
    if (this->DoHillClimbLength)
      this->LengthHillClimb();
    if (this->DoHillClimbDirection)
      this->DirectionHillClimb();
    }
  if (this->DoHillClimbLength)
    this->LengthHillClimb();
  
  //Create drillhole output
  //1. create the points that represent where the drillhole was evaluated
  vtkPoints *points = vtkPoints::New();
  int last = this->Elite->size() - 1;
  this->GetDrillholePoints(*this->Elite->best_element(), points);
  //2. add points to output (automatically 'allocates')
  vtkCellArray *cells = vtkCellArray::New();
  for (int i = 0; i < points->GetNumberOfPoints(); ++i)
    {
    cells->InsertNextCell(1);
    cells->InsertCellPoint(i);
    }
  output->SetVerts(cells);
  output->SetPoints(points);
  points->Delete();
  cells->Delete();

  //create the volume of effect output (indicates which blocks were
  //"covered" by drillholes and which were not)
  vtkIntArray *prop = vtkIntArray::New();
  prop->SetName("Found");
  this->IndividualFitness(*this->Elite->best_element(), prop);
  output2->GetPointData()->AddArray(prop);
  prop->Delete();
  return 1;
  }

//----------------------------------------------------------------------------
// InitialPopulation
//----------------------------------------------------------------------------
Population vtkDrillholeGA::InitialPopulation()
  {
  //this creates a random initial population. Chooses from a list of collar
  //points randomly (for x, y, z)
  std::vector<Individual> individuals;
  for (int i = 0; i < this->PopulationSize; ++i)
    {
    //create PopulationSize individuals
    std::vector<Drillhole> drillholes;
    for (int j = 0; j < this->IndividualSize; ++j)
      {
      std::vector<double> parameters;
      int index = rand() % this->CollarPoints.size(); 
      parameters.push_back(this->CollarPoints[index][0]);
      parameters.push_back(this->CollarPoints[index][1]);
      parameters.push_back(this->CollarPoints[index][2]);
      for (int k = 3; k < NUM_GENES; ++k)
        {
        parameters.push_back(this->RandGene(k));
        }
      drillholes.push_back(Drillhole(parameters));
      }
    individuals.push_back(Individual(drillholes));
    }
  return Population(individuals);
  }

//----------------------------------------------------------------------------
// ReadCollarPoints - Retrieves collar points from either a csv file or the
// filter's input.
//----------------------------------------------------------------------------
bool vtkDrillholeGA::ReadCollarPoints()
  {
  this->CollarPoints.clear();
  if (this->CollarLocation == 0) //CollarLocation == 0 is csv file
    {
    std::ifstream file;
    file.open(this->CollarFile);
    if (!file || file.fail())
      {
      vtkErrorMacro("Could not open collar point file.\n");
      return false;
      }
    std::string line;
    while(getline(file, line))
      {
      std::stringstream ss(line);
      std::string item;
      std::vector<std::string> v;
      while (std::getline(ss, item, ','))
        {
        v.push_back(item);
        }
      if (!v.empty())
        {
        DoubleVector dv;
        dv.push_back(atof(v[0].c_str()));
        dv.push_back(atof(v[1].c_str()));
        dv.push_back(atof(v[2].c_str()));
        this->CollarPoints.push_back(dv);
        }
      }
    }
  else if(this->CollarLocation == 1) // CollarLocation == 1 is the Filter's Source
    {
    if (!this->InputCollarPoints)
		  {
		  vtkErrorMacro(<<"No Collar Points specified in the Filter Source");
		  return false;
		  }
    int npts = this->InputCollarPoints->GetNumberOfPoints();
    if (npts == 0)
      {
      vtkErrorMacro(<<"No Collar Points found in the Filter Source");
      return false;
      }
    for (int i = 0; i < npts; ++i)
      {
      double xyz[3];
      this->InputCollarPoints->GetPoint(i, xyz);
      DoubleVector dv(&xyz[0], &xyz[3]);
      this->CollarPoints.push_back(dv);
      }
    }
  

  return true;
  }

//----------------------------------------------------------------------------
// ResumeElite - Finds the individual named "ResumeFrom" in the database
//               If it fails, the individual's score will be "null value"
//----------------------------------------------------------------------------
Individual vtkDrillholeGA::ResumeElite()
  {
  if (this->DatabaseValid())
    {
    vtkSQLiteDatabase *db = vtkSQLiteDatabase::New();
    this->OpenDatabase(db);
    vtkSQLQuery *query = (vtkSQLiteQuery*)db->GetQueryInstance();
    std::stringstream buffer;

    //first find out if this will work
    buffer << "select count(*) from collar where hole_id = '" << this->ResumeFromC2S << "'";
    query->SetQuery(buffer.str().c_str());
    query->Execute();
    int count = query->DataValue(0).ToInt();
    if (count == 0)
      {
      vtkErrorMacro("Could not resume from database (can't find" <<this->ResumeFromC2S << ").\n");
      return Individual();
      }

    //get the start and end points
    buffer.str("");
    buffer << "select collar.id, collar.x,collar.y,collar.z, drillcurve.x, drillcurve.y, drillcurve.z from collar, drillcurve where hole_id = '" << this->ResumeFromC2S << "' and collar.id = drillcurve.collar_id and collar.x != drillcurve.x";
    query->SetQuery(buffer.str().c_str());
    query->Execute();
    DrillholeVector drillholes;
    while (query->NextRow())
      {
      int id = query->DataValue(0).ToInt();
      double start_point[3];
      double end_point[3];
      start_point[0] = query->DataValue(1).ToDouble();
      start_point[1] = query->DataValue(2).ToDouble();
      start_point[2] = query->DataValue(3).ToDouble();
      end_point[0] = query->DataValue(4).ToDouble();
      end_point[1] = query->DataValue(5).ToDouble();
      end_point[2] = query->DataValue(6).ToDouble();
      double az, dip, length;
      this->GetAzimuthDipLength(start_point, end_point, &az, &dip, &length);
      DoubleVector dv;
      dv.push_back(start_point[0]);
      dv.push_back(start_point[1]);
      dv.push_back(start_point[2]);
      dv.push_back(az);
      dv.push_back(dip);
      dv.push_back(length);
      drillholes.push_back(Drillhole(dv));
      }
    //cleanup
	  query->SetQuery("");
	  query->Execute();
	  query->Delete(); 		
    db->Close();
    db->Delete();
    //return the individual
    Individual ind(drillholes);
    this->IndividualFitness(ind);
    return ind;
    }
  else
    {
    vtkErrorMacro("Could not resume from database (invalid db).\n");
    return Individual();
    }
  }

//----------------------------------------------------------------------------
// Crossover
//----------------------------------------------------------------------------
void vtkDrillholeGA::Crossover(Population &population) 
  {
  /*
    Someone should look into adding parameters to tune the crossover 
    probabilities below. 50% in each case isn't necessarily the best.
  */
  Population selected = this->Select(population);
  Population::iterator begin = selected.begin();
  for (Population::iterator current = begin; current < selected.end(); current += 2)
    {
    //get parents
    Individual parent1 = *current;
    Individual parent2 = *(current+1);
    //create children
    for (int i = 0; i < parent1.size(); ++i)
      {
      if (this->Rand0Or1()) //50% chance drillhole or gene
        {
        //swap the drillhole
        if(this->Rand0Or1()) //50% chance of drillhole crossover
          {
          Drillhole temp = parent1[i];
          parent1[i] = parent2[i];
          parent2[i] = temp;
          }
        }
      else
        {
        //swap the genes
        for (int j = 0; j < parent1[i].size(); ++j)
          {
          if (this->Rand0Or1()) //50% chance of gene crossover
            {
            double temp = parent1[i][j];
            parent1[i][j] = parent2[i][j];
            parent2[i][j] = temp;
            }
          }
        }
      }
    }  
  }

//----------------------------------------------------------------------------
// Select
//----------------------------------------------------------------------------
Population vtkDrillholeGA::Select(Population &population)
  {
  int numParents = this->PopulationSize;
  //add an extra parent to deal with non-even population sizes. truthfully
  //there are far better ways of handling this
  if (numParents % 2 != 0)
    {
    ++numParents;
    }
  //combine the population with the elite
  Population combined = population;
  combined += *(this->Elite);
  //double totalScore = combined.sum();
  IndividualVector individuals;
  //compute the inverse scores (because we are minimizing)
  double totalScore = 0;
  double invert = combined.worst() + combined.best();
  std::vector<double> inverted_scores;
  for (Population::iterator begin = combined.begin(); begin < combined.end(); ++begin)
    {
    double inverted_score = invert - begin->score;
    inverted_scores.push_back(inverted_score);
    totalScore += inverted_score;
    }
  //select parents
  for (int i = 0; i < numParents*2; ++i)
    {
    double roulettePosition = this->Rand01() * totalScore;
    double rouletteBall = 0;
    std::vector<double>::iterator begin = inverted_scores.begin();
    for (std::vector<double>::iterator it = begin; it < inverted_scores.end(); ++it)
      {
      rouletteBall += *it;
      if (rouletteBall >= roulettePosition)
        {
        individuals.push_back(combined[it - begin]);
        break;
        }
      }
    }
  return Population(individuals);
  }

//----------------------------------------------------------------------------
// Mutation
//----------------------------------------------------------------------------
void vtkDrillholeGA::Mutation(Population &population) 
  {
  /*
    Look at all those hacks. When I first made this, we selected collar points
    from within ranges. Andrew asked that we choose them from a list. That is
    why you see the HACK comments. It would probably make more sense to handle
    mutation separately for each gene, that way mutation functions could be
    swapped out without expecting differences in API

    Note the way that the two mutation probabilities combine to enact gene changes
  */
  for (Population::iterator i = population.begin(); i < population.end(); ++i)
    {
    if (this->Rand01() < this->MutationProb + this->StagnancyMutationProb)
      {
      double mutationGeneProb = this->MutationGeneProb + this->StagnancyMutationGeneProb;
      for (Individual::iterator chromosome = i->begin(); chromosome != i->end(); ++chromosome)
        {
        Drillhole::iterator gene_begin = chromosome->begin();
        int index = rand() % this->CollarPoints.size(); //HACK
        *gene_begin = this->CollarPoints[index][0];//HACK
        ++gene_begin;//HACK
        *gene_begin = this->CollarPoints[index][1];//HACK
        ++gene_begin;//HACK
        *gene_begin = this->CollarPoints[index][2];//HACK
        ++gene_begin;//HACK
        for (Drillhole::iterator gene = gene_begin; gene < chromosome->end(); ++gene)
          {
          if (this->Rand01() < mutationGeneProb)
            {
            *gene = this->RandGene(gene - gene_begin + 3);//HACK
            }
          }
        }
      }
    }
  }

//----------------------------------------------------------------------------
// Fitness
//----------------------------------------------------------------------------
void vtkDrillholeGA::Fitness(Population &population) 
  {
  for (Population::iterator it = population.begin(); it < population.end(); ++it)
    {
    this->IndividualFitness(*it);
    }
  }

//----------------------------------------------------------------------------
// IndividualFitness
//----------------------------------------------------------------------------
void vtkDrillholeGA::IndividualFitness(Individual &individual)
  {
  //1. for each drillhole, build a pointset
  vtkPoints *points = vtkPoints::New();
  this->GetDrillholePoints(individual, points);
  
  //2. run our new neighborhood search 
  int numCells = this->InputGrid->GetNumberOfCells();
  int *found = new int[numCells];
  for (int i = 0; i < numCells; ++i)
    found[i] = 0;
  int n_dh_pts = points->GetNumberOfPoints();
  for (int i = 0; i < n_dh_pts; ++i)
    {
    double p[3];
    points->GetPoint(i, p);
    vtkIdType k = this->Locator->FindClosestPoint(p);
    double q[3];
    this->InputGrid->GetPoint(k, q);
    int row = k;
    if (this->EvaluateDist2(p,q) <= 1)
      {
      std::vector<int>::iterator begin;
      std::vector<int> row_ = this->neighbors[row];
      for (begin = row_.begin(); begin < row_.end(); ++begin)
        {
        found[*begin] = 1;
        }
      }
    }
  points->Delete();

  //3. figure out how many points were "missed" and apply that to score
  individual.score = numCells - std::count(found, found+numCells, 1);

  delete [] found;
  }
//----------------------------------------------------------------------------
// IndividualFitness - this one returns the 'found' property
//----------------------------------------------------------------------------
void vtkDrillholeGA::IndividualFitness(Individual &individual, vtkIntArray* prop)
  {
  //1. for each drillhole, build a pointset
  vtkPoints *points = vtkPoints::New();
  this->GetDrillholePoints(individual, points);
  
  //2. run our new neighborhood search 
  int numCells = this->InputGrid->GetNumberOfCells();
  int *found = new int[numCells];
  for (int i = 0; i < numCells; ++i)
    found[i] = 0;
  int n_dh_pts = points->GetNumberOfPoints();
  for (int i = 0; i < n_dh_pts; ++i)
    {
    double p[3];
    points->GetPoint(i, p);
    vtkIdType k = this->Locator->FindClosestPoint(p);
    double q[3];
    this->InputGrid->GetPoint(k, q);
    int row = k;
    if (this->EvaluateDist2(p,q) <= 1)
      {
      std::vector<int>::iterator begin;
      std::vector<int> row_ = this->neighbors[row];
      for (begin = row_.begin(); begin < row_.end(); ++begin)
        {
        found[*begin] = 1;
        }
      }
    }
  points->Delete();

  //3. figure out how many points were "missed" and apply that to score
  individual.score = numCells - std::count(found, found+numCells, 1);

  //4. return the found array
  prop->SetArray(found, numCells, false); //will delete the array for us
  }

//----------------------------------------------------------------------------
// GetEndPoint - gets the drillhole's end-point from azimuth, dip, length
//----------------------------------------------------------------------------
void vtkDrillholeGA::GetEndPoint(Drillhole &drillhole, double *xyz)
  {
  double rads = vtkMath::DoubleDegreesToRadians();
  double azr = drillhole[3] * rads;
  double dipr = drillhole[4] * rads;
  double temp = abs(drillhole[5] * cos(dipr));
  double dX = temp * cos(azr);
  double dY = temp * sin(azr);
  double dZ = drillhole[5] * sin(dipr);
  xyz[0] = drillhole[0] + dX;
  xyz[1] = drillhole[1] + dY;
  xyz[2] = drillhole[2] + dZ;
  }

//----------------------------------------------------------------------------
// GetAzimuthDipLength - gets the drillhole's azimuth, dip, length, from
//                       the start and end-point of the drillhole
//----------------------------------------------------------------------------
void vtkDrillholeGA::GetAzimuthDipLength(double *start_point, double *end_point, double *az, double *dip, double *length)
  {
  //get length
  *length = 0;
  for (int i = 0; i < 3; ++i)
    {
    *length += pow((end_point[i] - start_point[i]),2);
    }
  *length = sqrt(*length);

  //get dx, dy, dz
  double dX = end_point[0] - start_point[0];
  double dZ = end_point[2] - start_point[2];

  //to find dipr, we divide dZ by length, then take arcsin
  double dipr = asin(dZ / *length); //let's hope length != 0

  //to find azr, we divide dX by temp, then take the arccos
  double temp = abs(*length * cos(dipr));
  double azr = acos(dX / temp);

  //convert to degrees, check sign
  double degs = vtkMath::DoubleRadiansToDegrees();
  *az = azr * degs;
  if (end_point[1] < start_point[1])
    {
    *az = -(*az);
    }
  *dip = dipr * degs;
  }

//----------------------------------------------------------------------------
// GetDrillholePoints - generates the points that are spaced by SegmentLength
//                      per drillhole
//----------------------------------------------------------------------------
void vtkDrillholeGA::GetDrillholePoints(Individual &individual, vtkPoints *points)
  {
  for (Individual::iterator drillhole = individual.begin(); drillhole < individual.end(); ++drillhole)
    {
    Drillhole d = *drillhole;
    //get start, end, number of points
    double startpoint[3];
    startpoint[0] = d[0];
    startpoint[1] = d[1];
    startpoint[2] = d[2];
    double endpoint[3];
    this->GetEndPoint(d, endpoint);
    int num_points = (int) ceil(d[5] / this->SegmentLength);
    //create normalized direction vector
    double dir_vec_norm[3];
    for (int i = 0; i < 3; ++i)
      {
      dir_vec_norm[i] = (endpoint[i] - startpoint[i]) / d[5];
      }
    for (int i = 0; i < num_points; ++i)
      {
      points->InsertNextPoint(startpoint);
      for (int j = 0; j < 3; ++j)
        {
        startpoint[j] += this->SegmentLength * dir_vec_norm[j];
        }
      }
    //hrmm we could be duplicating a point here
    points->InsertNextPoint(endpoint);
    }
  }

//----------------------------------------------------------------------------
// GetDrillholeEndPoints - make the endpoints of all drillholes in population
//----------------------------------------------------------------------------
void vtkDrillholeGA::GetDrillholeEndPoints(Population &p, vtkPoints *points)
  {
  for (Population::iterator i = p.begin(); i < p.end(); ++i)
    {
    this->GetDrillholeEndPoints(*i, points);
    }
  }
//----------------------------------------------------------------------------
// GetDrillholeEndPoints - make the endpoints of all drillholes in individual
//----------------------------------------------------------------------------
void vtkDrillholeGA::GetDrillholeEndPoints(Individual &i, vtkPoints *points)
  {
  for (Individual::iterator d = i.begin(); d < i.end(); ++d)
      {
      Drillhole d_ = *d;
      double endpoint[3];
      this->GetEndPoint(d_, endpoint);
      //hrmm we could be duplicating a point here
      points->InsertNextPoint(d_[0], d_[1], d_[2]);
      points->InsertNextPoint(endpoint);
      }
  }

//----------------------------------------------------------------------------
// Stagnancy - if the best hasn't been updated, then change mutation modifiers
//             else, reset them to 0
//             This stuff needs major tuning time
//----------------------------------------------------------------------------
void vtkDrillholeGA::Stagnancy(Population &population) 
  {
  if (population > *(this->Elite))
    {
    this->StagnancyMutationProb = 0;
    this->StagnancyMutationGeneProb = 0;
    }
  else
    {
    this->StagnancyMutationProb = 0.4;
    this->StagnancyMutationGeneProb = 0;
    }
  }

//----------------------------------------------------------------------------
// LengthHillClimb - try to make the holes longer for better score, then
//                   try to make the holes shorter for same score
//----------------------------------------------------------------------------
void vtkDrillholeGA::LengthHillClimb()
  {
  //get the best individual 
  Individual individual = *(this->Elite->best_element()); // this is a copy now
  double last_score = individual.score;
  double last_length = 0;

  //Step 1/2: check whether you should bother maxing.
  //we don't update last_score, because we want to see independently 
  //for each drillhole whether it is better than last_score
  int c = 0;
  int g = 5; //TODO: the chromosome indices should be "global" consts or enums or something
  for (; c < individual.size(); ++c)
    {
    last_length = individual[c][g];
    individual[c][g] = this->ChromosomeRangeMax[g];
    this->IndividualFitness(individual);
    //if the score is worse or the same, reset length
    if (individual.score >= last_score)
      {
      individual[c][g] = last_length;
      }
    }

  //Step 2/2: we decrease the length to see if we get the same score
  c = 0;
  while (c < this->IndividualSize)
    {
    //record last_length and last_score in case decreasing length was no good
    last_length = individual[c][g];
    last_score = individual.score;
    //increase the length by seg length
    individual[c][g] -= this->HillClimbLength;
    //if it's now out of range, use max range
    bool last_chance = false;
    if (individual[c][g] < this->HillClimbLength)
      {
      last_chance = true;
      individual[c][g] = this->HillClimbLength;
      }
    //get fitness
    this->IndividualFitness(individual);
    //if it's worse than before then reset length and move on
    if (individual.score > last_score)
      {
      individual.score = last_score;
      individual[c][g] = last_length;
      ++c;
      continue; //i know, but just in case I add code afterward
      }
    else
      {
      if (last_chance)
        {
        ++c;
        continue;
        }
      }
    }
  (*this->Elite)[this->Elite->best_element() - this->Elite->begin()] = individual;
  }

//----------------------------------------------------------------------------
// DirectionHillClimb - try various locations around current dip/azimuth
//                      to get a better score
//----------------------------------------------------------------------------
void vtkDrillholeGA::DirectionHillClimb()
  {
  //This is not exactly a perfect attempt at hill climbing because we're hill
  //climbing on a set of neighbours instead of one neighbour at a time
  int g1 = 3; //azimuth
  int g2 = 4; //dip
  for (int c = 0; c < this->IndividualSize; ++c)
    {
    Individual old_individual = *(this->Elite->best_element()); // this is a copy now
    IndividualVector new_population;
    double old_az = old_individual[c][g1];
    double old_dip = old_individual[c][g2];
    double lo_az = std::max(this->ChromosomeRangeMin[g1], old_az - this->HillClimbAzimuthWindow);
    double hi_az = std::min(this->ChromosomeRangeMax[g1], old_az + this->HillClimbAzimuthWindow);
    double lo_dip = std::max(this->ChromosomeRangeMin[g2], old_dip - this->HillClimbDipWindow);
    double hi_dip = std::min(this->ChromosomeRangeMax[g2], old_dip + this->HillClimbDipWindow);
    for (double az = lo_az; az <= hi_az; ++az) 
      {
      for (double dip = lo_dip; dip <= hi_dip; ++dip) 
        {
        Individual new_individual = old_individual; //this is a copy now
        new_individual[c][g1] = az;
        new_individual[c][g2] = dip;
        this->IndividualFitness(new_individual);
        new_population.push_back(new_individual);
        }
      }
    this->Elite->Update(Population(new_population));
    }
  }

//----------------------------------------------------------------------------
// WriteElite - Write out the elite list to a file (probably .dhd)
//----------------------------------------------------------------------------
int vtkDrillholeGA::WriteElite()
  {
	if (this->DatabaseValid())
    {
    vtkSQLiteDatabase *db = vtkSQLiteDatabase::New();
    this->OpenDatabase(db);
    //retrieve the drillhole endpoints
    vtkPoints *points = vtkPoints::New();
    Individual individual = *(this->Elite->best_element());
    double score = individual.score;
    this->GetDrillholeEndPoints(individual, points);
    //create a query instance
    vtkSQLQuery *query = (vtkSQLiteQuery*)db->GetQueryInstance();
    time_t t = time(NULL);
    const int POINTS_PER_HOLE = 2;
    for (int i = 0; i < points->GetNumberOfPoints(); i+=POINTS_PER_HOLE)
      {
      //STEP 0/3: figure out what you can call the individual
      std::stringstream name;
      name << "'DHGA_score_" << score 
        << "_params(pop_" << this->PopulationSize << ",pass_" << this->NumPasses << ",gen_" << this->NumGenerations << ",dhs_" << this->IndividualSize << ")" << "_ellipsoid(ran_" << this->Ranges[0] << "," << this->Ranges[1] << "," << this->Ranges[2] << ",ang_" << this->Angles[0] << "," << this->Angles[1] << "," << this->Angles[2] << ")_" << t << "'";
      
      //STEP 1/3: collar point (start point)
      //write an insert query
      double point[3];
      points->GetPoint(i, point);
      std::stringstream buffer;
	    buffer << "insert into Collar(hole_id,x,y,z,length,valid) values (" << name.str() << "," << point[0] << "," << point[1] << "," << point[2] << ",0,0)";
      query->SetQuery(buffer.str().c_str());
      //execute the query
      query->Execute();

      //STEP 2/3: drillcurve point (start point)
      //write an insert query
      buffer.str("");
      buffer << "insert into DrillCurve(collar_id,x,y,z) values ((select max(id) from collar)," << point[0] << "," << point[1] << "," << point[2] << ")";
      query->SetQuery(buffer.str().c_str());
      //execute the query
      query->Execute();

      //STEP 3/3: drillcurve point (end point)
      //write an insert query
      points->GetPoint(i+1,point);
      buffer.str("");
      buffer << "insert into DrillCurve(collar_id,x,y,z) values ((select max(id) from collar)," << point[0] << "," << point[1] << "," << point[2] << ")";
      query->SetQuery(buffer.str().c_str());
      //execute the query
      query->Execute();
      }
    //cleanup
	  query->SetQuery("");
	  query->Execute();
	  query->Delete(); 		
    points->Delete();
    db->Close();
    db->Delete();
    }
  else
    {
    vtkErrorMacro(<<"Could not open database to write elite list.");
    } 
  return 1;
  }

//----------------------------------------------------------------------------
// OpenDatabase - Tries to open the SQLite Database
// Returns
// 0 if it fails to open
// 1 if it succeeds to open
//----------------------------------------------------------------------------
int vtkDrillholeGA::OpenDatabase(vtkSQLiteDatabase* db)
  {
  db->SetDatabaseFileName(this->DrillholeDatabase);
  bool status = db->Open("");
  if (!status)
    {
    vtkErrorMacro("Couldn't open database (1).");
    return 0;
    }
  if (!db)
    {
    vtkErrorMacro("Couldn't open database (2).");
    return 0;
    }
  return 1;
  }

// -----------------------------------------------------------------------------
// FileNameValid - Detects errors including
//                  FileName == 0
//                  len(FileName) == 0
//                  FileName is not a valid file path
// -----------------------------------------------------------------------------
int vtkDrillholeGA::FileNameValid()
  {
  // Make sure we have a file to read.
  if(!this->DrillholeDatabase)
    {
    vtkErrorMacro("A Drillhole Database must be specified.");
    return 0;
    }
  if(strlen(this->DrillholeDatabase)==0)
    {
    vtkErrorMacro("Drillhole Database was NULL.");
    return 0;
    }

  // Test to see if the input file exists
  // Use ifstream to remove any warnings about fopen
  ifstream fin;
  fin.open(this->DrillholeDatabase, ios::in);
  if(fin.fail())
    {
    vtkErrorMacro("Error in file path: " << this->DrillholeDatabase);
    fin.close();
    return 0;
    }
  fin.close();
  return 1;
  }

// -----------------------------------------------------------------------------
// DatabaseValid - Determines whether the database specified by the user is a
//                 valid drillhole database (dhd) file.
// -----------------------------------------------------------------------------
int vtkDrillholeGA::DatabaseValid()
  {
  // make sure the file name actually points to a file
  if (!this->FileNameValid())
    {
    return 0;
    }
  // try to open the database
  vtkSQLiteDatabase *db = vtkSQLiteDatabase::New();
  if (!this->OpenDatabase(db))
    {
    db->Delete();
    return 0;
    }
  //make sure it's a drillhole database
  vtkSQLQuery *cursor = db->GetQueryInstance();
  cursor->SetQuery("select id from drillcurve");
  int readable = cursor->Execute(); //TODO: this is a poor test
  //cleanup
  cursor->SetQuery("");
  cursor->Execute();
  cursor->Delete();
  db->Close();
  db->Delete();
  return readable;
  }

//----------------------------------------------------------------------------
// Rand01 - generate pseudo-random numbers between closed [0,1] range
//          i.e., 0 and 1 are possibilities.
//----------------------------------------------------------------------------
double vtkDrillholeGA::Rand01()
  {
  return (double)rand() / (double)RAND_MAX;
  }

//----------------------------------------------------------------------------
// Rand0Or1 - generate pseudo-randomly either 0 or 1
//----------------------------------------------------------------------------
double vtkDrillholeGA::Rand0Or1()
  {
  return (int)(this->Rand01() + 0.5);
  }

//----------------------------------------------------------------------------
// RandGene - Returns a new gene between [min, max], given a 'gene index'
//----------------------------------------------------------------------------
double vtkDrillholeGA::RandGene(int index)
  {
  double min = this->ChromosomeRangeMin[index];
  double max = this->ChromosomeRangeMax[index];
  return this->Rand01() * (max - min) + min;
  }



//----------------------------------------------------------------------------
// SetMetric
//----------------------------------------------------------------------------
void vtkDrillholeGA::SetMetric()
  {
  /*
    Marie-Gabrielle is the one you should talk to if you have trouble with this
  */
  //convert angles to radians
  double *angleRads = new double[3];
  double factor = vtkMath::DoublePi() / 180.0;
  angleRads[0] = this->Angles[0] * factor;
  angleRads[1] = this->Angles[1] * factor;
  angleRads[2] = this->Angles[2] * factor;

  //make rotation matrix
  double *rot = new double[16];
  this->MakeRotMatrix( angleRads[1], angleRads[2], angleRads[0], rot);
  vtkMatrix4x4 *rotMatrix = vtkMatrix4x4::New();
  rotMatrix->DeepCopy(rot);

  //make transpose of rotation matrix
  vtkMatrix4x4 *inv = vtkMatrix4x4::New();
  vtkMatrix4x4::Transpose(rotMatrix, inv);

  //make scale matrix
  vtkMatrix4x4 *sca = vtkMatrix4x4::New();
  sca->Identity();
  vtkTransform *transform = vtkTransform::New();
  transform->SetMatrix(sca);
  transform->Scale(1.0 / (this->Ranges[0] * this->Ranges[0]), 
                   1.0 / (this->Ranges[1] * this->Ranges[1]), 
                   1.0 / (this->Ranges[2] * this->Ranges[2]));
  vtkMatrix4x4 *scaleMatrix = transform->GetMatrix();

  //make the matrix for tensor
  vtkMatrix4x4 *temp = vtkMatrix4x4::New();
  vtkMatrix4x4 *for_tensor = vtkMatrix4x4::New();
  vtkMatrix4x4::Multiply4x4(rotMatrix, scaleMatrix, temp);
  vtkMatrix4x4::Multiply4x4(temp, inv, for_tensor);


  this->MetricTensor[0] = for_tensor->GetElement(0,0);
  this->MetricTensor[1] = for_tensor->GetElement(0,1);
  this->MetricTensor[2] = for_tensor->GetElement(0,2);
  this->MetricTensor[3] = for_tensor->GetElement(1,1);
  this->MetricTensor[4] = for_tensor->GetElement(1,2);
  this->MetricTensor[5] = for_tensor->GetElement(2,2);

  //cleanup
  delete [] angleRads;
  delete [] rot;
  rotMatrix->Delete();
  inv->Delete();
  sca->Delete();
  transform->Delete();
  //scaleMatrix->Delete();//this was a reference from 'transform', so dont del
  temp->Delete();
  for_tensor->Delete();

  }

//----------------------------------------------------------------------------
// MakeRotMatrix
//  the rotation moves the xyz axis onto the ellipsoid axis 
//----------------------------------------------------------------------------
void vtkDrillholeGA::MakeRotMatrix(double ax, double ay, double az, double *m)
  {
  /*
    Marie-Gabrielle is the one you should talk to if you have trouble with this
  */
  // Assume az = azimuth, ay = rake, ax = dip, as defined in SGeMS
  //  - rotation around Z, angle pi/2, to initialize as SGeMS do,
  //  - First rotation in SGeMS  around Z', angle alpha = -azimuth
  //  - Second rotation in SGeMS around X', angle beta  = -dip
  //  - Third rotation in SGeMS  around Y', angle theta = plunge
  //
  // Maxima commands
  // Rx(a) := matrix([1,0,0],[0,cos(a),-sin(a)],[0,sin(a),cos(a)]);
  // Ry(b) := matrix([cos(b), 0, -sin(b)],[0,1,0],[sin(b),0,cos(b)]);
  // Rz(a) := matrix([cos(a), -sin(a),0],[sin(a),cos(a),0],[0,0,1]);
  // Ry(ay).Rx(-ax).Rz(-az).Rz(%pi/2);
  double cx,sx,cy,sy,cz,sz,cycz,cysz;
  cx = cos(ax);
  cy = cos(ay);
  cz = cos(az);
  sx = sin(ax);
  sy = sin(ay);
  sz = sin(az);
  cycz = cy*cz;
  cysz = cy*sz;
#ifdef COLUMN_ORDER
  m[0]  = sx*sy*cz + cysz;  
  m[4]  = cx*cz;
  m[8]  = sy*sz - sx*cycz;
  m[12] = 0.0;
  m[1]  = sx*sy*sz - cycz;
  m[5]  = cx*sz;
  m[9]  = - sy*cz - sx*cysz;
  m[13] = 0.0;
  m[2]  = - cx*sy;
  m[6]  = sx;
  m[10] = cx*cy;
  m[14] = 0.0;
  m[3]  = 0.0;
  m[7]  = 0.0;
  m[11] = 0.0;
  m[15] = 1.0;
#else
  m[0]  = sx*sy*cz + cysz; 
  m[1]  = cx*cz;	   
  m[2]  = sy*sz - sx*cycz; 
  m[3]  = 0.0;		   
  m[4]  = sx*sy*sz - cycz; 
  m[5]  = cx*sz;	   
  m[6]  = - sy*cz - sx*cysz;
  m[7]  = 0.0;		   
  m[8]  = - cx*sy;	   
  m[9]  = sx;		   
  m[10] = cx*cy;	   
  m[11] = 0.0;		   
  m[12] = 0.0;		   
  m[13] = 0.0;		   
  m[14] = 0.0;		   
  m[15] = 1.0;		   
#endif
  }

//----------------------------------------------------------------------------
// EvaluateDist2 - gives you a squared ellipsoidal distance between points
//                 less than 1 means inside the ellipsoid
//----------------------------------------------------------------------------
double  vtkDrillholeGA::EvaluateDist2( double* P, double* Q)
  {
  /*
    Marie-Gabrielle is the one you should talk to if you have trouble with this
  */
  double dx = Q[0] - P[0];
  double dy = Q[1] - P[1];
  double dz = Q[2] - P[2];
  double* met = this->MetricTensor;

  double ret = met[0]*dx*dx + met[3]*dy*dy + met[5]*dz*dz 
    + 2.* ( met[1]*dx*dy + met[2]*dx*dz + met[4]*dy*dz );

  return ret;
  }