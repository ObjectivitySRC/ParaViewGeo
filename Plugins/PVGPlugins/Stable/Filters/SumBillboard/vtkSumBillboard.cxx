/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $ vtkDistanceToGrid.cxx $
  Author:    Robert Maynard
  MIRARCO, Laurentian University
  Date:      June 1, 2008
  Version:   0.8



  =========================================================================*/
#include "vtkSumBillboard.h"
#include "vtkCell.h"
#include "vtkCellData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkSmartPointer.h"
#include "vtkDoubleArray.h"
#include "vtkCellData.h"
#include "vtkPointData.h"
#include "vtkStringArray.h"
#include "vtkTable.h"
#include "vtkDataObject.h"
#include "vtkDataSet.h"

#include <vtkstd/string>
#include <sstream>

vtkStandardNewMacro(vtkSumBillboard);
vtkCxxRevisionMacro(vtkSumBillboard, "$Revision: 1.1 $");
//----------------------------------------------------------------------------
vtkSumBillboard::vtkSumBillboard()
{
  this->Format = 0;
  this->SetFormat("Total Amount of this property is ");        
  this->SelectedComponent      = 0;

  // by default process active point scalars
  this->SetInputArrayToProcess(0,0,0,vtkDataObject::FIELD_ASSOCIATION_POINTS_THEN_CELLS,vtkDataSetAttributes::SCALARS);
}

//----------------------------------------------------------------------------
vtkSumBillboard::~vtkSumBillboard()
{
  this->SetFormat(0);
}

//----------------------------------------------------------------------------
int vtkSumBillboard::FillInputPortInformation(int vtkNotUsed(port), vtkInformation* info)
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataSet");  
  return 1;
}


int vtkSumBillboard::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  vtkTable* output = vtkTable::GetData(outputVector);
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkDataSet *input = vtkDataSet::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));

  vtkIdType cellId;
  vtkIdList *cellPts;  
  vtkCell *cell;  
  int i, ptId, numPts;
  int numCellPts;
    
  int usePointScalars;

  vtkDebugMacro(<< "Executing threshold filter");

  vtkDataArray *inScalars = this->GetInputArrayToProcess(0,inputVector);
  
  if (!inScalars)
    {
    vtkDebugMacro(<<"No scalar data to threshold");
    return 1;
    }
  
  numPts = input->GetNumberOfPoints();      
  
  //keep track of the amout we have found 
  double sum = 0;
  
  // are we using pointScalars?
  usePointScalars = (inScalars->GetNumberOfTuples() == numPts);
  
  // Check that the scalars of each cell satisfy the threshold criterion
  for (cellId=0; cellId < input->GetNumberOfCells(); cellId++)
    {
    cell = input->GetCell(cellId);
    cellPts = cell->GetPointIds();
    numCellPts = cell->GetNumberOfPoints();
    
    if ( usePointScalars )
      {      
      for ( i=0; i < numCellPts; i++)
        {
        ptId = cellPts->GetId(i);                   
        sum+= inScalars->GetComponent(ptId, this->SelectedComponent);          
        }              
      }
    else //use cell scalars
      {                   
      sum+= inScalars->GetComponent(cellId, this->SelectedComponent);       
      }
    }
            
  vtkstd::ostringstream buffer;      
  buffer << this->Format << sum << endl;
  
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
void vtkSumBillboard::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "Format: " << (this->Format? this->Format : "(none)") << endl;
}


