
#include "vtkAbriToSotWriter.h"
#include "vtkInformation.h"
#include "vtkObjectFactory.h"
#include "vtkPolyData.h"
#include "vtkDataArray.h"
#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkPointData.h"
#include "vtkStringArray.h"
#include "vtkSmartPointer.h"
#include "vtkStdString.h"
#include "vtkCellDataToPointData.h"
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <iomanip>
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include <vtkstd/list>
#include "vtkDoubleArray.h"
#include "vtkIntArray.h"
#include "vtkStdString.h"
#include <vtkstd/vector>
extern "C"
{
	#include "CLIPS/clips.h"
}
using namespace std;

extern "C"
{
	int printDependency();
}
ofstream myFile;


struct dependenciesStruct
{
	list<vtkStdString> dependenciesList;
};
dependenciesStruct dependencies;


typedef vector<list<NSCoords>> vector_ListOfInt;

class vtkNeighboursVect: public vector_ListOfInt
{};


vtkCxxRevisionMacro(vtkAbriToSotWriter, "$Revision: 1.27 $");
vtkStandardNewMacro(vtkAbriToSotWriter);

//-----------------------------------------------------------------------------------
vtkAbriToSotWriter::vtkAbriToSotWriter()
{
	StartDate=NULL;
	Duration=1;	
	DaysByMeter=0.1;
	HzDByMeter= 0.1;
	StrikePosition = NULL;
	ScriptPath = NULL;
	StartingStope = 0;
	BlockSize[0] = 5.0;
	BlockSize[1] = 5.0;
	BlockSize[2] = 5.0;
	this->numberOfStopes = 0;
	this->generateDependencies = 0;

	inPoints=NULL;
	stopePointsList= NULL;
	StopeCenterXArray= NULL;
	StopeCenterYArray= NULL;
	StopeCenterZArray= NULL;
	TypeOfBlocks = 0;
}

//-----------------------------------------------------------------------------------
vtkAbriToSotWriter::~vtkAbriToSotWriter()
{ 
}


//-----------------------------------------------------------------------------------
void split(const vtkStdString& str,	vector<vtkStdString>& tokens,
							const vtkStdString& delimiters = " ")
{
	if( !str.length() )
		return;
	tokens.clear();
	// Skip delimiters at beginning.
	vtkStdString::size_type lastPos = 0;
	// Find first "non-delimiter"
	vtkStdString::size_type pos     = str.find_first_of(delimiters, lastPos);

	while (vtkStdString::npos != pos)
	{
		// Found a token, add it to the vector.
		tokens.push_back(str.substr(lastPos, pos - lastPos));
		// Skip delimiters.  Note the "not_of"
		lastPos = pos+1;
		// Find next "non-delimiter"
		pos = str.find_first_of(delimiters, lastPos);
	}
	tokens.push_back( str.substr(lastPos, str.length()-lastPos) );
}


//-----------------------------------------------------------------------------------
double returnDays(char* date)
{
	TimeConverter *timeC= new TimeConverter();
  vtkstd::vector<vtkStdString> DateList_vector;
	vtkStdString Date_str(date);
	split(Date_str,DateList_vector,"-");

	timeC->setDay(atoi( (DateList_vector[2]).c_str()));
	timeC->setMonth(atoi( (DateList_vector[1]).c_str()));
	timeC->setYear(atoi( (DateList_vector[0]).c_str()));
	double n= timeC->GetMSDate()+1;
	delete timeC;	
	return n;
}
//-----------------------------------------------------------------------------------

char* getDate(double days)
{	
	TimeConverter *timeC= new TimeConverter("%y-%m-%d");
	timeC->Parse(days, TimeConverter::MSDATE);

	char* date;
	date= timeC->GetISODate();
	return date;
}


/*******************************************************************************/
/* Name: ComputeStopesCenter                                                   */
/*                                                                             */
/* Description: compute the coordonnates of the center of each stope           */
/*                                                                             */
/* parameters: none                                                            */
/*                                                                             */
/* return value: none                                                          */
/*******************************************************************************/
void vtkAbriToSotWriter::ComputeStopesCenter()
{
	double coord[3];
	double Xmin,Xmax,Ymin,Ymax,Zmin,Zmax;
	this->StopeCenterXArray= vtkDoubleArray::New();
	this->StopeCenterXArray->Allocate(this->numberOfStopes);

	this->StopeCenterYArray= vtkDoubleArray::New();
	this->StopeCenterYArray->Allocate(this->numberOfStopes);

	this->StopeCenterZArray= vtkDoubleArray::New();
	this->StopeCenterZArray->Allocate(this->numberOfStopes);

	for(int n=0; n<this->numberOfStopes; n++)
	{
		if(this->stopePointsList[n]->GetNumberOfTuples()>0)
		{
			inPoints->GetPoint(this->stopePointsList[n]->GetTuple1(0), coord);				
			Xmin=coord[0]; Xmax=coord[0]; 
			Ymin=coord[1]; Ymax=coord[1]; 
			Zmin=coord[2]; Zmax=coord[2];	
		
			for(int p=0; p<this->stopePointsList[n]->GetNumberOfTuples(); p++)
			{
				inPoints->GetPoint(this->stopePointsList[n]->GetTuple1(p), coord);

				if(coord[0]<Xmin)
					Xmin= coord[0];
				if(coord[0]>Xmax)
					Xmax= coord[0];

				if(coord[1]<Ymin)
					Ymin= coord[1];
				if(coord[1]>Ymax)
					Ymax= coord[1];

				if(coord[2]<Zmin)
					Zmin= coord[2];
				if(coord[2]>Zmax)
					Zmax= coord[2];			
			}

			this->StopeCenterXArray->InsertTuple1(n,(Xmin+(Xmax-Xmin)/2) );
			this->StopeCenterYArray->InsertTuple1(n,(Ymin+(Ymax-Ymin)/2) );
			this->StopeCenterZArray->InsertTuple1(n,(Zmin+(Zmax-Zmin)/2) );
		}
	}
}


