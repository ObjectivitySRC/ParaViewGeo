/*=========================================================================
  Program:   Visualization Toolkit
  Module:    $ vtkCompleteBlockFilter.cxx $
  Authors:    Arolde VIDJINNAGNI 
  MIRARCO, Laurentian University
  Date:    July 2009 
  Version:   0.1
=========================================================================*/

#include "vtkCompleteBlockFilter.h"
#include "vtkPointSet.h"
#include "vtkPoints.h"
#include "vtkDataSet.h"
#include "vtkUnstructuredGrid.h"
#include "vtkDoubleArray.h"
#include "vtkPointData.h"
#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkPointData.h"
#include "vtkDataArray.h"	
#include "vtkMath.h"
#include "vtkIdTypeArray.h"
#include "vtkIdList.h"
#include "vtkDataArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkAppendPolyData.h"

vtkCxxRevisionMacro ( vtkCompleteBlockFilter, "$Revision: 0.3 $" );
vtkStandardNewMacro ( vtkCompleteBlockFilter );

//========================================================================================
vtkCompleteBlockFilter::vtkCompleteBlockFilter(void)
{
	this->SetNumberOfInputPorts(1);
  this->SetNumberOfOutputPorts(1);
	
	this->XINC= NULL;
	this->YINC= NULL;
	this->ZINC= NULL;

	NormalSizeCX=0;
	NormalSizeCY=0;
	NormalSizeCZ=0;
 
  this->Nbx=0;
  this->Nby=0;
  this->Nbz=0;

	this->BlockModelType =0;
	this->numberOfNewPoints=0;
	this->arrays=NULL;
	this->nbArrays=0;

}


