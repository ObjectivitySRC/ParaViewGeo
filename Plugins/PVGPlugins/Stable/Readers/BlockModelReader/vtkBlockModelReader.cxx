#include "vtkBlockModelReader.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <ostream>
#include <cmath>
#include <limits>

#include "vtkDataArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkStructuredPoints.h"
#include "vtkCellData.h"
#include "vtkDoubleArray.h"

#define COORD_VALUES 3
#define CSV_SEPARATOR ","

#define EMPTY -1
#define X 0
#define Y 1
#define Z 2

vtkCxxRevisionMacro(vtkBlockModelReader, "$Revision: 1.3 $");
vtkStandardNewMacro(vtkBlockModelReader);

// Constructor
vtkBlockModelReader::vtkBlockModelReader()
  {
  this->FileName = NULL;
  this->SetNumberOfInputPorts(0);
  this->SetNumberOfOutputPorts(1);
  this->HeaderExists = 0;
  };

// --------------------------------------
// Destructor
vtkBlockModelReader::~vtkBlockModelReader()
  {
  this->SetFileName(0);
  }

// --------------------------------------
void vtkBlockModelReader::PrintSelf(ostream& os, vtkIndent indent)
  {
  this->Superclass::PrintSelf(os,indent);
  os << indent << "FileName: "
     << (this->FileName ? this->FileName : "(none)") << "\n";
  }


//---------------------------------------
int vtkBlockModelReader::CanReadFile( const char* fname )
{
  vtkStdString line;
  ifstream myfile (fname);
  int goodFile=0;
  if (myfile.is_open())
    {   
    //check the first 5 lines, make sure they all have the same length.    
    int counter = 0;    
    int len = 0;
    while (! myfile.eof() && counter < 5 ) //make sure the line exists
      {      
      getline (myfile,line);
      if (line.size() > 0)
        { 
        if (counter == 0)
					{       
					len = this->numberOfValuesOnLine(line);
					}
				else
					{
					//increment if the line is good
					if (len == this->numberOfValuesOnLine(line) )
						{
						goodFile++;
						}
					}        
        }
      counter++;      
      }  
    }
	return (goodFile >= 4 );
}

// --------------------------------------
int vtkBlockModelReader::RequestInformation(vtkInformation* request,
                          vtkInformationVector** inputVector,
                          vtkInformationVector* outputVector)
  {
  return 1;
  }