//========================================================================================
bool compare_nocase(NSCoords first, NSCoords second)
{		
	if(first.id < second.id)
		return true;
	else return false;
}

bool compare_nocase1(NSCoords first, NSCoords second)
{		
	if(first.id == second.id)
		return true;
	else return false;
}


/*******************************************************************************/
/* Name: ComputeStopesCenter                                                   */
/*                                                                             */
/* Description: compute the coordonnates of the center of each stope           */
/*                                                                             */
/* parameters: none                                                            */
/*                                                                             */
/* return value: none                                                          */
/*******************************************************************************/
void vtkAbriToSotWriter::ComputeNeighbours_Ir( vtkNeighboursVect* Nx, 
																							 vtkNeighboursVect* Ny,
																							 vtkNeighboursVect* Nz )
{
	 vtkDoubleArray* stopeXminArray = vtkDoubleArray::New();
	 stopeXminArray= vtkDoubleArray::SafeDownCast(this->input->GetPointData()->GetArray("Stope Xmin"));

	 vtkDoubleArray* stopeXmaxArray = vtkDoubleArray::New();
	 stopeXmaxArray= vtkDoubleArray::SafeDownCast(this->input->GetPointData()->GetArray("Stope Xmax"));   

	 vtkDoubleArray* stopeYminArray = vtkDoubleArray::New();
	 stopeYminArray= vtkDoubleArray::SafeDownCast(this->input->GetPointData()->GetArray("Stope Ymin"));

	 vtkDoubleArray* stopeYmaxArray = vtkDoubleArray::New();
	 stopeYmaxArray= vtkDoubleArray::SafeDownCast(this->input->GetPointData()->GetArray("Stope Ymax"));

	 vtkDoubleArray* stopeZminArray = vtkDoubleArray::New();
	 stopeZminArray= vtkDoubleArray::SafeDownCast(this->input->GetPointData()->GetArray("Stope Zmin"));

	 vtkDoubleArray* stopeZmaxArray = vtkDoubleArray::New();
	 stopeZmaxArray= vtkDoubleArray::SafeDownCast(this->input->GetPointData()->GetArray("Stope Zmax"));
	
	 vtkIntArray* p1StopeIdArray= vtkIntArray::New();
	 p1StopeIdArray= vtkIntArray::SafeDownCast(this->input->GetPointData()->GetArray("Stope Id")); 

	 double range[2];
	 p1StopeIdArray->GetRange(range);
	 int nbStopes = range[1]-range[0];
	 int numberOfCells = this->input->GetNumberOfCells();
	 vtkIdType idStope, id, ptstopeId ;
	 int nIds;
	 double currentBds[6];
	 vtkVariant* variant;
	 vtkIdList* list= vtkIdList::New();
	 vtkIdList* listStopesDone = vtkIdList::New();
	 vtkIdList* pts;
	 
	 NSCoords  current;

	 this->StopeCenterXArray= vtkDoubleArray::New();
	 this->StopeCenterXArray->Allocate(nbStopes);
	 this->StopeCenterYArray= vtkDoubleArray::New();
	 this->StopeCenterYArray->Allocate(nbStopes);
	 this->StopeCenterZArray= vtkDoubleArray::New();
	 this->StopeCenterZArray->Allocate(nbStopes);

	 for(int n=0; n<numberOfCells; n++)
	 {
		 pts= this->input->GetCell(n)->GetPointIds();	
		 idStope = (vtkIdType)p1StopeIdArray->GetTuple1(pts->GetId(0));
		 if(listStopesDone->IsId(idStope)<0)
		 {
			 listStopesDone->InsertNextId(idStope);
			 nIds=0;
			 currentBds[0] = stopeXminArray->GetTuple1(pts->GetId(0));
			 currentBds[1] = stopeXmaxArray->GetTuple1(pts->GetId(0));
			 currentBds[2] = stopeYminArray->GetTuple1(pts->GetId(0));
			 currentBds[3] = stopeYmaxArray->GetTuple1(pts->GetId(0));
			 currentBds[4] = stopeZminArray->GetTuple1(pts->GetId(0));
			 currentBds[5] = stopeZmaxArray->GetTuple1(pts->GetId(0));
				
			 this->StopeCenterXArray->InsertTuple1(idStope,(currentBds[0]+(currentBds[1]-currentBds[0])/2) );
			 this->StopeCenterYArray->InsertTuple1(idStope,(currentBds[2]+(currentBds[3]-currentBds[2])/2) );
			 this->StopeCenterZArray->InsertTuple1(idStope,(currentBds[4]+(currentBds[5]-currentBds[4])/2) );

			 //neighbours by X direction
			 stopeXminArray->LookupValue((vtkVariant)currentBds[0], list);
			 nIds= list->GetNumberOfIds();
			 for(int i=0; i<nIds; i++)
			 {
				 id = list->GetId(i);
				 ptstopeId = (vtkIdType)p1StopeIdArray->GetTuple1(id);
				 if(idStope!=ptstopeId)
				 {
					 if( (stopeZminArray->GetTuple1(id)<currentBds[5]) && (stopeZmaxArray->GetTuple1(id)>currentBds[4])
								&&( ((stopeYminArray->GetTuple1(id)>=currentBds[2])&&(stopeYminArray->GetTuple1(id)< currentBds[3]))
										||((stopeYmaxArray->GetTuple1(id)<= currentBds[3])&&(stopeYmaxArray->GetTuple1(id)>currentBds[2]))							
										||((stopeYminArray->GetTuple1(id)== currentBds[2])&&(stopeYmaxArray->GetTuple1(id)==currentBds[3]))
									)
						 )
					 {
						 current.id  = ptstopeId;
						 current.position = 0; //mean left
						 Nx->at((int)idStope).push_back(current);
					 }
				 }
			 }
			 list->Reset();
			 stopeXmaxArray->LookupValue((vtkVariant)currentBds[1], list);
			 nIds= list->GetNumberOfIds();
			 for(int i=0; i<nIds; i++)
			 {
				 id = list->GetId(i);
				 ptstopeId = (vtkIdType)p1StopeIdArray->GetTuple1(id);
				 if(idStope!=ptstopeId)
				 {				
					 if( (stopeZminArray->GetTuple1(id)<currentBds[5]) && (stopeZmaxArray->GetTuple1(id)>currentBds[4])
								&&( ((stopeYminArray->GetTuple1(id)>=currentBds[2])&&(stopeYminArray->GetTuple1(id)< currentBds[3]))
										||((stopeYmaxArray->GetTuple1(id)<= currentBds[3])&&(stopeYmaxArray->GetTuple1(id)>currentBds[2]))							
										||((stopeYminArray->GetTuple1(id)== currentBds[2])&&(stopeYmaxArray->GetTuple1(id)==currentBds[3]))
									)
						 )
					 {
						 current.id  = ptstopeId;
						 current.position = 1; //mean right
						 Nx->at((int)idStope).push_back(current);
					 }
				 }
			 }
			 list->Reset();

			 //neighbours by Y direction
			 stopeYminArray->LookupValue((vtkVariant)currentBds[2], list);
			 nIds= list->GetNumberOfIds();
			 for(int i=0; i<nIds; i++)
			 {
				 id = list->GetId(i);
				 ptstopeId = (vtkIdType)p1StopeIdArray->GetTuple1(id);
				 if(idStope!=ptstopeId)
				 {
					 if( (stopeZminArray->GetTuple1(id)<currentBds[5]) && (stopeZmaxArray->GetTuple1(id)>currentBds[4])
								&&( ((stopeXminArray->GetTuple1(id)>=currentBds[2])&&(stopeXminArray->GetTuple1(id)< currentBds[3]))
										||((stopeXmaxArray->GetTuple1(id)<= currentBds[3])&&(stopeXmaxArray->GetTuple1(id)>currentBds[2]))							
										||((stopeXminArray->GetTuple1(id)== currentBds[2])&&(stopeXmaxArray->GetTuple1(id)==currentBds[3]))
									)
						 )
					 {
						 current.id  = ptstopeId;
						 current.position = 0; 
						 Ny->at((int)idStope).push_back(current);
					 }
				 }
			 }
			 list->Reset();

			 stopeYmaxArray->LookupValue((vtkVariant)currentBds[3], list);
			 nIds= list->GetNumberOfIds();
			 for(int i=0; i<nIds; i++)
			 {
				 id = list->GetId(i);
				 ptstopeId = (vtkIdType)p1StopeIdArray->GetTuple1(id);
				 if(idStope!=ptstopeId)
				 {				
					 if( (stopeZminArray->GetTuple1(id)<currentBds[5]) && (stopeZmaxArray->GetTuple1(id)>currentBds[4])
								&&( ((stopeXminArray->GetTuple1(id)>=currentBds[2])&&(stopeXminArray->GetTuple1(id)< currentBds[3]))
										||((stopeXmaxArray->GetTuple1(id)<= currentBds[3])&&(stopeXmaxArray->GetTuple1(id)>currentBds[2]))							
										||((stopeXminArray->GetTuple1(id)== currentBds[2])&&(stopeXmaxArray->GetTuple1(id)==currentBds[3]))
									)
						 )
					 {
						 current.id  = ptstopeId;
						 current.position = 1; 
						 Ny->at((int)idStope).push_back(current);
					 }
				 }
			 }
			 list->Reset();

			 //neighbours by Z direction
			 stopeZminArray->LookupValue((vtkVariant)currentBds[4], list);
			 nIds= list->GetNumberOfIds();
			 for(int i=0; i<nIds; i++)
			 {
				 id = list->GetId(i);
				 ptstopeId = (vtkIdType)p1StopeIdArray->GetTuple1(id);
				 if(idStope!=ptstopeId)
				 {				 
					 if( (stopeXminArray->GetTuple1(id)<currentBds[1]) && (stopeXmaxArray->GetTuple1(id)>currentBds[0])
								&&( ((stopeYminArray->GetTuple1(id)>=currentBds[2])&&(stopeYminArray->GetTuple1(id)< currentBds[3]))
										||((stopeYmaxArray->GetTuple1(id)<= currentBds[3])&&(stopeYmaxArray->GetTuple1(id)>currentBds[2]))							
										||((stopeYminArray->GetTuple1(id)== currentBds[2])&&(stopeYmaxArray->GetTuple1(id)==currentBds[3]))
									)
						 )
					 {
						 current.id  = ptstopeId;
						 current.position = 0; 
						 Nz->at((int)idStope).push_back(current);
					 }
				 }
			 }
			 list->Reset();

			 stopeZmaxArray->LookupValue((vtkVariant)currentBds[5], list);
			 nIds= list->GetNumberOfIds();
			 for(int i=0; i<nIds; i++)
			 {
				 id = list->GetId(i);
				 ptstopeId = (vtkIdType)p1StopeIdArray->GetTuple1(id);
				 if(idStope!=ptstopeId)
				 {				 
					 if( (stopeXminArray->GetTuple1(id)<currentBds[1]) && (stopeXmaxArray->GetTuple1(id)>currentBds[0])
								&&( ((stopeYminArray->GetTuple1(id)>=currentBds[2])&&(stopeYminArray->GetTuple1(id)< currentBds[3]))
										||((stopeYmaxArray->GetTuple1(id)<= currentBds[3])&&(stopeYmaxArray->GetTuple1(id)>currentBds[2]))							
										||((stopeYminArray->GetTuple1(id)== currentBds[2])&&(stopeYmaxArray->GetTuple1(id)==currentBds[3]))
									)
						 )
					 {
						 current.id  = ptstopeId;
						 current.position = 1; 
						 Nz->at((int)idStope).push_back(current);
					 }
				 }
			 }
			 list->Reset();
		 }
	 }
}

