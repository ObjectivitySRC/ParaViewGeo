/*=========================================================================
  Program:   Visualization Toolkit
  Module:    $ vtkStopesResearch.cxx $
  Authors:    Arolde VIDJINNAGNI
  MIRARCO, Laurentian University
  Date:    September 2009 
  Version:   0.1
=========================================================================*/

#include "vtkStopesResearch.h"
#include "vtkPointData.h"
#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkDataArray.h"	
#include "vtkMath.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkArrayCalculator.h"
#include "vtkBlockSubdivision.h"
#include <vtkstd/list>


vtkCxxRevisionMacro ( vtkStopesResearch, "$Revision: 0.3 $" );
vtkStandardNewMacro ( vtkStopesResearch );


namespace
{		
	struct StopeCave
		{
		int StopeId;
		double Score;
		double Weight;
		};

	typedef vtkstd::list<StopeCave> StopeCave_L;
	typedef vtkstd::list<StopeCave>::iterator ExtractionListIterator; 
}

// --------------------------------------
class InternalExtractList: public StopeCave_L {};

//========================================================================================
vtkStopesResearch::vtkStopesResearch(void)
{
	InstituteTone = 250.0 ;
	Swell = 0.6;
	ExtractionCapacity = 100000.0;
	UnitCave=1;

	XINC = NULL;
	YINC = NULL;
	ZINC = NULL;
	stopeWidth=10.0;
	stopeDepth=50.0;	
	this->densityInput = NULL;
	Function=NULL;
	FitnessArrayName= NULL;
	ReplacementValue= 0.0;
	ReplaceInvalidValues=0; 
	this->nbXLayers=0;
	this->nbYLayers=0;
	this->stopeId=0;
	this->BlockModelType =0; //0 mean regular block in the algorithm
}

