#include "vtkRegexThreshold.h"

#include "vtkCell.h"
#include "vtkCellData.h"
#include "vtkIdList.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkUnstructuredGrid.h"
#include "vtkStringArray.h"
#include <vtksys/RegularExpression.hxx>


vtkCxxRevisionMacro(vtkRegexThreshold, "$Revision: 2.10 $");
vtkStandardNewMacro(vtkRegexThreshold);

// Construct with lower threshold=0, upper threshold=1, and threshold
// function=upper AllScalars=1.
vtkRegexThreshold::vtkRegexThreshold()
{
  this->Regex                  = 0;
}

vtkRegexThreshold::~vtkRegexThreshold()
{
  delete[] this->Regex;
}


int vtkRegexThreshold::RequestData(vtkInformation *vtkNotUsed(request), vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{
// get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  // get the input and ouptut
  vtkDataSet *input = vtkDataSet::SafeDownCast(
    inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkUnstructuredGrid *output = vtkUnstructuredGrid::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

  vtkIdType cellId, newCellId;
  vtkIdList *cellPts, *pointMap;
  vtkIdList *newCellPts;
  vtkCell *cell;
  vtkPoints *newPoints;
  int i, ptId, newId, numPts;
  int numCellPts;
  double x[3];
  vtkPointData *pd=input->GetPointData(), *outPD=output->GetPointData();
  vtkCellData *cd=input->GetCellData(), *outCD=output->GetCellData();
  int keepCell, usePointScalars;

  vtkDebugMacro(<< "Executing threshold filter");
  
  if (this->AttributeMode != -1)
    {
    vtkErrorMacro(<<"You have set the attribute mode on vtkThreshold. This method is deprecated, please use SetInputArrayToProcess instead.");
    return 1;
    }


	int inputType = VTK_INT;
  vtkAbstractArray *inScalars = (vtkDataArray*)this->GetInputArrayToProcess(0,inputVector);
  
  //try string next
  if (!inScalars)
		{
		inputType = VTK_STRING;
		inScalars = (vtkStringArray*)this->GetInputAbstractArrayToProcess(0,inputVector);
		}
  
  if (!inScalars)
    {
    vtkDebugMacro(<<"No scalar data to threshold");
    return 1;
    }

  outPD->CopyGlobalIdsOn();
  outPD->CopyAllocate(pd);
  outCD->CopyGlobalIdsOn();
  outCD->CopyAllocate(cd);

  numPts = input->GetNumberOfPoints();
  output->Allocate(input->GetNumberOfCells());

  newPoints = vtkPoints::New();
  newPoints->SetDataType( this->PointsDataType );
  newPoints->Allocate(numPts);

  pointMap = vtkIdList::New(); //maps old point ids into new
  pointMap->SetNumberOfIds(numPts);
  for (i=0; i < numPts; i++)
    {
    pointMap->SetId(i,-1);
    }

  newCellPts = vtkIdList::New();     

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
      if (this->AllScalars)
        {
        keepCell = 1;
        for ( i=0; keepCell && (i < numCellPts); i++)
          {
          ptId = cellPts->GetId(i);
          if ( inputType == VTK_INT )
						{
						keepCell = this->EvaluateComponents( (vtkDataArray*)inScalars, ptId );
						}
					else
						{
						keepCell = this->EvaluateComponents( (vtkStringArray*)inScalars, ptId );
						}
          }
        }
      else
        {
        keepCell = 0;
        for ( i=0; (!keepCell) && (i < numCellPts); i++)
          {
          ptId = cellPts->GetId(i);
          if ( inputType == VTK_INT )
						{
						keepCell = this->EvaluateComponents( (vtkDataArray*)inScalars, ptId );
						}
					else
						{
						keepCell = this->EvaluateComponents( (vtkStringArray*)inScalars, ptId );
						}
          }
        }
      }
    else //use cell scalars
      {
      if ( inputType == VTK_INT )
				{
				keepCell = this->EvaluateComponents( (vtkDataArray*)inScalars, cellId );
				}
			else
				{
				keepCell = this->EvaluateComponents( (vtkStringArray*)inScalars, cellId );
				}      
      }
    
    if (  numCellPts > 0 && keepCell )
      {
      // satisfied thresholding (also non-empty cell, i.e. not VTK_EMPTY_CELL)
      for (i=0; i < numCellPts; i++)
        {
        ptId = cellPts->GetId(i);
        if ( (newId = pointMap->GetId(ptId)) < 0 )
          {
          input->GetPoint(ptId, x);
          newId = newPoints->InsertNextPoint(x);
          pointMap->SetId(ptId,newId);
          outPD->CopyData(pd,ptId,newId);
          }
        newCellPts->InsertId(i,newId);
        }
      newCellId = output->InsertNextCell(cell->GetCellType(),newCellPts);
      outCD->CopyData(cd,cellId,newCellId);
      newCellPts->Reset();
      } // satisfied thresholding
    } // for all cells

  vtkDebugMacro(<< "Extracted " << output->GetNumberOfCells() 
                << " number of cells.");

  // now clean up / update ourselves
  pointMap->Delete();
  newCellPts->Delete();
  
  output->SetPoints(newPoints);
  newPoints->Delete();

  output->Squeeze();

  return 1;

}

int vtkRegexThreshold::EvaluateComponents( vtkDataArray *scalars, vtkIdType id )
{
  int keepCell = 0;
  int numComp = scalars->GetNumberOfComponents();
  int c;
  vtkStdString tmp;

  switch ( this->ComponentMode )
    {
    case VTK_COMPONENT_MODE_USE_SELECTED:
      c = (this->SelectedComponent < numComp)?(this->SelectedComponent):(0);
      tmp = vtkRegexConvert( scalars->GetComponent(id,c)  );
      keepCell = this->Search( tmp );
      break;
    case VTK_COMPONENT_MODE_USE_ANY:
      keepCell = 0;
      for ( c = 0; (!keepCell) && (c < numComp); c++ )
        {
        tmp = vtkRegexConvert( scalars->GetComponent(id,c) );
        keepCell = this->Search( tmp );
        }
      break;
    case VTK_COMPONENT_MODE_USE_ALL:
      keepCell = 1;
      for ( c = 0; keepCell && (c < numComp); c++ )
        {
        tmp = vtkRegexConvert( scalars->GetComponent(id,c) );
        keepCell = this->Search( tmp );
        }
      break;
    }
  return keepCell;
}

int vtkRegexThreshold::EvaluateComponents( vtkStringArray *scalars, vtkIdType id )
{  
  vtkStdString tmp;  
  tmp = vtkRegexConvert( scalars->GetValue( id ) );
  return this->Search( tmp );  
}

bool vtkRegexThreshold::Search(vtkStdString& text)
{
  vtksys::RegularExpression regExp = this->Regex;
  return regExp.find(text);
}


void vtkRegexThreshold::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

}