//-----------------------------------------------------------------------------------
void vtkAbriToSotWriter::WriteData()
{
	if(validateData() != 1)
		return;
	// Output stream that will be used to write the data to the file
	
	// Grab object data from paraview
	this->input = vtkPolyData::SafeDownCast(this->GetInput());

	// input returns NULL if the data is not Polygonal
	if(this->input == NULL)
	{
		vtkErrorMacro("Writer only accepts PolyData, input type invalid.");
		return;
	}
	myFile.open(this->FileName);
	if(!myFile)
	{
		vtkErrorMacro("Error writing to the file");
		return;
	}

	//stope id array
  vtkIntArray* pStopeIdArray= vtkIntArray::New();
	pStopeIdArray= vtkIntArray::SafeDownCast(input->GetPointData()->GetArray("Stope Id"));   
	//weight
	vtkDoubleArray* pStopeweightArray= vtkDoubleArray::New();	  
	pStopeweightArray= vtkDoubleArray::SafeDownCast(input->GetPointData()->GetArray("Stope weight"));
	//lenght array
	vtkDoubleArray* pStopeLengthArray= vtkDoubleArray::New();
	pStopeLengthArray= vtkDoubleArray::SafeDownCast(input->GetPointData()->GetArray("Stope Length"));
	//volume
	vtkDoubleArray* pStopeVolumeArray= vtkDoubleArray::New();
	pStopeVolumeArray= vtkDoubleArray::SafeDownCast(input->GetPointData()->GetArray("Stope Volume"));
	//nsr
	vtkDoubleArray* pStopeFitnessArray= vtkDoubleArray::New();
	pStopeFitnessArray= vtkDoubleArray::SafeDownCast(input->GetPointData()->GetArray("Stope Fitness"));
	//nbBlocks
	vtkIntArray* pStopeNbBlocksArray= vtkIntArray::New();
	pStopeNbBlocksArray= vtkIntArray::SafeDownCast(input->GetPointData()->GetArray("Stope NbOfBlocks"));
 
	vtkDoubleArray* pDownhillArray= vtkDoubleArray::New();
	pDownhillArray= vtkDoubleArray::SafeDownCast(input->GetPointData()->GetArray("Stope Downhill"));

	vtkDoubleArray* pHorizontalArray= vtkDoubleArray::New();
	pHorizontalArray= vtkDoubleArray::SafeDownCast(input->GetPointData()->GetArray("Stope HzlConstraint"));
	
	
	inPoints = this->input->GetPoints();
	int numberOfPoints = inPoints->GetNumberOfPoints();
	
	double range[2];
	pStopeIdArray->GetRange(range);
	int nbStope= range[1]+1;
	this->numberOfStopes = nbStope;
  vtkIdList* temp= vtkIdList::New();

	if( (this->generateDependencies ==1)&&(this->TypeOfBlocks==0) )
	{
		double bounds[6];
		this->input->GetBounds(bounds);

		double xmin = bounds[0];
		double xmax = bounds[1];
		double ymin = bounds[2];
		double ymax = bounds[3];
		double zmin = bounds[4];
		double zmax = bounds[5];
		//calculate the blocks array size
		if((strcmp(StrikePosition,"x") == 0) || (strcmp(StrikePosition,"X") == 0))
		{
			d1 = (int)((xmax - xmin)/BlockSize[0]) + 1;
			d2 = (int)((ymax - ymin)/BlockSize[1]) + 1;
		}
		else
		{
			d2 = (int)((xmax - xmin)/BlockSize[0]) + 1;
			d1 = (int)((ymax - ymin)/BlockSize[1]) + 1;
		}
		d3 = static_cast<int>((zmax - zmin)/BlockSize[2]) + 1;

		//initialize the blocks array
		blocksArray = new int**[d1];
		for(int i=0 ; i<d1 ; i++)
		{
			blocksArray[i] = new int*[d2];
			for(int j=0 ; j<d2 ; j++)
			{
				blocksArray[i][j] = new int[d3];
				for(int k=0 ; k<d3 ; k++)
					blocksArray[i][j][k] = -1;
			}
		}
		this->stopePointsList= new vtkIntArray*[this->numberOfStopes];
		for(int n=0 ; n<this->numberOfStopes; n++)
			this->stopePointsList[n] = vtkIntArray::New();
		
		int i,j,k;
		double coord[3];
		//fill the blocks array
		for(int id=0; id<numberOfPoints; id++)
		{
			inPoints->GetPoint(id, coord);
			if((strcmp(StrikePosition,"x") == 0) || (strcmp(StrikePosition,"X") == 0))
			{
				i = (int)((coord[0] - xmin)/BlockSize[0]);
				j = (int)((coord[1] - ymin)/BlockSize[1]);
			}
			else
			{
				j = (int)((coord[0] - xmin)/BlockSize[0]);
				i = (int)((coord[1] - ymin)/BlockSize[1]);
			}
			k = (int)((coord[2] - zmin)/BlockSize[2]);
			blocksArray[i][j][k] = (int) pStopeIdArray->GetTuple1(id);

			stopePointsList[blocksArray[i][j][k]]->InsertNextTuple1(id);
		}

		ComputeStopesCenter(); //compute the centers of the stopes

		if(StartingStope > nbStope)
		{
			vtkWarningMacro(<<"The starting stope specified does not exist. "<<nbStope<<" was used instead");
			StartingStope = nbStope;
		}		
	}
	
	int stopeId=0;
	Stope* StopesArray= new Stope[nbStope];
	
	int nbPts= input->GetNumberOfPoints();
	vtkIdList *pt = vtkIdList::New();	
	
	//-------next lines to add all properties on the block model to the file--------------------------------------
	int stopeIdRank=0;

	input->GetPointData()->GetArray("StopeId",stopeIdRank); //get the rank of StopeId array
	//by that way we know the number of arrays on the original block model
	int nArrays=0;
	double** Arrays;
	if(stopeIdRank >0)
	{
			nArrays= stopeIdRank; //not necessary
			Arrays= new double*[nArrays];
			for(int i=0; i<nArrays; i++)
			{
				Arrays[i]= new double[nbStope];
				for(int j=0; j<nbStope; j++)
					Arrays[i][j]= 0;
				
	 		}
	 }		
		
  //-------------------------------------------------------------------------------------------------------------		
	for(int i=0; i<nbPts; i++)
	{
	   pt = this->input->GetCell(i)->GetPointIds();
	   stopeId= (int) pStopeIdArray->GetTuple1( pt->GetId(0));
	   if(temp->IsId(stopeId)==-1) 
		 {
			StopesArray[stopeId].id_stope= stopeId;
			StopesArray[stopeId].nsr= pStopeFitnessArray->GetTuple1( pt->GetId(0))
																/pStopeNbBlocksArray->GetTuple1( pt->GetId(0)); //revenue/Tonne
			StopesArray[stopeId].volume= pStopeVolumeArray->GetTuple1( pt->GetId(0));
			StopesArray[stopeId].weigth= pStopeweightArray->GetTuple1( pt->GetId(0));
			StopesArray[stopeId].length= pStopeLengthArray->GetTuple1( pt->GetId(0));
			StopesArray[stopeId].duration= 	StopesArray[stopeId].weigth/this->Duration;
			//constraint date calculation using the downhill metters and horizontal metters constraint
			StopesArray[stopeId].constraint_Date= getDate( (pDownhillArray->GetTuple1( pt->GetId(0))/this->DaysByMeter) +  returnDays(this->StartDate)
																										+(pHorizontalArray->GetTuple1( pt->GetId(0))/this->HzDByMeter)) ;
		 }

			//sum the properties values of each block in the stope and make the average of the stope
			for(int p=0; p<nArrays; p++)
				Arrays[p][stopeId]+= this->input->GetPointData()->GetArray(p)->GetTuple1(pt->GetId(0))
															/pStopeNbBlocksArray->GetTuple1( pt->GetId(0));						 
	}

	  //writting Project Details
	  myFile<<"1,Project Details"<<endl;
	  myFile<<"\t"<<"2,Key,Value"<<endl;
		myFile<<"\t"<<"\t"<<"3,StartDate,"<<this->StartDate<<endl;
	  myFile<<"\t"<<"\t"<<"3,Title,SampleMine"<<endl<<endl;

    //writting Number-Valued Fields
    myFile<<"1,Number-Valued Fields"<<endl;
	  myFile<<"\t"<<"2,Type,Name,Units"<<endl;
	  myFile<<"\t"<<"\t"<<"3,,Length,"<<endl;
	  myFile<<"\t"<<"\t"<<"3,,Tonnes,"<<endl;

		//adding all of properties on the blockmodel		
    for(int n=0; n<nArrays; n++)
			myFile<<"\t"<<"\t"<<"3,,"<< this->input->GetPointData()->GetArray(n)->GetName()<<","<<endl;
	
	  //we can add the minerals used to calculate the fitness
	  myFile<<"\t"<<"\t"<<"3,,NSR Total,"<<endl;
	  myFile<<"\t"<<"\t"<<"3,,Volume,"<<endl<<endl;
	 
	  //writting String-Valued Fields
	  myFile<<"1,String-Valued Fields"<<endl;
	  myFile<<"\t"<<"2,Type,Name"<<endl;
	  myFile<<"\t"<<"\t"<<"3,,ActivityID"<<endl;
	  myFile<<"\t"<<"\t"<<"3,,Area"<<endl;
    myFile<<"\t"<<"\t"<<"3,,Excavation Type"<<endl;
		myFile<<"\t"<<"\t"<<"3,,ClusterID"<<endl<<endl;    
		
		//writting Tasks
	  myFile<<"1,Tasks"<<endl;
    myFile<<"\t"<<"2,ActivityID,Excavation Type,Area,ClusterID,Tonnes,Length,Duration,Volume,NSR Total,";

		for(int n=0; n<nArrays; n++)
			myFile<< this->input->GetPointData()->GetArray(n)->GetName()<<",";
		   
		myFile<<"Constraint,"<<"Constraint Date,Dependency Method"<<endl;
    for(int i=0; i<nbStope; i++)
	  {
			//task for one stope
			//for the revenue we think we can just use the NSR value on the original blocks
			myFile<<"\t"<<"\t"<<"3,"
			<<StopesArray[i].id_stope<<",stope," 
			<<"east,"
			<<"1000A,"
			<<StopesArray[i].weigth<<","
			<<StopesArray[i].length<<","
			<<StopesArray[i].duration<<"d,"
			<<StopesArray[i].volume<<","
			<<StopesArray[i].nsr<<",";
			    
		  for(int p=0; p<nArrays; p++)
			 myFile<<Arrays[p][i]<<",";
					
			myFile<<"Start No Earlier Than,"
				<<"20"<<StopesArray[i].constraint_Date<<","
			<<"Earliest"	
			<<endl;
    }      
	 myFile<<endl<<endl;

   //-----------------writing Dependencies--------------------------------------------
   myFile<<"1,Dependencies"<<endl;
	 myFile<<"\t"<<"2,From,To,Type,Lag"<<endl;
	 if(this->generateDependencies == 1)
	 {
		 generateFactsFile();
		 InitializeEnvironment();
		 //load facts file
		 if(Load("abrifacts.clp") != 1)
			{
				vtkWarningMacro("An error occured while opening facts file...aborting\n");
				return;
			}
		 //load CLIPS script
		 if(Load(this->ScriptPath) != 1)
			{
				vtkWarningMacro("An error occured while opening CLIPS file...aborting\n");
				return;
			}
			Reset();
			//run script
			Run(-1L);
		  remove("abrifacts.clp");
		 //put the dependencies in the act file
		 for(list<vtkStdString>::iterator it=dependencies.dependenciesList.begin() ; it!=dependencies.dependenciesList.end() ; it++ )
		 {
			 myFile<<"\t"<<"\t"<<(*it)<<endl;
		 }
	 }
   //------------------------------------------------------------------------------------
	 myFile.close();

	 //Memory clean up	 
   delete StopesArray; 
	 for(int p=0; p<nArrays; p++)
		 delete Arrays[p];
	 delete Arrays;
	
}
	