// --------------------------------------
int vtkBlockModelReader::RequestData(vtkInformation* request,
                          vtkInformationVector** inputVector,
                          vtkInformationVector* outputVector)
  {
  // Make sure we have a file to read.
  if(!this->FileName)
    {
    vtkErrorMacro("A FileName must be specified.");
		return 0;
    }
  if(strlen(this->FileName)==0)
    {
    vtkErrorMacro("A NULL FileName.");
		return 0;
    }

  vtkInformation *outInfo = outputVector->GetInformationObject(0);
  vtkStructuredPoints *output = vtkStructuredPoints::SafeDownCast(
      outInfo->Get(vtkDataObject::DATA_OBJECT()));

  vtkStdString line;
  ifstream myfile (this->FileName);
  double *coords;
  double firstValues[COORD_VALUES] = {0,0,0}; 
  bool flagFirst = false; 
  int firstCounts[COORD_VALUES] = {1,1,1}; 
  int counter = 0; // line count
  
    //storage for the min and max values for X,Y,Z
  double minValues[COORD_VALUES] = {VTK_INT_MAX,VTK_INT_MAX,VTK_INT_MAX};
  double maxValues[COORD_VALUES] = {VTK_INT_MIN,VTK_INT_MIN,VTK_INT_MIN};
  
  //holds all the header names
  vtkStdString *headerNames = NULL;
  
  int numVals = 0;
  int propVals = 0;
  if (myfile.is_open())
    {
    bool headerRead = false;
    while (! myfile.eof() ) //make sure the line exists
      {      
      getline (myfile,line);
      if (line.size() > 0)
        {
        if (headerRead == false) 
          {
          numVals = this->numberOfValuesOnLine(line);
          propVals = numVals - COORD_VALUES;
          headerRead = true;
          if (this->HeaderExists == true)
            {
  //        we have a header so parse it
            headerNames = this->headerNames(line, propVals);
            getline(myfile, line);
            }
          else
            {
  //        we do not have a header
            headerNames = new vtkStdString[propVals];
            for (int i = 0; i < propVals; i++)
              {
              std::ostringstream s1;
              s1 << "Property " << i;
              headerNames[i] = s1.str();
              //cout << headerNames[i] << endl;
              }
            }           
          }
        
        //dont read in whitespace lines
        coords = parseCoords(line);
        minValues[X] = this->min(coords[X], minValues[X]);
        maxValues[X] = this->max(coords[X], maxValues[X]);
        minValues[Y] = this->min(coords[Y], minValues[Y]);
        maxValues[Y] = this->max(coords[Y], maxValues[Y]);
        minValues[Z] = this->min(coords[Z], minValues[Z]);
        maxValues[Z] = this->max(coords[Z], maxValues[Z]);
    
        counter++;   
        
        if (flagFirst == false)
          {
          firstValues[X] = coords[X];
          firstValues[Y] = coords[Y];
          firstValues[Z] = coords[Z];
          flagFirst = true;
          }
        else
          {
          if (coords[X] == firstValues[X]) 
            firstCounts[X]++;
          if (coords[Y] == firstValues[Y]) 
            firstCounts[Y]++;
          if (coords[Z] == firstValues[Z]) 
            firstCounts[Z]++;
          }                            
             
        }   
      }
    }
  //cleanup file and coords
  myfile.close();
  delete [] coords;
    
  
  //max plus min gets us the number of cells in each direction
  int *numCells = new int[COORD_VALUES];
  numCells[X] = counter / firstCounts[X];
  numCells[Y] = counter / firstCounts[Y];
  numCells[Z] = counter / firstCounts[Z];

  // the min + max / totals cells gets us the size of each cell in x,y,z
  double *spacing = new double[COORD_VALUES];
  spacing[X] = fabs(minValues[X] - maxValues[X]) / (numCells[X] - 1);
  spacing[Y] = fabs(minValues[Y] - maxValues[Y]) / (numCells[Y] - 1);
  spacing[Z] = fabs(minValues[Z] - maxValues[Z]) / (numCells[Z] - 1);

  output->SetDimensions(numCells[X]+1, numCells[Y]+1, numCells[Z]+1);
  output->SetWholeExtent(0, numCells[X], 0, numCells[Y], 0, numCells[Z]);
  output->SetOrigin(minValues[X], minValues[Y], minValues[Z]);
  output->SetSpacing(spacing[X], spacing[Y], spacing[Z]);
  output->AllocateScalars();

  double **m = new double*[counter]; //2d array

  ifstream myfile2 (this->FileName);
  int currentLine = 0;
  if (myfile2.is_open())
    {
    if (this->HeaderExists)
      {
      getline(myfile2, line); // avoid the header line
      }
    while (! myfile2.eof() ) //make sure the line exists
      { 
      getline (myfile2,line);
      if (line.size() > 0)
        { //dont read in whitespace lines
        double *coords = parseCoords(line); //1d array
        
        int *index = new int[COORD_VALUES];
        index[X] = int(fabs(coords[X] - minValues[X]) / spacing[X]);
        index[Y] = int(fabs(coords[Y] - minValues[Y]) / spacing[Y]);
        index[Z] = int(fabs(coords[Z] - minValues[Z]) / spacing[Z]);
        int cellIndex = index[X] + ( numCells[X] * ( index[Y] + ( numCells[Y] * ( index[Z] ) ) ) );
        m[cellIndex] = parseProperties(line, propVals);
        
        
        currentLine++; 
        delete [] index;
        delete [] coords;
        }
      }
    }
  
  //now we have a 2d array in m
  //rows are indexed by cell #
  //columns are indexed by property #
  //but paraview wants us to add arrays where the row is a property
  //and the column is the cell
  for (int i = 0; i < propVals; i++)
    {
    vtkDoubleArray *cell = vtkDoubleArray::New();
    //set the property name to what we found in the header
    cell->SetName(headerNames[i]);
    for (int j = 0; j < counter; j++)
      {
      cell->InsertNextValue(m[j][i]);
      }
    output->GetCellData()->AddArray(cell);
    }
  
  //take out the trash
  for (int i = 0; i < counter; i++)
    {
    delete [] m[i];
    }
  
  delete [] m; 
  delete [] numCells;
  delete [] spacing; 
  return 1;
  }

