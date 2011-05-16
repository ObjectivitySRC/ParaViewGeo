/*=========================================================================
  Program:   Visualization Toolkit
  Module:    $ vtkBlockNormalization.cxx $
  Authors:    Arolde VIDJINNAGNI 
  MIRARCO, Laurentian University
  Date:    July 2009 
  Version:   0.1
=========================================================================*/

#include "vtkBlockNormalization.h"
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

vtkCxxRevisionMacro ( vtkBlockNormalization, "$Revision: 0.3 $" );
vtkStandardNewMacro ( vtkBlockNormalization );

//========================================================================================
vtkBlockNormalization::vtkBlockNormalization(void)
{
	this->SetNumberOfInputPorts(1);
  this->SetNumberOfOutputPorts(1);
	
	this->XINC= NULL;
	this->YINC= NULL;
	this->ZINC= NULL;
  this->NormalSizeCX= 5;
  this->NormalSizeCY= 5;
  this->NormalSizeCZ= 5;

  this->Nbx=0;
  this->Nby=0;
  this->Nbz=0;

}


//========================================================================================
vtkBlockNormalization::~vtkBlockNormalization(void)
{
}
//----------------------------------------------------------------------------
int vtkBlockNormalization::FillInputPortInformation ( int vtkNotUsed(port), vtkInformation* info )
  {
   info->Set ( vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData", 0 );
  return 1;
  }

//----------------------------------------------------------------------------
int vtkBlockNormalization::FillOutputPortInformation(int vtkNotUsed(port), vtkInformation* info)
{
	info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkPolyData");	
  return 1;
}
//----------------------------------------------------------------------------
int vtkBlockNormalization::getBlockSize(vtkPolyData* dataset)
{
	 res[0]=0.0; res[1]=0.0;
	 res[2]=0.0; res[3]=0.0;
	 res[4]=0.0; res[5]=0.0;
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
			res[1]= (pt[0]+delta);
			res[0]= (pt[0]-delta);

			delta= (this->YINCArray->GetTuple1(pts->GetId(0))/2);
			res[3]= (pt[1]+delta);
			res[2]= (pt[1]-delta);

			delta= (this->ZINCArray->GetTuple1(pts->GetId(0))/2);
			res[5]= (pt[2]+delta);
			res[4]= (pt[2]-delta);	

		 }
		 else{
		  delta=0;
			pts= dataset->GetCell(i)->GetPointIds();			
			dataset->GetPoint(pts->GetId(0),pt);		

			delta= (this->XINCArray->GetTuple1(pts->GetId(0))/2); //size/2
			if( (pt[0]+delta)>res[1])
				res[1]= (pt[0]+delta);

			if( (pt[0]-delta)<res[0])
				res[0]= (pt[0]-delta);

			delta= (this->YINCArray->GetTuple1(pts->GetId(0))/2);
			if( (pt[1]+delta) >res[3])
				res[3]= (pt[1]+delta);

			if( (pt[1]-delta) <res[2])
				res[2]= (pt[1]-delta);

			delta= (this->ZINCArray->GetTuple1(pts->GetId(0))/2);
			if( (pt[2]+delta)>res[5])
				res[5]= (pt[2]+delta);

			if((pt[2]-delta)<res[4])
				res[4]= (pt[2]-delta);	
			}
	 }
	 	
  return 1;
}
//----------------------------------------------------------------------------
double vtkBlockNormalization::getVolumeIntersection(double cell1[3], double size1[3],
													double cell2[3], double size2[3])
{
	
	double deltaX=0, deltaY=0, deltaZ=0;
	
	if( ((cell2[0]-size2[0])>= (cell1[0]-size1[0]))&&((cell2[0]+size2[0])<= (cell1[0]+size1[0])) )
	deltaX= 2*size2[0];
	if( ((cell2[1]-size2[1])>= (cell1[1]-size1[1])) && ((cell2[1]+size2[1])<= (cell1[1]+size1[1])) )
	deltaY= 2*size2[1];
	if( ((cell2[2]-size2[2])>= (cell1[2]-size1[2]))&&((cell2[2]+size2[2])<= (cell1[2]+size1[2])) )
	deltaZ= 2*size2[2];

	if( (deltaX==0)&&((cell2[0]-size2[0])<= (cell1[0]-size1[0]))&&((cell2[0]+size2[0])>= (cell1[0]+size1[0])) )
	deltaX= 2*size1[0];
	if( (deltaY==0)&&((cell2[1]-size2[1])<= (cell1[1]-size1[1]))&&((cell2[1]+size2[1])>= (cell1[1]+size1[1])) )
	deltaY= 2*size1[1];
	if( (deltaZ==0)&&((cell2[2]-size2[2])<= (cell1[2]-size1[2]))&&((cell2[2]+size2[2])>= (cell1[2]+size1[2])) )
	deltaZ= 2*size1[2];

	if(deltaX==0)
	{
		if( (cell1[0]+size1[0])>=(cell2[0]+size2[0]) )
			deltaX= (cell2[0]+size2[0])- (cell1[0]-size1[0]);
		else if( (cell1[0]+size1[0])<=(cell2[0]+size2[0]) )
			deltaX= (cell1[0]+size1[0])- (cell2[0]-size2[0]);			
	}
	if(deltaY==0)
	{
		if( (cell1[1]+size1[1])>=(cell2[1]+size2[1]) )
		  deltaY= (cell2[1]+size2[1])- (cell1[1]-size1[1]);
		else if( (cell1[1]+size1[1])<=(cell2[1]+size2[1]) )
			deltaY= (cell1[1]+size1[1])- (cell2[1]-size2[1]);			
	}
	if(deltaZ==0)
	{
		if( (cell1[2]+size1[2])>=(cell2[2]+size2[2]) )
			deltaZ= (cell2[2]+size2[2])- (cell1[2]-size1[2]);
		else if( (cell1[2]+size1[2])<=(cell2[2]+size2[2]) )
			deltaZ= (cell1[2]+size1[2])- (cell2[2]-size2[2]);
	}	
	
	return(deltaX*deltaY*deltaZ);
}

