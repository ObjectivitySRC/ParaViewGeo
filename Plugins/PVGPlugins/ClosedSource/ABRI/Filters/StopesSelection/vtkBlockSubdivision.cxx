/*=========================================================================
  Program:   Visualization Toolkit
  Module:    $ vtkBlockSubdivision.cxx $
  Authors:    Arolde VIDJINNAGNI
  MIRARCO, Laurentian University
  Date:    August 2009 
  Version:   0.1
=========================================================================*/

#include "vtkBlockSubdivision.h"
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
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkstd/list"
#include "vtkCleanPolyData.h"
#include "vtkThreshold.h"
#include "vtkDataSetSurfaceFilter.h"
#include "vtkUnstructuredGrid.h"
#include "vtkXMLUtilities.h"
#include "vtkXMLDataElement.h"
#include "vtkArrayCalculator.h"
vtkCxxRevisionMacro ( vtkBlockSubdivision, "$Revision: 0.3 $" );
vtkStandardNewMacro ( vtkBlockSubdivision );



//========================================================================================
vtkBlockSubdivision::vtkBlockSubdivision(void)
{
	this->SetNumberOfInputPorts(1);
  this->SetNumberOfOutputPorts(1);

	boundMinX=0; 
  boundMinY=0;
	boundMinZ=0;
	boundMaxX=0;
  boundMaxY=0;
	boundMaxZ=0;
	Direction =2;
	this->XINC= NULL;
	this->YINC= NULL;
	this->ZINC= NULL;
	newCellId= 0;
}

