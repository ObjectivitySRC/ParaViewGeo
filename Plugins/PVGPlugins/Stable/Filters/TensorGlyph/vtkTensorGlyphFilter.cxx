
#include "vtkTensorGlyphFilter.h"
#include "vtkCell.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkPointData.h"
#include "vtkCellData.h"

#include "vtkPVGlyphFilter.h"
#include <math.h>

vtkCxxRevisionMacro ( vtkTensorGlyphFilter, "$Revision: 1.1 $" );
vtkStandardNewMacro ( vtkTensorGlyphFilter );


//-----------------------------------------------------------------------
vtkTensorGlyphFilter::vtkTensorGlyphFilter()
{
}

//-----------------------------------------------------------------------
vtkTensorGlyphFilter::~vtkTensorGlyphFilter()
{
}


  
//-----------------------------------------------------------------------
int vtkTensorGlyphFilter::RequestData( vtkInformation *vtkNotUsed ( request ), 
                                         vtkInformationVector **inputVector, 
                                         vtkInformationVector *outputVector )
{
  // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);

  // get the input and ouptut
  vtkDataSet* input = vtkDataSet::SafeDownCast(
    inInfo->Get(vtkDataObject::DATA_OBJECT()));

	vtkTensorGlyph::RequestData(0, inputVector, outputVector);

	vtkInformation *outInfo = outputVector->GetInformationObject(0);
  vtkDataSet* output = vtkDataSet::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));


	return 1;
}