//----------------------------------------------------------------------------
int vtkBlockNormalization::getReplaceValues( vtkPolyData* tempInput)
{
	int ip0=0, jp0=0, kp0=0;
	int ip1=0, jp1=0, kp1=0;
	int nbCells= tempInput->GetNumberOfCells();
	vtkIdList* pts;
	double pt[3], size[3];
	double cell[3], cellSize[3];
	double volume=0.0;
	double interpolatedValue=0, tempvalue=0;
	int id=0;
	int nbProp= tempInput->GetPointData()->GetNumberOfArrays();

	cellSize[0]= this->NormalSizeCX/2;
	cellSize[1]= this->NormalSizeCY/2;
	cellSize[2]= this->NormalSizeCZ/2;

	double* temptable = new double[this->Nbx*this->Nby*this->Nbz];	
	int* threshold_table = new int[this->Nbx*this->Nby*this->Nbz];
	for(int i=0; i<(this->Nbx*this->Nby*this->Nbz); i++)
			threshold_table[i]=1;
	bool tresh = false;
  int p1=0;
	for(int p=0; p<nbProp; p++)
	{
		if( (strcmp(tempInput->GetPointData()->GetArrayName(p), this->XINC)!=0)
			&&( strcmp(tempInput->GetPointData()->GetArrayName(p), this->YINC)!=0)
			&&( strcmp(tempInput->GetPointData()->GetArrayName(p), this->ZINC)!=0))
		{		
			for(int i=0; i<(this->Nbx*this->Nby*this->Nbz); i++)
			temptable[i]=0;

			for(int i=0; i<nbCells; i++)
			 {
				pts= tempInput->GetCell(i)->GetPointIds();
				tempInput->GetPoint(pts->GetId(0),pt);
				size[0]= this->XINCArray->GetTuple1(pts->GetId(0))/2;
				size[1]= this->YINCArray->GetTuple1(pts->GetId(0))/2;
				size[2]= this->ZINCArray->GetTuple1(pts->GetId(0))/2;

				ip0= ((pt[0]-size[0])-res[0])/this->NormalSizeCX;
				ip1= ((pt[0]+size[0])-res[0])/this->NormalSizeCX;
				jp0= ((pt[1]-size[1])-res[2])/this->NormalSizeCY;
				jp1= ((pt[1]+size[1])-res[2])/this->NormalSizeCY;
				kp0= ((pt[2]-size[2])-res[4])/this->NormalSizeCZ;
				kp1= ((pt[2]+size[2])-res[4])/this->NormalSizeCZ;

				for(int i=ip0; i<ip1; i++)
				{
					for(int j=jp0; j<jp1; j++)
					{
						for(int k=kp0; k<kp1; k++)
						{
							cell[0]= this->res[0]+ (2*i+1)*(this->NormalSizeCX/2);
							cell[1]= this->res[2]+ (2*j+1)*(this->NormalSizeCY/2);
							cell[2]= this->res[4]+ (2*k+1)*(this->NormalSizeCZ/2);
											
							id= i*(this->Nbz*this->Nby) + j*this->Nbz +k; 
							volume=	getVolumeIntersection(cell, cellSize, pt, size);
							temptable[id] = temptable[id]+(tempInput->GetPointData()->GetArray(p)->GetTuple1(pts->GetId(0)))*
								(volume/(this->NormalSizeCX*this->NormalSizeCY*this->NormalSizeCZ));
							
							if(tresh==false)
							{
								if(volume!=0)
									threshold_table[id]=0;														
							}							
						}
					}				
				}
			}
			tresh=true;
			for(int i=0; i<(this->Nbx*this->Nby*this->Nbz); i++)
				arrays[p1]->InsertTuple1(i, temptable[i]);
			p1++;
		}
	}
	
	for(int i=0; i<(this->Nbx*this->Nby*this->Nbz); i++)
		threshold->InsertTuple1(i,threshold_table[i]);

	delete temptable;
	delete threshold_table;

	return 1;
}
//----------------------------------------------------------------------------
void vtkBlockNormalization::SetBlocks(vtkPolyData* tempInput, 
																			vtkPolyData* tempoutput)
{
	vtkPoints* Points= vtkPoints::New();
	vtkCellArray* Cells= vtkCellArray::New();
	int id=0;
	double currentPoint[3];

	for(int i=0; i<this->Nbx; i++)
	{
		for(int j=0; j<this->Nby; j++)
	  {
			for(int k=0; k<this->Nbz; k++)
			{				
				currentPoint[0]= res[0]+ (2*i+1)*(this->NormalSizeCX/2);
				currentPoint[1]= res[2]+ (2*j+1)*(this->NormalSizeCY/2);
				currentPoint[2]= res[4]+ (2*k+1)*(this->NormalSizeCZ/2);

				Points->InsertNextPoint(currentPoint);
				Cells->InsertNextCell(1);
				Cells->InsertCellPoint(id);
				id++;
			}	
	  }
	}

	tempoutput->SetPoints(Points);
	tempoutput->SetVerts(Cells);

	Points->Delete();
	Cells->Delete();
}