//-----------------------------------------------------------------------------------
int vtkAbriToSotWriter::FillInputPortInformation(int, vtkInformation *info)
{
	info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData");
	return 1;
}



//-----------------------------------------------------------------------------------
vtkPolyData* vtkAbriToSotWriter::GetInput()
{
	return vtkPolyData::SafeDownCast(this->Superclass::GetInput());
}


//-----------------------------------------------------------------------------------
vtkPolyData* vtkAbriToSotWriter::GetInput(int port)
{
	return vtkPolyData::SafeDownCast(this->Superclass::GetInput(port));
}


//-----------------------------------------------------------------------------------
//Called with this->PrintSelf(*fp, indent)
void vtkAbriToSotWriter::PrintSelf(ostream& fp, vtkIndent indent)
{
	//this->Superclass::PrintSelf(fp,indent);
}


//-----------------------------------------------------------------------------------
int vtkAbriToSotWriter::generateFactsFile()
{
	FILE *outFile;
	//outFile = fopen("C:/test/facts.clp","w");
	outFile = fopen("abrifacts.clp","w");
	if(outFile == NULL)
	{
		vtkWarningMacro("could not create facts file...aborting");
		return 0;
	}

	ofstream out_ADJ_File;
	int length = (int)strlen(this->FileName);
	char* ADJ_name= new char[length+5];
	strncpy(ADJ_name,this->FileName,(length-4));
	ADJ_name[length-4]='\0';
	strcat(ADJ_name,"_ADJ.csv");
	
	out_ADJ_File.open(ADJ_name);	
	if(!out_ADJ_File)
	{
		vtkWarningMacro("could not create ADJ csv file...aborting");
		return 0;
	}

	 out_ADJ_File<<"GUID1,"<<"GUID2,"<<"ADJTYPE,"<<"DELTA-X,"<<"DELTA-Y,"<<"DELTA-Z"<<endl;
	 /* ADJTYPE:  1= along strike, horizontal 2= along strike, vertical
						    3= diagonal, along strike 4= accross strike, horizontal
			GUID1:    firt neighbour stope 	GUID2:    second neighbour stope  */

	/*vector<list<int>> neighborsVectorX;
	vector<list<int>> neighborsVectorY;
	vector<list<int>> neighborsVectorZ;*/
  vtkNeighboursVect* neighborsVectorX = new vtkNeighboursVect();
	vtkNeighboursVect* neighborsVectorY = new vtkNeighboursVect();
	vtkNeighboursVect* neighborsVectorZ = new vtkNeighboursVect();
	NSCoords current;
	neighborsVectorX->resize(this->numberOfStopes);
	neighborsVectorY->resize(this->numberOfStopes);
	neighborsVectorZ->resize(this->numberOfStopes);
	fputs("(deffacts init-data\n",outFile);
	
	if(this->TypeOfBlocks==0)
	{
		for(int i=0 ; i<d1 ; i++)
		{
			for(int j=0 ; j<d2 ; j++)
			{
				for(int k=0 ; k<d3 ; k++)
				{
					if(i<d1-1)
					{
						if((blocksArray[i][j][k] != -1) && (blocksArray[i+1][j][k] != -1) 
							&& (blocksArray[i][j][k] != blocksArray[i+1][j][k]))
						{
							current.id = blocksArray[i+1][j][k];
							current.position = 1;
							neighborsVectorX->at(blocksArray[i][j][k]).push_back(current);
						} 
					}
					if(j<d2-1)
					{
						if((blocksArray[i][j][k] != -1) && (blocksArray[i][j+1][k] != -1) 
							&& (blocksArray[i][j][k] != blocksArray[i][j+1][k]))
						{
							current.id = blocksArray[i][j+1][k];
							current.position = 1;
							neighborsVectorY->at(blocksArray[i][j][k]).push_back(current);
						}
					}
					if(k<d3-1)
					{
						if((blocksArray[i][j][k] != -1) && (blocksArray[i][j][k+1] != -1) 
							&& (blocksArray[i][j][k] != blocksArray[i][j][k+1]))
						{
							current.id = blocksArray[i][j][k+1];
							current.position = 1;
							neighborsVectorZ->at(blocksArray[i][j][k]).push_back(current);
						}
					}
				}
			}
		}
	}
	else
	{
		this->ComputeNeighbours_Ir(neighborsVectorX, neighborsVectorY, neighborsVectorZ);
	}

	char neighbors[40];

	//adding the stairting point (initial stope) as the first knowledge
	sprintf(neighbors,"\t(startingPoint stope%i)\n",StartingStope);;
	fputs(neighbors,outFile);


	for(int i=0 ; i<this->numberOfStopes ; i++)
	{
		neighborsVectorX->at(i).sort(compare_nocase);
		neighborsVectorX->at(i).unique(compare_nocase1);

		for(list<NSCoords>::iterator it=neighborsVectorX->at(i).begin() ; it!=neighborsVectorX->at(i).end() ; it++)
		{
			if((*it).position == 1)
			sprintf(neighbors,"\t(neighbors stope%i stope%i %i)\n",i,(*it).id,0); //0 for the neighbours in X direction
			else sprintf(neighbors,"\t(neighbors stope%i stope%i %i)\n",(*it).id, i, 0);
			fputs(neighbors,outFile);

			//WRITE AT THE SAME TIME IN THE ADJ csv FILE
			out_ADJ_File<<i<<","<<(*it).id<<","<<"1"<<",";
			out_ADJ_File<<this->StopeCenterXArray->GetTuple1((*it).id) - this->StopeCenterXArray->GetTuple1(i)
									<<","<<this->StopeCenterYArray->GetTuple1((*it).id) - this->StopeCenterYArray->GetTuple1(i)
									<<","<<this->StopeCenterZArray->GetTuple1((*it).id) - this->StopeCenterZArray->GetTuple1(i)<<endl;
		}

		neighborsVectorY->at(i).sort(compare_nocase);
		neighborsVectorY->at(i).unique(compare_nocase1);
		for(list<NSCoords>::iterator it=neighborsVectorY->at(i).begin() ; it!=neighborsVectorY->at(i).end() ; it++)
		{ 
			if((*it).position == 1)
			sprintf(neighbors,"\t(neighbors stope%i stope%i %i)\n",i,(*it).id,1); //1 for the neighbours in Y direction
			else sprintf(neighbors,"\t(neighbors stope%i stope%i %i)\n",(*it).id,i,1);
			fputs(neighbors,outFile);

			//WRITE AT THE SAME TIME IN THE ADJ csv FILE
			out_ADJ_File<<i<<","<<(*it).id<<","<<"4"<<",";
			out_ADJ_File<<this->StopeCenterXArray->GetTuple1((*it).id) - this->StopeCenterXArray->GetTuple1(i)
								<<","<<this->StopeCenterYArray->GetTuple1((*it).id) - this->StopeCenterYArray->GetTuple1(i)
								<<","<<this->StopeCenterZArray->GetTuple1((*it).id) - this->StopeCenterZArray->GetTuple1(i)<<endl;
		}
		neighborsVectorZ->at(i).sort(compare_nocase);
		neighborsVectorZ->at(i).unique(compare_nocase1);
		for(list<NSCoords>::iterator it=neighborsVectorZ->at(i).begin() ; it!=neighborsVectorZ->at(i).end() ; it++)
		{
			if((*it).position == 1)
			sprintf(neighbors,"\t(neighbors stope%i stope%i %i)\n",i,(*it).id,2); //2 for the neighbours in Z direction
			else sprintf(neighbors,"\t(neighbors stope%i stope%i %i)\n", (*it).id, i, 2);
			fputs(neighbors,outFile);

			//WRITE AT THE SAME TIME IN THE ADJ csv FILE
			out_ADJ_File<<i<<","<<(*it).id<<","<<"2"<<",";
			out_ADJ_File<<this->StopeCenterXArray->GetTuple1((*it).id) - this->StopeCenterXArray->GetTuple1(i)
								<<","<<this->StopeCenterYArray->GetTuple1((*it).id) - this->StopeCenterYArray->GetTuple1(i)
								<<","<<this->StopeCenterZArray->GetTuple1((*it).id) - this->StopeCenterZArray->GetTuple1(i)<<endl;
		}
	}
	fputs(")",outFile);
	fclose(outFile);

	//FREE UP MEMORY
	out_ADJ_File.close();
	this->StopeCenterXArray->Delete();
	this->StopeCenterYArray->Delete();
	this->StopeCenterZArray->Delete();
	if(this->TypeOfBlocks==0)
	{
		for(int n=0; n<this->numberOfStopes; n++)
			this->stopePointsList[n]->Delete();

		for(int i=0 ; i<d1 ; i++)
		{
			for(int j=0 ; j<d2 ; j++)
				delete blocksArray[i][j];
			delete blocksArray[i];
		}
		delete blocksArray;

	}

  delete ADJ_name;	
	delete neighborsVectorX;
	delete neighborsVectorY;
	delete neighborsVectorZ;

	return 1;
}