//========================================================================================
vtkStopesResearch::~vtkStopesResearch(void)
{
}
//========================================================================================
int vtkStopesResearch::FillInputPortInformation ( int vtkNotUsed(port), vtkInformation* info )
  {
   info->Set ( vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData", 0 );
  return 1;
  }

//========================================================================================
int vtkStopesResearch::FillOutputPortInformation(int vtkNotUsed(port), vtkInformation* info)
{
	info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkPolyData");	
  return 1;
}


//========================================================================================
void vtkStopesResearch::ComputeDevelopmentStope(vtkPolyData* inDataset)
{
	double S_Fitness=0, S_Weight=0, 
				 S_volume=0,  S_nbBlocks=0,
				 S_Xmin=0,    S_Ymin=0,     
				 S_Xmax=0,    S_Ymax=0,
				 S_Zmin=0,		S_Zmax=0,
				 Sx=0,        Sy=0,        
				 Sz=0,				pt[3],
				 bounds[6];
	Stope* candidate=NULL; 
	vtkIdType idc;
	vtkIdType nBlocks=0;
	numBerOfCells= inDataset->GetNumberOfCells();

	this->Container= new StopeContainer();
	
	for(int i=0; i<this->nbXLayers; i++)
		{
		for(int j=0; j<this->nbYLayers; j++)
			{		
			S_Fitness=0;
			S_volume=0;
			S_Weight=0;			
			bounds[0]=this->Allbounds[1]; bounds[1]= this->Allbounds[0];
			bounds[2]=this->Allbounds[3]; bounds[3]= this->Allbounds[2]; 
			bounds[4]=this->Allbounds[5]; bounds[5]= this->Allbounds[4];
			S_nbBlocks=this->ptsOrderedByLayer[i][j]->GetNumberOfIds();			
			
			if(S_nbBlocks>0)
				{
				candidate= new Stope(this->stopeId, S_nbBlocks);
				for(int c=0; c<this->ptsOrderedByLayer[i][j]->GetNumberOfIds();c++)
					{
					idc= this->ptsOrderedByLayer[i][j]->GetId(c);
					candidate->AddBlock(idc);
					this->stopeIdArray->InsertTuple1(idc,this->stopeId);

					Sx= this->newXINCArray->GetTuple1(idc)/2;
					Sy= this->newYINCArray->GetTuple1(idc)/2;
					Sz= this->newZINCArray->GetTuple1(idc)/2;

					S_volume += (2*Sx)*(2*Sy)*(2*Sz);
					S_Weight += ((2*Sx)*(2*Sy)*(2*Sz)*this->blockDensity->GetTuple1(idc));
					S_Fitness += this->fitnessArray->GetTuple1(idc);
					inDataset->GetPoint(idc,pt);

					if( (pt[0]-Sx) < bounds[0])
						bounds[0]= pt[0]-Sx;
					if( (pt[0]+Sx) > bounds[1])
						bounds[1]= pt[0]+Sx;
					if( (pt[1]-Sy)<bounds[2])
						bounds[2]=pt[1]-Sy;
					if( (pt[1]+Sy) > bounds[3])
						bounds[3]= pt[1]+Sy;
					if( (pt[2]-Sz)<bounds[4])
						bounds[4]=pt[2]-Sz;
					if( (pt[2]+Sz)>bounds[5])
						bounds[5]=pt[2]+Sz;
					}
		
				candidate->SetNumberOfBlocks(S_nbBlocks);
				candidate->SetBounds(bounds);
				candidate->SetFitness(S_Fitness);
				candidate->SetVolume(S_volume);
				candidate->SetWeight(S_Weight);
									 
				Container->AddStope(candidate);
				this->stopeId++;
				}
			}
		}

	 for(int i=0; i<this->nbXLayers; i++)
		{
		 for(int j=0; j<this->nbYLayers; j++)
		   this->ptsOrderedByLayer[i][j]->Delete();
		 delete[] ptsOrderedByLayer[i];
		}
		delete[] ptsOrderedByLayer;

}


//========================================================================================
double vtkStopesResearch::GetScore(vtkPolyData* inDataset,
																	 vtkIdType id,
																   int level,
																	 double We[1]
																   )
{
	double pt[3], Sx=0, Sy=0, Sz=0,
				 score = 0, Weight = 0;
	int  n;
		
	int nBlocks = this->Container->GetStope(id)->GetNumberOfBlocks();
	vtkIdType current;

	for(int i=0; i<nBlocks; i++)
		{
		current = this->Container->GetStope(id)->GetBlockId(i); 
		inDataset->GetPoint(current,pt);
		Sz= this->newZINCArray->GetTuple1(current)/2;

		if( (pt[2]-Sz) < (this->Allbounds[4]+(level+1)*this->UnitCave))
			{
				if(CaveResultList[id]->IsId(current)==-1)
				CaveResultList[id]->InsertNextId(current);
			}	
		}

	n= CaveResultList[id]->GetNumberOfIds();
	for(int i=0; i<n; i++)
		{
		current = CaveResultList[id]->GetId(i);
		inDataset->GetPoint(current,pt);
		Sx= this->newXINCArray->GetTuple1(current);
		Sy= this->newYINCArray->GetTuple1(current);
		Sz= this->newZINCArray->GetTuple1(current);
		score += this->fitnessArray->GetTuple1(current);			
		Weight += Sx*Sy*this->InstituteTone/this->Swell; //[ (Institute_Tone*Area)/Swell_factor]	
		}
	We[0] = Weight;
	return score;
}

																	 
//========================================================================================
bool compare_nocase(StopeCave first, StopeCave second)
{		
	if(first.Score > second.Score)
		return true;
	else return false;
}

//========================================================================================
void vtkStopesResearch::ComputeCaving(vtkPolyData* inDataset)
{	
	int level=0,
		  nbExtractedCells=0;
	double Weight[1]={0.0},
				 totalLevelWeight=0;
	vtkIdType id=0;
		
	this->CaveResultList = new vtkIdList*[this->stopeId];
	for(int S=0; S< this->stopeId; S++)
		CaveResultList[S] = vtkIdList::New();

	this->BCavingList  = new InternalExtractList();
	ExtractionListIterator it;	

	while(nbExtractedCells<numBerOfCells)
		{		
		//run the score for all development stopes and compute the total weight of the blocks candidates
		for(int S=0; S< this->stopeId; S++)
			{
			Weight[0]=0.0;
			StopeCave currentCave;
			currentCave.StopeId  = S;
			currentCave.Score = GetScore(inDataset, S, level, Weight);
			currentCave.Weight = Weight[0];
			this->BCavingList->push_back(currentCave);
			}

		this->BCavingList->sort(compare_nocase); //we need to sort by the fitness score

		//now start the extraction by maximizing the total fitness and stay under to the capacity extraction per day
		it= this->BCavingList->begin();
		totalLevelWeight=0.0;
		while( (totalLevelWeight<this->ExtractionCapacity) && (it != this->BCavingList->end()) )
			{			
			while(this->CaveResultList[(*it).StopeId]->GetNumberOfIds()>0)
			 {
		 		id = this->CaveResultList[(*it).StopeId]->GetId(0);
				this->ExtractionLevelArray->SetValue(id,level);
				nbExtractedCells++;
				this->Container->RemoveBlock((*it).StopeId,id);
				this->CaveResultList[(*it).StopeId]->DeleteId(id);
				}
			totalLevelWeight += (*it).Weight;					
			it++;
			}
		level++;
		this->BCavingList->clear();
		}

	for(int S=0; S< this->stopeId; S++)
		this->CaveResultList[S]->Delete();
	delete this->CaveResultList;
	delete this->BCavingList;
	delete this->Container;	

}

//========================================================================================
void vtkStopesResearch::SubDivideBModel(vtkPolyData* bModel, 
																				vtkPolyData* out
																				)
{
	 //Normalization of the block model in firts iteration
	 vtkBlockSubdivision* BlockSubdivisionByX= vtkBlockSubdivision::New();
	 BlockSubdivisionByX->SetInput(bModel);
	 BlockSubdivisionByX->SetDirection(0);
	 BlockSubdivisionByX->SetXINC(this->XINC);
	 BlockSubdivisionByX->SetYINC(this->YINC);
	 BlockSubdivisionByX->SetZINC(this->ZINC);
	 BlockSubdivisionByX->SetboundMaxX(this->Allbounds[1]);
	 BlockSubdivisionByX->SetboundMaxY(this->Allbounds[3]);
	 BlockSubdivisionByX->SetboundMaxZ(this->Allbounds[5]);
	 BlockSubdivisionByX->SetboundMinX(this->Allbounds[0]);
	 BlockSubdivisionByX->SetboundMinY(this->Allbounds[2]);
	 BlockSubdivisionByX->SetboundMinZ(this->Allbounds[4]);
	 BlockSubdivisionByX->SetDeltaByLayer(this->stopeWidth);
	 BlockSubdivisionByX->Update();

	 vtkBlockSubdivision* BlockSubdivisionByY= vtkBlockSubdivision::New();
	 BlockSubdivisionByY->SetInput(BlockSubdivisionByX->GetOutput());
	 BlockSubdivisionByY->SetDirection(1);
	 BlockSubdivisionByY->SetXINC(this->XINC);
	 BlockSubdivisionByY->SetYINC(this->YINC);
	 BlockSubdivisionByY->SetZINC(this->ZINC);
	 BlockSubdivisionByY->SetboundMaxX(this->Allbounds[1]);
	 BlockSubdivisionByY->SetboundMaxY(this->Allbounds[3]);
	 BlockSubdivisionByY->SetboundMaxZ(this->Allbounds[5]);
	 BlockSubdivisionByY->SetboundMinX(this->Allbounds[0]);
	 BlockSubdivisionByY->SetboundMinY(this->Allbounds[2]);
	 BlockSubdivisionByY->SetboundMinZ(this->Allbounds[4]);
	 BlockSubdivisionByY->SetDeltaByLayer(this->stopeDepth);
	 BlockSubdivisionByY->Update();

	 out->ShallowCopy(BlockSubdivisionByY->GetOutput());
	 BlockSubdivisionByX->Delete();
	 BlockSubdivisionByY->Delete();

	 
	 XLayersArray= vtkIntArray::New();
	 XLayersArray->DeepCopy(out->GetPointData()->GetArray("LayerIDByX"));
	 YLayersArray= vtkIntArray::New();
	 YLayersArray->DeepCopy(out->GetPointData()->GetArray("LayerIDByY"));

   this->blockDensity= vtkDoubleArray::New();
	 this->blockDensity->DeepCopy( out->GetPointData()->GetArray(this->densityInput));

	 this->newXINCArray= vtkDoubleArray::New();
	 this->newXINCArray->DeepCopy(out->GetPointData()->GetArray(this->XINC));
	 this->newYINCArray= vtkDoubleArray::New();
	 this->newYINCArray->DeepCopy(out->GetPointData()->GetArray(this->YINC));
	 this->newZINCArray= vtkDoubleArray::New();
	 this->newZINCArray->DeepCopy(out->GetPointData()->GetArray(this->ZINC));


}
//========================================================================================
int vtkStopesResearch::RequestData( vtkInformation *request, 
                         vtkInformationVector **InputVector, 
                         vtkInformationVector *outputVector )
{
	
   vtkInformation *inputInfo = InputVector[0]->GetInformationObject ( 0 );
   vtkInformation *outInfo = outputVector->GetInformationObject ( 0 );

   vtkPolyData* input = vtkPolyData::SafeDownCast (
                         inputInfo->Get ( vtkDataObject::DATA_OBJECT() ) );
	
   vtkPolyData *output = vtkPolyData::SafeDownCast (
   outInfo->Get ( vtkDataObject::DATA_OBJECT() ) );

	 if(this->densityInput==NULL){
		 vtkErrorMacro("The Density array specified does not exist or is not specified...Aborting");
		 return -1;
	 }
	 
	this->XINCArray= vtkDoubleArray::New();
	this->XINCArray->DeepCopy(input->GetPointData()->GetArray(this->XINC));
	this->YINCArray= vtkDoubleArray::New();
	this->YINCArray->DeepCopy(input->GetPointData()->GetArray(this->YINC));
	this->ZINCArray= vtkDoubleArray::New();
	this->ZINCArray->DeepCopy(input->GetPointData()->GetArray(this->ZINC));

	 this->getBlockSize(input); 
	 vtkPolyData* transform = vtkPolyData::New(); 
	 this->SubDivideBModel(input, transform);


	 /***********************Fitness Array computing**************************************/
	 if(this->Function==NULL)
	 {
		 vtkErrorMacro("The Fitness Equation is not specified or does not exist...Aborting");
		 return -1;
	 }
	 else
	 {
		 vtkArrayCalculator* calculator= vtkArrayCalculator::New();
		 calculator->SetInput(transform);
		 calculator->SetFunction(this->Function);
		 calculator->SetResultArrayName(this->FitnessArrayName);
		 int nb= transform->GetPointData()->GetNumberOfArrays();
		 for(int j=0; j<nb; j++)
		 calculator->AddScalarArrayName(transform->GetPointData()->GetArrayName(j));
		 calculator->SetReplaceInvalidValues(this->ReplaceInvalidValues);
		 calculator->SetReplacementValue(this->ReplacementValue);
		 calculator->SetAttributeModeToUsePointData();
		 calculator->Update();

		 this->fitnessArray=  vtkDoubleArray::New();
		 this->fitnessArray->DeepCopy(calculator->GetPolyDataOutput()->GetPointData()->GetArray(this->FitnessArrayName));
		 if(this->fitnessArray==NULL){
				vtkErrorMacro("fitnessArray is empty aborting...");
				return -1;
		 }
		 calculator->Delete();
	 }	
	 
	 initialize(transform); //used to initialize the parameters
	 ComputeDevelopmentStope(transform);
	 setProperties(transform);
	 ComputeCaving(transform); //calculate the caving level for each stope
	 transform->GetPointData()->AddArray(this->stopeIdArray);
	 transform->GetPointData()->AddArray(this->ExtractionLevelArray);
	
	 output->ShallowCopy(transform);
			
		this->stopeIdArray->Delete();
		this->ExtractionLevelArray->Delete();
		this->fitnessArray->Delete();
	  this->blockDensity->Delete();
			
		return 1;
}

//=====================================================================================
void vtkStopesResearch::initialize(vtkPolyData* inDataset)
{
	int nbOfCells= inDataset->GetNumberOfCells();
	vtkIdList* pts;
	int layerByX=0, layerByY=0;
	
	//order the input points by layer
	double range[2];
	this->XLayersArray->GetRange(range); 
	this->nbXLayers= (int)(range[1]-range[0])+1;
	this->ptsOrderedByLayer= new vtkIdList**[this->nbXLayers];	

	this->YLayersArray->GetRange(range); 
	this->nbYLayers= (int)(range[1]-range[0])+1;
	for(int i=0; i<this->nbXLayers; i++)
	{
		this->ptsOrderedByLayer[i]= new vtkIdList*[this->nbYLayers];
		for(int j=0; j<this->nbYLayers; j++)
		{
			this->ptsOrderedByLayer[i][j]= vtkIdList::New();
		}
	}

	this->ExtractionLevelArray = vtkIntArray::New();
	this->ExtractionLevelArray->SetName("Extraction Level");	
	this->ExtractionLevelArray->Allocate(nbOfCells);
	
	for(int i=0; i<nbOfCells; i++)
	{
		pts= inDataset->GetCell(i)->GetPointIds();
		layerByX= (int) this->XLayersArray->GetTuple1(pts->GetId(0));
		layerByY= (int) this->YLayersArray->GetTuple1(pts->GetId(0));
		this->ptsOrderedByLayer[layerByX][layerByY]->InsertNextId(pts->GetId(0));
		this->ExtractionLevelArray->InsertTuple1(pts->GetId(0), -1);
	}

	this->XLayersArray->Delete();
	this->YLayersArray->Delete();

	this->stopeId=0;
	this->stopeIdArray= vtkIntArray::New();
	this->stopeIdArray->Allocate(nbOfCells);
	this->stopeIdArray->SetName("Stope Id");

}

//=====================================================================================
void vtkStopesResearch::getBlockSize(vtkPolyData* dataset)
{
	 Allbounds[0]=0.0; Allbounds[1]=0.0;
	 Allbounds[2]=0.0; Allbounds[3]=0.0;
	 Allbounds[4]=0.0; Allbounds[5]=0.0;
	 int nbCells= dataset->GetNumberOfCells();
	 vtkIdList* pts;
	 double pt[3];
	 double delta=0, Sx, Sy, Sz;	 
	
	 if(this->BlockModelType==0)
	 {
		 dataset->GetBounds(Allbounds);
		 double range[2]={0.0};
		 dataset->GetPointData()->GetArray(this->ZINC)->GetRange(range);
		 Sz=range[0]; 
		 this->UnitCave = Sz; 

		 dataset->GetPointData()->GetArray(this->YINC)->GetRange(range);
		 Sy=range[0];
		 dataset->GetPointData()->GetArray(this->XINC)->GetRange(range);
		 Sx=range[0];
	
		Allbounds[0] -= Sx/2;
		Allbounds[1] += Sx/2;
		Allbounds[2] -= Sy/2;
		Allbounds[3] += Sy/2;
		Allbounds[4] -= Sz/2;
		Allbounds[5] += Sz/2;		
	 }
	 else 
		{
		 for(int i=0; i<nbCells; i++)
		 {
			 if(i==0)
			 {
					delta=0;
					pts= dataset->GetCell(i)->GetPointIds();			
					dataset->GetPoint(pts->GetId(0),pt);		

					delta= (this->XINCArray->GetTuple1(pts->GetId(0))/2); //size/2
					Allbounds[1]= (pt[0]+delta);
					Allbounds[0]= (pt[0]-delta);

					delta= (this->YINCArray->GetTuple1(pts->GetId(0))/2);
					Allbounds[3]= (pt[1]+delta);
					Allbounds[2]= (pt[1]-delta);

					delta= (this->ZINCArray->GetTuple1(pts->GetId(0))/2);
					Allbounds[5]= (pt[2]+delta);
					Allbounds[4]= (pt[2]-delta);	
			 }
			 else
			 {
					delta=0;
					pts= dataset->GetCell(i)->GetPointIds();			
					dataset->GetPoint(pts->GetId(0),pt);

					delta= (this->XINCArray->GetTuple1(pts->GetId(0))/2); //size/2
					if( (pt[0]+delta)>Allbounds[1])
						Allbounds[1]= (pt[0]+delta);

					if( (pt[0]-delta)<Allbounds[0])
						Allbounds[0]= (pt[0]-delta);

					delta= (this->YINCArray->GetTuple1(pts->GetId(0))/2);
					if( (pt[1]+delta) >Allbounds[3])
						Allbounds[3]= (pt[1]+delta);

					if( (pt[1]-delta) <Allbounds[2])
						Allbounds[2]= (pt[1]-delta);

					delta= (this->ZINCArray->GetTuple1(pts->GetId(0))/2);
					if( (pt[2]+delta)>Allbounds[5])
						Allbounds[5]= (pt[2]+delta);

					if((pt[2]-delta)<Allbounds[4])
						Allbounds[4]= (pt[2]-delta);	
				}
		 }
	 }
}


//=====================================================================================
void vtkStopesResearch::setProperties(vtkPolyData* dataset)
{
	 int nbCells= dataset->GetNumberOfCells();
	 vtkIdList* pts;
	 vtkIdType id_S=0;
	 double bounds[6];
  
   vtkDoubleArray* pStopeFitnessArray= vtkDoubleArray::New();
   pStopeFitnessArray->Allocate(nbCells);
	 ((vtkDoubleArray*)pStopeFitnessArray)->SetName("Stope Fitness");

	 vtkDoubleArray* pStopeWeightArray= vtkDoubleArray::New();
   pStopeWeightArray->Allocate(nbCells);
	 ((vtkDoubleArray*)pStopeWeightArray)->SetName("Stope weight");

   vtkDoubleArray* pStopeVolumeArray= vtkDoubleArray::New();
   pStopeVolumeArray->Allocate(nbCells);
   ((vtkDoubleArray*)pStopeVolumeArray)->SetName("Stope Volume");

	 vtkIntArray* pStopeNbBlocksArray= vtkIntArray::New();
   pStopeNbBlocksArray->Allocate(nbCells);
	 ((vtkIntArray*)pStopeNbBlocksArray)->SetName("Stope NbOfBlocks");

	 vtkDoubleArray* pStopeLengthArray= vtkDoubleArray::New();
   pStopeLengthArray->Allocate(nbCells);
	 ((vtkDoubleArray*)pStopeLengthArray)->SetName("Stope Length");

	 vtkDoubleArray* pDownhillArray = vtkDoubleArray::New();
   pDownhillArray->Allocate(nbCells);
   ((vtkDoubleArray*)pDownhillArray)->SetName("Stope Downhill");

	 vtkDoubleArray* pHorizontalConstraintArray = vtkDoubleArray::New();
   pHorizontalConstraintArray->Allocate(nbCells);
   ((vtkDoubleArray*)pHorizontalConstraintArray)->SetName("Stope HzlConstraint");


	 for(int i=0; i<nbCells; i++)
	 {
		pts= dataset->GetCell(i)->GetPointIds();		
		id_S= (vtkIdType)this->stopeIdArray->GetTuple1(pts->GetId(0));
		this->Container->GetStope(id_S)->GetBounds(bounds);

		pStopeFitnessArray->InsertTuple1( pts->GetId(0), this->Container->GetStope(id_S)->GetFitness() );
		pStopeWeightArray->InsertTuple1( pts->GetId(0), this->Container->GetStope(id_S)->GetWeight() );
		pStopeVolumeArray->InsertTuple1( pts->GetId(0), this->Container->GetStope(id_S)->GetVolume() );
		pStopeNbBlocksArray->InsertTuple1( pts->GetId(0), this->Container->GetStope(id_S)->GetNumberOfBlocks() );
		pStopeLengthArray->InsertTuple1( pts->GetId(0), (bounds[3]-bounds[2]) );
		pDownhillArray->InsertTuple1( pts->GetId(0), (this->Allbounds[5]-bounds[5]));
		pHorizontalConstraintArray->InsertTuple1( pts->GetId(0), (this->Allbounds[1]-bounds[1]) );
	 }
	 
	 dataset->GetPointData()->AddArray(pStopeFitnessArray);
	 dataset->GetPointData()->AddArray(pStopeWeightArray);
	 dataset->GetPointData()->AddArray(pStopeVolumeArray);
	 dataset->GetPointData()->AddArray(pStopeNbBlocksArray);
	 dataset->GetPointData()->AddArray(pStopeLengthArray);
	 dataset->GetPointData()->AddArray(pDownhillArray);
	 dataset->GetPointData()->AddArray(pHorizontalConstraintArray);
	
	 pStopeFitnessArray->Delete();
	 pStopeWeightArray->Delete();
	 pStopeVolumeArray->Delete();
	 pStopeNbBlocksArray->Delete();
	 pStopeLengthArray->Delete();
	 pDownhillArray->Delete();
	 pHorizontalConstraintArray->Delete();	
}