//========================================================================================
vtkBlockSubdivision::~vtkBlockSubdivision(void)
{
}
//========================================================================================
int vtkBlockSubdivision::FillInputPortInformation ( int vtkNotUsed(port), vtkInformation* info )
  {
   info->Set ( vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData", 0 );
  return 1;
  }

//========================================================================================
int vtkBlockSubdivision::FillOutputPortInformation(int vtkNotUsed(port), vtkInformation* info)
{
	info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkPolyData");	
  return 1;
}



/**********************************************************************************************/
/* Name: getIntersectionWithLayerByX																													*/
/*																																													  */
/* Description:																																								*/
/*																																														*/
/* parameters: input(vtkPolyData*): input of the filter																				*/
/*																																														*/
/* return value: int:		0 if there is no intersection with the layer													*/
/*											1 and return the cell bounds needed to create the new cell						*/
/**********************************************************************************************/
int vtkBlockSubdivision::getIntersectionWithLayerByX(	vtkPolyData* inDataset,
																										  vtkIdType id, double XLMin,
																										  double XLMax,	double bounds[6])
{
	double pt[3], ptSize[3];
	ptSize[0]=0; ptSize[1]=0; ptSize[2]=0;
	bounds[0]=0; bounds[1]=0; bounds[2]=0; 
	bounds[3]=0; bounds[4]=0; bounds[5]=0;

	inDataset->GetPoint(id,pt);
	ptSize[0]= this->XINCArray->GetTuple1(id)/2;
	ptSize[1]= this->YINCArray->GetTuple1(id)/2;
	ptSize[2]= this->ZINCArray->GetTuple1(id)/2;

	//first check...if the cell bounds is not in the layer then return 0
	if( ((pt[0]-ptSize[0])>=XLMax)||((pt[0]+ptSize[0])<=XLMin) )
		return 0;
	//second check...the cell bounds is inside the layer
	else if( ((pt[0]-ptSize[0])>=XLMin)&&((pt[0]+ptSize[0])<=XLMax) )
	{
		bounds[0]= pt[0]-ptSize[0];
		bounds[1]= pt[0]+ptSize[0];
		bounds[2]= pt[1]-ptSize[1]; 
		bounds[3]= pt[1]+ptSize[1];
		bounds[4]= pt[2]-ptSize[2];
		bounds[5]= pt[2]+ptSize[2];
		return 1;
	}
	else{
		//intersection by the top of the current layer
		if( ((pt[0]-ptSize[0])>=XLMin)&&((pt[0]-ptSize[0])<=XLMax)&&((pt[0]+ptSize[0])>=XLMax) )
		{
			bounds[0]= pt[0]-ptSize[0];
			bounds[1]= XLMax;
			bounds[2]= pt[1]-ptSize[1]; 
			bounds[3]= pt[1]+ptSize[1];
			bounds[4]= pt[2]-ptSize[2];
			bounds[5]= pt[2]+ptSize[2];			
		}
		//intersection by the buttom of the current layer
		else if( ((pt[0]+ptSize[0])>=XLMin)&&((pt[0]+ptSize[0])<=XLMax)&&((pt[0]-ptSize[0])<=XLMin) )
		{
			bounds[0]= XLMin;
			bounds[1]= pt[0]+ptSize[0];
			bounds[2]= pt[1]-ptSize[1]; 
			bounds[3]= pt[1]+ptSize[1];
			bounds[4]= pt[2]-ptSize[2];
			bounds[5]= pt[2]+ptSize[2];
		}
		//intersection with the top and the buttom of the current layer
		else if( ((pt[0]+ptSize[0])>=XLMax)&&((pt[0]-ptSize[0])<=XLMin) )
		{
			bounds[0]= XLMin;
			bounds[1]= XLMax;
			bounds[2]= pt[1]-ptSize[1]; 
			bounds[3]= pt[1]+ptSize[1];
			bounds[4]= pt[2]-ptSize[2];
			bounds[5]= pt[2]+ptSize[2];
		}
		return 1;
	}
}


/**********************************************************************************************/
/* Name: getIntersectionWithLayerByY																													*/
/*																																													  */
/* Description:																																								*/
/*																																														*/
/* parameters: input(vtkPolyData*): input of the filter																				*/
/*																																														*/
/* return value: int:		0 if there is no intersection with the layer													*/
/*											1 and return the cell bounds needed to create the new cell						*/
/**********************************************************************************************/
int vtkBlockSubdivision::getIntersectionWithLayerByY(	vtkPolyData* inDataset,
																										  vtkIdType id, double YLMin,
																										  double YLMax,	double bounds[6])
{
	double pt[3], ptSize[3];
	ptSize[0]=0; ptSize[1]=0; ptSize[2]=0;
	bounds[0]=0; bounds[1]=0; bounds[2]=0; 
	bounds[3]=0; bounds[4]=0; bounds[5]=0;

	inDataset->GetPoint(id,pt);
	ptSize[0]= this->XINCArray->GetTuple1(id)/2;
	ptSize[1]= this->YINCArray->GetTuple1(id)/2;
	ptSize[2]= this->ZINCArray->GetTuple1(id)/2;

	//first check...if the cell bounds is not in the layer then return 0
	if( ((pt[1]-ptSize[1])>=YLMax)||((pt[1]+ptSize[1])<=YLMin) )
		return 0;
	//second check...the cell bounds is inside the layer
	else if( ((pt[1]-ptSize[1])>=YLMin)&&((pt[1]+ptSize[1])<=YLMax) )
	{
		bounds[0]= pt[0]-ptSize[0];
		bounds[1]= pt[0]+ptSize[0];
		bounds[2]= pt[1]-ptSize[1]; 
		bounds[3]= pt[1]+ptSize[1];
		bounds[4]= pt[2]-ptSize[2];
		bounds[5]= pt[2]+ptSize[2];
		return 1;
	}
	else{
		//intersection by the top of the current layer
		if( ((pt[1]-ptSize[1])>=YLMin)&&((pt[1]-ptSize[1])<=YLMax)&&((pt[1]+ptSize[1])>=YLMax) )
		{
			bounds[0]= pt[0]-ptSize[0];
			bounds[1]= pt[0]+ptSize[0];
			bounds[2]= pt[1]-ptSize[1]; 
			bounds[3]= YLMax;
			bounds[4]= pt[2]-ptSize[2];
			bounds[5]= pt[2]+ptSize[2];			
		}
		//intersection by the buttom of the current layer
		else if( ((pt[1]+ptSize[1])>=YLMin)&&((pt[1]+ptSize[1])<=YLMax)&&((pt[1]-ptSize[1])<=YLMin) )
		{
			bounds[0]= pt[0]-ptSize[0];
			bounds[1]= pt[0]+ptSize[0];
			bounds[2]= YLMin; 
			bounds[3]= pt[1]+ptSize[1];
			bounds[4]= pt[2]-ptSize[2];
			bounds[5]= pt[2]+ptSize[2];
		}
		//intersection with the top and the buttom of the current layer
		else if( ((pt[1]+ptSize[1])>=YLMax)&&((pt[1]-ptSize[1])<=YLMin) )
		{
			bounds[0]= pt[0]-ptSize[0];
			bounds[1]= pt[0]+ptSize[0];
			bounds[2]= YLMin; 
			bounds[3]= YLMax;
			bounds[4]= pt[2]-ptSize[2];
			bounds[5]= pt[2]+ptSize[2];
		}
		return 1;
	}
}


/**********************************************************************************************/
/* Name: getIntersectionWithLayerByZ																													*/
/*																																													  */
/* Description:																																								*/
/*																																														*/
/* parameters: input(vtkPolyData*): input of the filter																				*/
/*																																														*/
/* return value: int:		0 if there is no intersection with the layer													*/
/*											1 and return the cell bounds needed to create the new cell						*/
/**********************************************************************************************/
int vtkBlockSubdivision::getIntersectionWithLayerByZ(	vtkPolyData* inDataset,
																										vtkIdType id, double ZLMin,
																										double ZLMax,	double bounds[6])
{
	double pt[3], ptSize[3];
	ptSize[0]=0; ptSize[1]=0; ptSize[2]=0;
	bounds[0]=0; bounds[1]=0; bounds[2]=0; 
	bounds[3]=0; bounds[4]=0; bounds[5]=0;

	inDataset->GetPoint(id,pt);
	ptSize[0]= this->XINCArray->GetTuple1(id)/2;
	ptSize[1]= this->YINCArray->GetTuple1(id)/2;
	ptSize[2]= this->ZINCArray->GetTuple1(id)/2;

	//first check...if the cell bounds is not in the layer then return 0
	if( ((pt[2]-ptSize[2])>=ZLMax)||((pt[2]+ptSize[2])<=ZLMin) )
		return 0;
	//second check...the cell bounds is inside the layer
	else if( ((pt[2]-ptSize[2])>=ZLMin)&&((pt[2]+ptSize[2])<=ZLMax) )
	{
		bounds[0]= pt[0]-ptSize[0];
		bounds[1]= pt[0]+ptSize[0];
		bounds[2]= pt[1]-ptSize[1]; 
		bounds[3]= pt[1]+ptSize[1];
		bounds[4]= pt[2]-ptSize[2];
		bounds[5]= pt[2]+ptSize[2];
		return 1;
	}
	else{
		//intersection by the top of the current layer
		if( ((pt[2]-ptSize[2])>=ZLMin)&&((pt[2]-ptSize[2])<=ZLMax)&&((pt[2]+ptSize[2])>=ZLMax) )
		{
			bounds[0]= pt[0]-ptSize[0];
			bounds[1]= pt[0]+ptSize[0];
			bounds[2]= pt[1]-ptSize[1]; 
			bounds[3]= pt[1]+ptSize[1];
			bounds[4]= pt[2]-ptSize[2];
			bounds[5]= ZLMax;			
		}
		//intersection by the buttom of the current layer
		else if( ((pt[2]+ptSize[2])>=ZLMin)&&((pt[2]+ptSize[2])<=ZLMax)&&((pt[2]-ptSize[2])<=ZLMin) )
		{
			bounds[0]= pt[0]-ptSize[0];
			bounds[1]= pt[0]+ptSize[0];
			bounds[2]= pt[1]-ptSize[1]; 
			bounds[3]= pt[1]+ptSize[1];
			bounds[4]= ZLMin;
			bounds[5]= pt[2]+ptSize[2];
		}
		//intersection with the top and the buttom of the current layer
		else if( ((pt[2]+ptSize[2])>=ZLMax)&&((pt[2]-ptSize[2])<=ZLMin) )
		{
			bounds[0]= pt[0]-ptSize[0];
			bounds[1]= pt[0]+ptSize[0];
			bounds[2]= pt[1]-ptSize[1]; 
			bounds[3]= pt[1]+ptSize[1];
			bounds[4]= ZLMin;
			bounds[5]= ZLMax;
		}
		return 1;
	}
}



/**********************************************************************************************/
/* Name: shrinkBlockModelByLayersByX																													*/
/*																																													  */
/* Description:																																								*/
/*																																														*/
/* parameters: input(vtkPolyData*): input of the filter																				*/
/*																																														*/
/* return value: int:																																					*/
/*																																														*/
/**********************************************************************************************/
void vtkBlockSubdivision::shrinkBlockModelByLayersByX( double sizeOfLayer, 
																										   vtkPolyData* inDataset, 
																										   vtkPolyData* outDataset)
{
	int nbLayers= (this->Allbounds[1]-this->Allbounds[0])/this->DeltaByLayer; //get the number of layers in the block model
	if( (nbLayers*this->DeltaByLayer)<(this->Allbounds[1]-this->Allbounds[0]))
		nbLayers++;
	int numberOfCells= inDataset->GetNumberOfCells();
	int nbArrays= inDataset->GetPointData()->GetNumberOfArrays();

	vtkIdList* pts;
	vtkPoints* Points= vtkPoints::New();
	vtkCellArray* Cells= vtkCellArray::New();
	
	double newPoint[3];
	double newSize[3];
	double cellBounds[6];

	//layer specification
	double XMin=0, XMax=0;
	int p1=0;


	for(int currentLayer=0; currentLayer<nbLayers; currentLayer++)
	{
		XMin= this->Allbounds[0] + currentLayer*this->DeltaByLayer;
		XMax= this->Allbounds[0] + (currentLayer+1)*this->DeltaByLayer;

		for(int i=0; i<numberOfCells; i++)
		{
			pts= inDataset->GetCell(i)->GetPointIds();			
			cellBounds[0]=0; cellBounds[1]=0; cellBounds[2]=0; 
			cellBounds[3]=0; cellBounds[4]=0; cellBounds[5]=0;

			if( this->getIntersectionWithLayerByX( inDataset, pts->GetId(0),XMin, XMax, cellBounds)!=0)
			{
				newPoint[0]= cellBounds[0]+ (cellBounds[1]-cellBounds[0])/2;
				newPoint[1]= cellBounds[2]+ (cellBounds[3]-cellBounds[2])/2;
				newPoint[2]= cellBounds[4]+ (cellBounds[5]-cellBounds[4])/2;

				newSize[0]= cellBounds[1]-cellBounds[0];
				newSize[1]= cellBounds[3]-cellBounds[2];
				newSize[2]= cellBounds[5]-cellBounds[4];

				Points->InsertNextPoint(newPoint);
				Cells->InsertNextCell(1);
				Cells->InsertCellPoint(this->newCellId);

				this->newXINCArray->InsertNextTuple1(newSize[0]);
				this->newYINCArray->InsertNextTuple1(newSize[1]);
				this->newZINCArray->InsertNextTuple1(newSize[2]);
				this->layersArray->InsertNextTuple1(currentLayer);
				this->oldPointId->InsertNextTuple1(pts->GetId(0));
				
				p1=0;
				for(int p=0; p<nbArrays; p++)
				{		
					if( ( strcmp(inDataset->GetPointData()->GetArrayName(p), this->XINC)!=0)
							&&( strcmp(inDataset->GetPointData()->GetArrayName(p), this->YINC)!=0)
							&&( strcmp(inDataset->GetPointData()->GetArrayName(p), this->ZINC)!=0)
						)
					{	
						this->arrays[p1]->InsertNextTuple1(inDataset->GetPointData()->GetArray(p)->GetTuple1(pts->GetId(0)));					
						p1++;
					}
				}

				this->newCellId++;
			}
		}
	}
	this->layersArray->SetName("LayerIDByX");
	this->oldPointId->SetName("oldPointIdByX");
  outDataset->SetPoints(Points);
	outDataset->SetVerts(Cells);
	outDataset->GetPointData()->AddArray(this->newXINCArray);
	outDataset->GetPointData()->AddArray(this->newYINCArray);
	outDataset->GetPointData()->AddArray(this->newZINCArray);
	outDataset->GetPointData()->AddArray(this->layersArray);
	outDataset->GetPointData()->AddArray(this->oldPointId);
	//FREE UP MEMORY
	for(int p=0; p<(nbArrays-3); p++)
	{
		outDataset->GetPointData()->AddArray(this->arrays[p]);
		this->arrays[p]->Delete();
	}

	this->newXINCArray->Delete();
	this->newYINCArray->Delete();
	this->newZINCArray->Delete();
	this->layersArray->Delete();
	this->oldPointId->Delete();
	Points->Delete();
	Cells->Delete();

}


/**********************************************************************************************/
/* Name: shrinkBlockModelByLayersByY																												  */
/*																																													  */
/* Description:																																								*/
/*																																														*/
/* parameters: input(vtkPolyData*): input of the filter																				*/
/*																																														*/
/* return value: int:																																					*/
/*																																														*/
/**********************************************************************************************/
void vtkBlockSubdivision::shrinkBlockModelByLayersByY( double sizeOfLayer, 
																										   vtkPolyData* inDataset, 
																										   vtkPolyData* outDataset)
{
	int nbLayers= (this->Allbounds[3]-this->Allbounds[2])/this->DeltaByLayer; //get the number of layers in the block model
	if( (nbLayers*this->DeltaByLayer)<(this->Allbounds[3]-this->Allbounds[2]))
		nbLayers++;
	int numberOfCells= inDataset->GetNumberOfCells();
	int nbArrays= inDataset->GetPointData()->GetNumberOfArrays();

	vtkIdList* pts;
	vtkPoints* Points= vtkPoints::New();
	vtkCellArray* Cells= vtkCellArray::New();
	
	double newPoint[3];
	double newSize[3];
	double cellBounds[6];

	//layer specification
	double YMin=0, YMax=0;
	int p1=0;


	for(int currentLayer=0; currentLayer<nbLayers; currentLayer++)
	{
		YMin= this->Allbounds[2] + currentLayer*this->DeltaByLayer;
		YMax= this->Allbounds[2] + (currentLayer+1)*this->DeltaByLayer;

		for(int i=0; i<numberOfCells; i++)
		{
			pts= inDataset->GetCell(i)->GetPointIds();			
			cellBounds[0]=0; cellBounds[1]=0; cellBounds[2]=0; 
			cellBounds[3]=0; cellBounds[4]=0; cellBounds[5]=0;

			if( this->getIntersectionWithLayerByY( inDataset, pts->GetId(0),YMin, YMax, cellBounds)!=0)
			{
				newPoint[0]= cellBounds[0]+ (cellBounds[1]-cellBounds[0])/2;
				newPoint[1]= cellBounds[2]+ (cellBounds[3]-cellBounds[2])/2;
				newPoint[2]= cellBounds[4]+ (cellBounds[5]-cellBounds[4])/2;

				newSize[0]= cellBounds[1]-cellBounds[0];
				newSize[1]= cellBounds[3]-cellBounds[2];
				newSize[2]= cellBounds[5]-cellBounds[4];

				Points->InsertNextPoint(newPoint);
				Cells->InsertNextCell(1);
				Cells->InsertCellPoint(this->newCellId);

				this->newXINCArray->InsertNextTuple1(newSize[0]);
				this->newYINCArray->InsertNextTuple1(newSize[1]);
				this->newZINCArray->InsertNextTuple1(newSize[2]);
				this->layersArray->InsertNextTuple1(currentLayer);
				this->oldPointId->InsertNextTuple1(pts->GetId(0));
				
				p1=0;
				for(int p=0; p<nbArrays; p++)
				{		
					if( ( strcmp(inDataset->GetPointData()->GetArrayName(p), this->XINC)!=0)
							&&( strcmp(inDataset->GetPointData()->GetArrayName(p), this->YINC)!=0)
							&&( strcmp(inDataset->GetPointData()->GetArrayName(p), this->ZINC)!=0)
						)
					{	
						this->arrays[p1]->InsertNextTuple1(inDataset->GetPointData()->GetArray(p)->GetTuple1(pts->GetId(0)));					
						p1++;
					}
				}

				this->newCellId++;
			}
		}
	}
	this->layersArray->SetName("LayerIDByY");
	this->oldPointId->SetName("oldPointIdByY");
  outDataset->SetPoints(Points);
	outDataset->SetVerts(Cells);
	outDataset->GetPointData()->AddArray(this->newXINCArray);
	outDataset->GetPointData()->AddArray(this->newYINCArray);
	outDataset->GetPointData()->AddArray(this->newZINCArray);
	outDataset->GetPointData()->AddArray(this->layersArray);
	outDataset->GetPointData()->AddArray(this->oldPointId);
	//FREE UP MEMORY
	for(int p=0; p<(nbArrays-3); p++)
	{
		outDataset->GetPointData()->AddArray(this->arrays[p]);
		this->arrays[p]->Delete();
	}

	this->newXINCArray->Delete();
	this->newYINCArray->Delete();
	this->newZINCArray->Delete();
	this->layersArray->Delete();
	this->oldPointId->Delete();
	Points->Delete();
	Cells->Delete();

}


/**********************************************************************************************/
/* Name: shrinkBlockModelByLayers																															*/
/*																																													  */
/* Description:																																								*/
/*																																														*/
/* parameters: input(vtkPolyData*): input of the filter																				*/
/*																																														*/
/* return value: int:																																					*/
/*																																														*/
/**********************************************************************************************/
void vtkBlockSubdivision::shrinkBlockModelByLayersByZ( double sizeOfLayer, 
																											 vtkPolyData* inDataset, 
																											 vtkPolyData* outDataset)
{
	int nbLayers= (this->Allbounds[5]-this->Allbounds[4])/this->DeltaByLayer; //get the number of layers in the block model
	if( (nbLayers*this->DeltaByLayer)<(this->Allbounds[5]-this->Allbounds[4]))
		nbLayers++;
	int numberOfCells= inDataset->GetNumberOfCells();
	int nbArrays= inDataset->GetPointData()->GetNumberOfArrays();

	vtkIdList* pts;
	vtkPoints* Points= vtkPoints::New();
	vtkCellArray* Cells= vtkCellArray::New();
	
	double newPoint[3];
	double newSize[3];
	double cellBounds[6];

	//layer specification
	double ZMin=0, ZMax=0;
	int p1=0;


	for(int currentLayer=0; currentLayer<nbLayers; currentLayer++)
	{
		ZMin= this->Allbounds[4] + currentLayer*this->DeltaByLayer;
		ZMax= this->Allbounds[4] + (currentLayer+1)*this->DeltaByLayer;

		for(int i=0; i<numberOfCells; i++)
		{
			pts= inDataset->GetCell(i)->GetPointIds();			
			cellBounds[0]=0; cellBounds[1]=0; cellBounds[2]=0; 
			cellBounds[3]=0; cellBounds[4]=0; cellBounds[5]=0;

			if( this->getIntersectionWithLayerByZ( inDataset, pts->GetId(0),ZMin, ZMax, cellBounds)!=0)
			{
				newPoint[0]= cellBounds[0]+ (cellBounds[1]-cellBounds[0])/2;
				newPoint[1]= cellBounds[2]+ (cellBounds[3]-cellBounds[2])/2;
				newPoint[2]= cellBounds[4]+ (cellBounds[5]-cellBounds[4])/2;

				newSize[0]= cellBounds[1]-cellBounds[0];
				newSize[1]= cellBounds[3]-cellBounds[2];
				newSize[2]= cellBounds[5]-cellBounds[4];

				Points->InsertNextPoint(newPoint);
				Cells->InsertNextCell(1);
				Cells->InsertCellPoint(this->newCellId);

				this->newXINCArray->InsertNextTuple1(newSize[0]);
				this->newYINCArray->InsertNextTuple1(newSize[1]);
				this->newZINCArray->InsertNextTuple1(newSize[2]);
				this->layersArray->InsertNextTuple1(currentLayer);
				this->oldPointId->InsertNextTuple1(pts->GetId(0));
				
				p1=0;
				for(int p=0; p<nbArrays; p++)
				{		
					if( ( strcmp(inDataset->GetPointData()->GetArrayName(p), this->XINC)!=0)
							&&( strcmp(inDataset->GetPointData()->GetArrayName(p), this->YINC)!=0)
							&&( strcmp(inDataset->GetPointData()->GetArrayName(p), this->ZINC)!=0)
						)
					{	
						this->arrays[p1]->InsertNextTuple1(inDataset->GetPointData()->GetArray(p)->GetTuple1(pts->GetId(0)));					
						p1++;
					}
				}

				this->newCellId++;
			}
		}
	}
  this->layersArray->SetName("LayerIDByZ");
	this->oldPointId->SetName("oldPointIdByZ");
  outDataset->SetPoints(Points);
	outDataset->SetVerts(Cells);
	outDataset->GetPointData()->AddArray(this->newXINCArray);
	outDataset->GetPointData()->AddArray(this->newYINCArray);
	outDataset->GetPointData()->AddArray(this->newZINCArray);
	outDataset->GetPointData()->AddArray(this->layersArray);
	outDataset->GetPointData()->AddArray(this->oldPointId);
	//FREE UP MEMORY
	for(int p=0; p<(nbArrays-3); p++)
	{
		outDataset->GetPointData()->AddArray(this->arrays[p]);
		this->arrays[p]->Delete();
	}

	this->newXINCArray->Delete();
	this->newYINCArray->Delete();
	this->newZINCArray->Delete();
	this->layersArray->Delete();
	this->oldPointId->Delete();
	Points->Delete();
	Cells->Delete();

}


//--------------------------------------------------------------------------
int vtkBlockSubdivision::RequestData( vtkInformation *request, 
                         vtkInformationVector **InputVector, 
                         vtkInformationVector *outputVector )
{
	
   vtkInformation *inputInfo = InputVector[0]->GetInformationObject ( 0 );
   vtkInformation *outInfo = outputVector->GetInformationObject ( 0 );

   vtkPolyData* input = vtkPolyData::SafeDownCast (
                         inputInfo->Get ( vtkDataObject::DATA_OBJECT() ) );
	
   vtkPolyData *output = vtkPolyData::SafeDownCast (
   outInfo->Get ( vtkDataObject::DATA_OBJECT() ) );


	 this->XINCArray= vtkDoubleArray::New();
	 this->XINCArray->DeepCopy(input->GetPointData()->GetArray(this->XINC));
	 this->YINCArray= vtkDoubleArray::New();
	 this->YINCArray->DeepCopy(input->GetPointData()->GetArray(this->YINC));
	 this->ZINCArray= vtkDoubleArray::New();
	 this->ZINCArray->DeepCopy(input->GetPointData()->GetArray(this->ZINC));

	 this->newXINCArray= vtkDoubleArray::New();
	 this->newXINCArray->SetName(this->XINC);
	 this->newYINCArray= vtkDoubleArray::New();
	 this->newYINCArray->SetName(this->YINC);
	 this->newZINCArray= vtkDoubleArray::New();
	 this->newZINCArray->SetName(this->ZINC);
	 this->layersArray= vtkIntArray::New();	
	 this->oldPointId= vtkIdTypeArray::New();
	
	 int nbArrays= input->GetPointData()->GetNumberOfArrays();
	 if(nbArrays>3)
	 {
		  this->arrays= new vtkDataArray*[nbArrays-3]; 		
			int p1=0;
			for(int p=0; p< nbArrays; p++)
			{		
				if( ( strcmp(input->GetPointData()->GetArrayName(p), this->XINC)!=0)
						&&( strcmp(input->GetPointData()->GetArrayName(p), this->YINC)!=0)
						&&( strcmp(input->GetPointData()->GetArrayName(p), this->ZINC)!=0)
					)
				{	
					this->arrays[p1]= vtkDoubleArray::New();
					this->arrays[p1]->SetName(input->GetPointData()->GetArrayName(p));
					p1++;
				}
			}
	 }
	
	 Allbounds[0]=this->boundMinX; Allbounds[1]=this->boundMaxX;
	 Allbounds[2]=this->boundMinY; Allbounds[3]=this->boundMaxY;
	 Allbounds[4]=this->boundMinZ; Allbounds[5]=this->boundMaxZ;
	 
	 //shrink the block model by layers
	 if(this->Direction==0)
		 this->shrinkBlockModelByLayersByX(this->DeltaByLayer, input, output); 
	 else if (this->Direction==1)
		this->shrinkBlockModelByLayersByY(this->DeltaByLayer, input, output); 
	 else 
		 this->shrinkBlockModelByLayersByZ(this->DeltaByLayer, input, output); 

	 return 1;
}