//-----------------------------------------------------------------------------------
int vtkAbriToSotWriter::validateData()
{
	//check if a script was specified
	if(strlen(this->ScriptPath) > 1)
	{
		this->generateDependencies = 1;
		for(int i=0 ; i<strlen(this->ScriptPath) ; i++)
		{
			if(this->ScriptPath[i] == '\\')
				this->ScriptPath[i] = '/';
		}
		ifstream scriptFile;
		scriptFile.open(this->ScriptPath);
		if(scriptFile.fail())
		{
			vtkWarningMacro(<<"failed to open script file...aborting");
			scriptFile.close();
			return -1;
		}
		scriptFile.close();
	}
	else
	{
		return 1;
	}
	if((strcmp(StrikePosition,"x") != 0) && (strcmp(StrikePosition,"X") != 0) && 
		(strcmp(StrikePosition,"y") != 0) && (strcmp(StrikePosition,"Y") != 0))
	{
		vtkWarningMacro("invalid strike position...aborting");
		return -1;
	}
	if((BlockSize[0] == 0.0) || (BlockSize[1] == 0.0) || (BlockSize[2] == 0.0))
	{
		vtkWarningMacro("block size is invalid...aborting");
		return -1;
	}
	return 1;
}


//-----------------------------------------------------------------------------------
int printDependency()
{
	char* from = RtnLexeme(1);
	char* to = RtnLexeme(2);
	char dep[40];
	sprintf(dep,"3,%s,%s,FS,0d",from,to);
	dependencies.dependenciesList.push_back(vtkStdString(dep));
	return 1;
}