//----------------------------------------------------------------------------
int vtkBlockNormalization::RequestData( vtkInformation *request, 
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

	getBlockSize(input);

	this->Nbx = (int)((res[1]- res[0])/this->NormalSizeCX);
	if( (this->Nbx*this->NormalSizeCX)< (this->res[1]- this->res[0]))
			this->Nbx++;

	this->Nby = (int)((this->res[3]- this->res[2])/this->NormalSizeCY);
	if( (this->Nby*this->NormalSizeCY)< (this->res[3]- this->res[2]))
	  this->Nby++;
	
	this->Nbz = (int)((this->res[5]- this->res[4])/this->NormalSizeCZ);
	if( (this->Nbz*this->NormalSizeCZ)< (this->res[5]- this->res[4]))
		this->Nbz++;

	threshold= vtkIntArray::New();	
	threshold->Allocate(this->Nbx*this->Nby*this->Nbz);
	threshold->SetName("ExtraBlocks");

	int nbArrays= input->GetPointData()->GetNumberOfArrays();
	if(nbArrays>3)
	{
		arrays= new vtkDataArray*[nbArrays-3]; 		
		int p1=0;
		for(int p=0; p< nbArrays; p++)
		{		
				if( ( strcmp(input->GetPointData()->GetArrayName(p), this->XINC)!=0)
						&&( strcmp(input->GetPointData()->GetArrayName(p), this->YINC)!=0)
						&&( strcmp(input->GetPointData()->GetArrayName(p), this->ZINC)!=0)
					)
				{	
					arrays[p1]= vtkDoubleArray::New();
					arrays[p1]->Allocate(this->Nbx*this->Nby*this->Nbz);
					arrays[p1]->SetName(input->GetPointData()->GetArrayName(p));
					p1++;
				}
		}
	}

	getReplaceValues(input);
	SetBlocks(input, output);
	output->GetPointData()->AddArray(threshold);

	for(int p=0; p<(nbArrays-3); p++)
	{
		output->GetPointData()->AddArray(arrays[p]);
		arrays[p]->Delete();
	}

	threshold->Delete();	
	
  return 1;
}
