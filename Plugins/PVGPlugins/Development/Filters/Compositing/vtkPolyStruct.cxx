#include "vtkPolyStruct.h"

#include "vtkPointData.h"
#include "vtkSmartPointer.h"
#include "vtkStreamingDemandDrivenPipeline.h"


#include "vtkIntArray.h"

#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPoints.h"
#include "vtkCellData.h"
#include "vtkDataSet.h"
#include "vtkPolyDataAlgorithm.h"
#include "vtkImageData.h"
#include "vtkStructuredPoints.h"
#include <math.h>

vtkCxxRevisionMacro(vtkPolyStruct, "$Revision: 1.6 $");
vtkStandardNewMacro(vtkPolyStruct);

//----------------------------------------------------------------------------
vtkPolyStruct::vtkPolyStruct()
{      
  const int DEFAULT = 10;
  this->SetNumberOfInputPorts ( 1 );
  // Sensible initial values
  this->XCubes = DEFAULT;
  this->YCubes = DEFAULT;
  this->ZCubes = DEFAULT;  
}

//----------------------------------------------------------------------------
int vtkPolyStruct::FillInputPortInformation ( int vtkNotUsed(port), vtkInformation* info )
  {
  info->Set ( vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData" );
  return 1;
  }
  
//----------------------------------------------------------------------------
int vtkPolyStruct::FillOutputPortInformation(int vtkNotUsed(port), vtkInformation* info)
{
  info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkStructuredPoints"/*"vtkImageData"*/);
  return 1;
}


//----------------------------------------------------------------------------
int vtkPolyStruct::RequestInformation(vtkInformation *vtkNotUsed(request),vtkInformationVector **inputVector,vtkInformationVector *outputVector)   
{
  vtkInformation *outInfo = outputVector->GetInformationObject(0);
  //set the extents
  int whole[6];
  whole[0] = whole[2] = whole[4]=0;
  whole[1] = this->XCubes;
  whole[3] = this->YCubes;
  whole[5] = this->ZCubes;  
  outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(),whole,6);
  return 1;
}
//----------------------------------------------------------------------------
int vtkPolyStruct::RequestData(vtkInformation *vtkNotUsed(request),vtkInformationVector **inputVector,vtkInformationVector *outputVector)   
{
  const int XMIN = 0;
  const int XMAX = 1;
  const int YMIN = 2;
  const int YMAX = 3;
  const int ZMIN = 4;
  const int ZMAX = 5;
    
  // get the info object
  vtkInformation *outInfo = outputVector->GetInformationObject(0);       
  //get the output
 vtkImageData  *output = vtkImageData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));
  
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
    
  vtkDataSet *input = vtkDataSet::SafeDownCast (inInfo->Get ( vtkDataObject::DATA_OBJECT() ) );
  
  double origin[3];
  double spacing[3];
  
  //get origin
  double *bounds = input->GetBounds();
  origin[0]=bounds[XMIN];
  origin[1]=bounds[YMIN];
  origin[2]=bounds[ZMIN];
      
  //set the spacing
  spacing[0] = fabs ( (bounds[XMAX]-bounds[XMIN])/(this->XCubes) );
  spacing[1] = fabs ( (bounds[YMAX]-bounds[YMIN])/(this->YCubes) );
  spacing[2] = fabs ( (bounds[ZMAX]-bounds[ZMIN])/(this->ZCubes) );
  
  output->SetDimensions(this->XCubes+1,this->YCubes+1,this->ZCubes+1);
  output->SetOrigin(origin);
  output->SetSpacing(spacing);
  
  int *ext = outInfo->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT());
  output->SetWholeExtent(ext);
  output->SetExtent(ext);
  output->SetUpdateExtent(ext);
  output->AllocateScalars();
  
  return 1;
}

//----------------------------------------------------------------------------
void vtkPolyStruct::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

}