/*
  returns the max between one and two
  count increments when a "new" max has been set
  we assume one is the variable holding the current maximum
  and two is the next variable being compared against one
*/
double vtkBlockModelReader::max(double one, double two)
  {
  if (one > two)
    {
    return one;
    }
  else
    {
    return two;
    }
  }

/*
  returns the min between one and two
  count increments when a "new" min has been set
  we assume one is the variable holding the current minimum
  and two is the next variable being compared against one
*/
double vtkBlockModelReader::min(double one, double two)
  {
  if (one < two)
    {
    return one;
    }
  else
    {
    return two;
    }
  }

double* vtkBlockModelReader::parseCoords(vtkStdString line)
  {
  vtkStdString::size_type loc;
  double *values = new double[COORD_VALUES];
  int counter = 0;
  do
    {
    loc = line.find(CSV_SEPARATOR, 0);
    
    fromString(line.substr(0, loc), values[counter++]);
    line = line.substr(loc+1);
    } while(counter < COORD_VALUES);
  return values;
  }

vtkStdString* vtkBlockModelReader::headerNames(vtkStdString line, int numValues)
  {
  vtkStdString::size_type loc;
  vtkStdString *values = new vtkStdString[numValues];
  int counter = 0;
  for (int i = 0; i < COORD_VALUES; i++)
    {
    line = line.substr(line.find(CSV_SEPARATOR, 0) + 1);
    }
  do
    {
    loc = line.find(CSV_SEPARATOR, 0);
    if (!(loc == -1)) //if it actually found a comma
      {
      values[counter++] = line.substr(0,loc);
      line = line.substr(loc+1);
      }
    else
      {
      values[counter++] = line;
      }
    } while(counter < numValues);
  return values;
  }

int vtkBlockModelReader::numberOfValuesOnLine(vtkStdString line)
  {
  //count how many values are even on the line
  vtkStdString::size_type loc;
  int occur = 0;
  vtkStdString tempLine = line.substr(0, line.size() - 1);
  do
    {
    loc = tempLine.find(CSV_SEPARATOR, 0);
    if (!(loc == -1)) // if it actually found a comma
      {
      occur++;
      tempLine = tempLine.substr(loc + 1);
      }
    else
      {
      break;
      }
    
    } while (true);
  return occur + 1;
  }

double* vtkBlockModelReader::parseProperties(vtkStdString line, int numValues)
  {
  //one more value than the number of csv separators found

  double* values = new double[numValues]; 
  vtkStdString::size_type loc2;
  int counter = 0;
  for (int i = 0; i < COORD_VALUES; i++)
    {
    line = line.substr(line.find(CSV_SEPARATOR, 0) + 1);
    }
  do
    {
    loc2 = line.find(CSV_SEPARATOR, 0);
    if (!(loc2 == -1))
      {
      fromString(line.substr(0, loc2), values[counter++]);
      line = line.substr(loc2 + 1);
      }
    else
      {
      fromString(line, values[counter++]);
      }
    } while(counter < numValues);
  return values;
  }

void vtkBlockModelReader::fromString(const vtkStdString &s, double &result)
  {
  std::istringstream stream(s);
  stream >> result;
  }