//========================================================================================
vtkCompleteBlockFilter::~vtkCompleteBlockFilter(void)
{
}
//----------------------------------------------------------------------------
int vtkCompleteBlockFilter::FillInputPortInformation ( int vtkNotUsed(port), vtkInformation* info )
  {
   info->Set ( vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData", 0 );
  return 1;
  }

//----------------------------------------------------------------------------
int vtkCompleteBlockFilter::FillOutputPortInformation(int vtkNotUsed(port), vtkInformation* info)
{
	info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkPolyData");	
  return 1;
}
//----------------------------------------------------------------------------
int vtkCompleteBlockFilter::getBlockSize(vtkPolyData* dataset)
{
	 AllBounds[0]=0.0; AllBounds[1]=0.0;
	 AllBounds[2]=0.0; AllBounds[3]=0.0;
	 AllBounds[4]=0.0; AllBounds[5]=0.0;
	 int nbCells= dataset->GetNumberOfCells();
	 vtkIdList* pts /*= vtkIdList::New()*/;
	 double pt[3];
	 double delta=0;

	 for(int i=0; i<nbCells; i++)
	 {
		 if(i==0)
		 {
			delta=0;
			pts= dataset->GetCell(i)->GetPointIds();			
			dataset->GetPoint(pts->GetId(0),pt);		

			delta= (this->XINCArray->GetTuple1(pts->GetId(0))/2); //size/2
			AllBounds[1]= (pt[0]+delta);
			AllBounds[0]= (pt[0]-delta);

			delta= (this->YINCArray->GetTuple1(pts->GetId(0))/2);
			AllBounds[3]= (pt[1]+delta);
			AllBounds[2]= (pt[1]-delta);

			delta= (this->ZINCArray->GetTuple1(pts->GetId(0))/2);
			AllBounds[5]= (pt[2]+delta);
			AllBounds[4]= (pt[2]-delta);	

		 }
		 else{
		  delta=0;
			pts= dataset->GetCell(i)->GetPointIds();			
			dataset->GetPoint(pts->GetId(0),pt);		

			delta= (this->XINCArray->GetTuple1(pts->GetId(0))/2); //size/2
			if( (pt[0]+delta)>AllBounds[1])
				AllBounds[1]= (pt[0]+delta);

			if( (pt[0]-delta)<AllBounds[0])
				AllBounds[0]= (pt[0]-delta);

			delta= (this->YINCArray->GetTuple1(pts->GetId(0))/2);
			if( (pt[1]+delta) >AllBounds[3])
				AllBounds[3]= (pt[1]+delta);

			if( (pt[1]-delta) <AllBounds[2])
				AllBounds[2]= (pt[1]-delta);

			delta= (this->ZINCArray->GetTuple1(pts->GetId(0))/2);
			if( (pt[2]+delta)>AllBounds[5])
				AllBounds[5]= (pt[2]+delta);

			if((pt[2]-delta)<AllBounds[4])
				AllBounds[4]= (pt[2]-delta);	
			}
	 }
	 	
  return 1;
}


//----------------------------------------------------------------------------
int  vtkCompleteBlockFilter::CompleIRegularB(vtkPolyData* dataset, vtkPolyData* out)
{

	return 1;
}



//----------------------------------------------------------------------------
int  vtkCompleteBlockFilter::CompleRegularB(vtkPolyData* dataset, vtkPolyData* out)
{
	vtkPoints* Points= vtkPoints::New();
	vtkCellArray* Cells= vtkCellArray::New();
	vtkIdList *pt = vtkIdList::New();	
	double center[3];
	int x, y, z; 
	int Id; 

	this->pointsTable= new vtkIdType**[this->Nbx];
	for(int i=0; i<this->Nbx; i++)
	{
	  this->pointsTable[i]= new vtkIdType*[this->Nby];
	  for(int j=0; j<this->Nby; j++)
	  {
			this->pointsTable[i][j]= new vtkIdType[this->Nbz];
			for(int k=0; k<this->Nbz; k++)
				this->pointsTable[i][j][k]= -1;
		}
	}

	for(int i=0; i<dataset->GetNumberOfCells(); i++)
	{
		pt = dataset->GetCell(i)->GetPointIds();
	  dataset->GetPoint(pt->GetId(0), center);

		x= (int)((center[0]-this->AllBounds[0])/this->NormalSizeCX);	   
		y= (int)((center[1]-this->AllBounds[2])/this->NormalSizeCY);	   
		z= (int)((center[2]-this->AllBounds[4])/this->NormalSizeCZ);	
		this->pointsTable[x][y][z]= pt->GetId(0);
		Points->InsertNextPoint(center);
		Id= Cells->InsertNextCell(1);
		Cells->InsertCellPoint(Id); Id++;
		for(int p=0; p<this->nbArrays; p++)
		{
			this->arrays[p]->InsertNextTuple1(dataset->GetPointData()->GetArray(p)->GetTuple1(pt->GetId(0)));		
		}
		threshold->InsertNextTuple1(0);
		
	}

	//generate the new output with the adding of the gaps
	for(int i=0; i<this->Nbx; i++)
	{
		for(int j=0; j<this->Nby; j++)
		{
			for(int k=0; k<this->Nbz; k++)
			{
				if(this->pointsTable[i][j][k]==-1)
				{
					center[0]= this->AllBounds[0]+ (2*i+1)*(this->NormalSizeCX/2);
					center[1]= this->AllBounds[2]+ (2*j+1)*(this->NormalSizeCY/2);
					center[2]= this->AllBounds[4]+ (2*k+1)*(this->NormalSizeCZ/2);

					Points->InsertNextPoint(center);
					Cells->InsertNextCell(1);
					Cells->InsertCellPoint(Id); Id++;
					for(int p=0; p<this->nbArrays; p++)
					{
						if(this->pDimension[0]==p)
							this->arrays[p]->InsertNextTuple1(this->NormalSizeCX);
						else if(this->pDimension[1]==p)
							this->arrays[p]->InsertNextTuple1(this->NormalSizeCY);
						else if(this->pDimension[2]==p)
							this->arrays[p]->InsertNextTuple1(this->NormalSizeCZ);
						else this->arrays[p]->InsertNextTuple1(0);						
					}
					threshold->InsertNextTuple1(1);
				}
			}
		}
	}

	out->SetPoints(Points);
	out->SetVerts(Cells);
	Points->Delete();
	Cells->Delete();

	for(int p=0; p<this->nbArrays; p++)
	{
		out->GetPointData()->AddArray(this->arrays[p]);
		this->arrays[p]->Delete();
	}
	out->GetPointData()->AddArray(threshold);
	threshold->Delete();

	for(int i=0; i<this->Nbx; i++)
	{
		for(int j=0; j<this->Nby; j++)
		{
			delete[] this->pointsTable[i][j];
		}
		delete[] this->pointsTable[i];
	}

	return 1;
}

//----------------------------------------------------------------------------
int vtkCompleteBlockFilter::RequestData( vtkInformation *request, 
                         vtkInformationVector **InputVector, 
                         vtkInformationVector *outputVector )
{
   vtkInformation *inputInfo = InputVector[0]->GetInformationObject ( 0 );
   vtkInformation *outInfo = outputVector->GetInformationObject ( 0 );

   vtkPolyData* input = vtkPolyData::SafeDownCast (
                         inputInfo->Get ( vtkDataObject::DATA_OBJECT() ) );
	
   vtkPolyData *output = vtkPolyData::SafeDownCast (
   outInfo->Get ( vtkDataObject::DATA_OBJECT() ) );

	this->XINCArray= vtkDoubleArray::SafeDownCast(input->GetPointData()->GetArray(this->XINC));
	this->YINCArray= vtkDoubleArray::SafeDownCast(input->GetPointData()->GetArray(this->YINC));
	this->ZINCArray= vtkDoubleArray::SafeDownCast(input->GetPointData()->GetArray(this->ZINC));
	
	if((this->XINCArray==NULL)||(this->YINCArray==NULL)||(this->ZINCArray==NULL))
	{
		vtkErrorMacro("Size arrays are not set on the block model");
	}	
	threshold= vtkIntArray::New();	
	threshold->SetName("Gaps");

	this->nbArrays= input->GetPointData()->GetNumberOfArrays();
	this->arrays= new vtkDataArray*[this->nbArrays]; 
	for(int p=0; p<this->nbArrays; p++)
	{	
		if(strcmp(input->GetPointData()->GetArrayName(p), this->XINC)==0)
			pDimension[0]= p;
		if(strcmp(input->GetPointData()->GetArrayName(p), this->YINC)==0)
			pDimension[1]= p;
		if(strcmp(input->GetPointData()->GetArrayName(p), this->ZINC)==0)
			pDimension[2]= p;

		// need here to test the type of the current array
		this->arrays[p]= vtkDoubleArray::New();
		this->arrays[p]->SetName(input->GetPointData()->GetArrayName(p));
	}

	if(this->BlockModelType==1)
	{
		//getBlockSize(input);
		output->ShallowCopy(input);
	}
	else if(this->BlockModelType==0)
		{
			double range[2]={0.0};

			this->XINCArray->GetRange(range);
			this->NormalSizeCX= range[0];
			range[0]=0.0; range[1]=0.0;

			this->YINCArray->GetRange(range);
			this->NormalSizeCY= range[0];
			range[0]=0.0; range[1]=0.0;

			this->ZINCArray->GetRange(range);
			this->NormalSizeCZ= range[0];

		input->GetBounds(this->AllBounds);
		this->AllBounds[0]-= this->NormalSizeCX/2;
		this->AllBounds[1]+= this->NormalSizeCX/2;
		this->AllBounds[2]-= this->NormalSizeCY/2;
		this->AllBounds[3]+= this->NormalSizeCY/2;
		this->AllBounds[4]-= this->NormalSizeCZ/2;
		this->AllBounds[5]+= this->NormalSizeCZ/2;
		this->Nbx = (int)((AllBounds[1]- AllBounds[0])/this->NormalSizeCX);
		if( (this->Nbx*this->NormalSizeCX)< (this->AllBounds[1]- this->AllBounds[0]))
				this->Nbx++;

		this->Nby = (int)((this->AllBounds[3]- this->AllBounds[2])/this->NormalSizeCY);
		if( (this->Nby*this->NormalSizeCY)< (this->AllBounds[3]- this->AllBounds[2]))
			this->Nby++;
		
		this->Nbz = (int)((this->AllBounds[5]- this->AllBounds[4])/this->NormalSizeCZ);
		if( (this->Nbz*this->NormalSizeCZ)< (this->AllBounds[5]- this->AllBounds[4]))
			this->Nbz++;
		this->numberOfNewPoints = this->Nbx*this->Nby*this->Nbz;

	  CompleRegularB(input, output);
		}	
		
  return 1;
}
