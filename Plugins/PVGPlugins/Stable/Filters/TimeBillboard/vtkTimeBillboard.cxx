/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $ vtkDistanceToGrid.cxx $
  Author:    Robert Maynard
  MIRARCO, Laurentian University
  Date:      June 1, 2008
  Version:   0.8



  =========================================================================*/
#include "vtkTimeBillboard.h"
#include "TimeConverter.h"

#include "vtkCell.h"
#include "vtkCellData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkSmartPointer.h"
#include "vtkCellData.h"
#include "vtkPointData.h"
#include "vtkStringArray.h"
#include "vtkTable.h"
#include "vtkDataObject.h"
#include "vtkDataSet.h"

#include <vtkstd/string>
#include <sstream>

vtkStandardNewMacro(vtkTimeBillboard);
vtkCxxRevisionMacro(vtkTimeBillboard, "$Revision: 1.1 $");
//----------------------------------------------------------------------------
vtkTimeBillboard::vtkTimeBillboard()
{
  this->Format = 0;
  this->Text = 0;
  this->SetFormat("The Start and End Time's are: ");        
  this->SelectedComponent      = 0;

  // by default process active point scalars
  this->SetInputArrayToProcess(0,0,0,vtkDataObject::FIELD_ASSOCIATION_POINTS_THEN_CELLS,vtkDataSetAttributes::SCALARS);
}

//----------------------------------------------------------------------------
vtkTimeBillboard::~vtkTimeBillboard()
{
  this->SetFormat(0);
}

//----------------------------------------------------------------------------
int vtkTimeBillboard::FillInputPortInformation(int vtkNotUsed(port), vtkInformation* info)
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataSet");  
  return 1;
}


int vtkTimeBillboard::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  vtkTable* output = vtkTable::GetData(outputVector);
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkDataSet *input = vtkDataSet::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));

  vtkDebugMacro(<< "Executing time billboard filter");

  vtkDataArray *inScalars = this->GetInputArrayToProcess(0,inputVector);
  
  if (!inScalars)
    {
    vtkDebugMacro(<<"No scalar data to threshold");
    return 1;
    }
 
  
  double range[2];
  char* cRange[2];
  inScalars->GetRange(range,this->SelectedComponent);          
  
  //convert to char*
  TimeConverter timeC = TimeConverter(this->Format);  
  //we use 2, since we have a min and a max
  for (int i=0; i < 2; i++)
    {
    timeC.Parse(range[i], this->TimeFormat);        
    cRange[i]=timeC.GetISODate();
    cout << timeC.GetISODate()<<endl;
    }
    
  vtkstd::ostringstream buffer;      
  buffer << this->Text << "\n" << cRange[0] << "\n" << cRange[1] << endl;
  //use new line characters, to make a multiple line display
  //if the \n is in the text string, it will not actually create a new line
  
  vtkStringArray * data = vtkStringArray::New();  
  data->SetName("Text");
  data->SetNumberOfComponents(1);
  data->InsertNextValue(buffer.str().c_str()); 
  output->AddColumn(data);  
  
  //cleanup
  data->Delete();    
  return 1;
}


//----------------------------------------------------------------------------
void vtkTimeBillboard::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "Format: " << (this->Format? this->Format : "(none)") << endl;
}


