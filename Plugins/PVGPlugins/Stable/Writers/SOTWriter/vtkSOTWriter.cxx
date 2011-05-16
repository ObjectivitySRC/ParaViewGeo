/*=================================================
==  vtkSOTWriter.cxx
==  By: Patrick Ryan & Julie Zuo
==  Date: May 21, 2009
==  Description: Writes SOT Schedule IDs of selected
==    pointsto a .sqf (SOT Queue File) file. To be 
==    used in conjunction with vtkSOTResult.
================================================*/
#include "vtkSOTWriter.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkStringArray.h"
#include "vtkInformation.h"
#include <sstream>

vtkCxxRevisionMacro(vtkSOTWriter, "$Revision: 1.27 $");
vtkStandardNewMacro(vtkSOTWriter);

//=================================================
vtkSOTWriter::vtkSOTWriter()
{

}

//=================================================
vtkSOTWriter::~vtkSOTWriter()
{
  
}

//=================================================
int vtkSOTWriter::FillInputPortInformation(
  int vtkNotUsed(port), vtkInformation* info)
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkUnstructuredGrid");
  return 1;
}

//=================================================
void vtkSOTWriter::WriteData()
{
	// Cast data to Unstructured Grid
  vtkUnstructuredGrid *input = vtkUnstructuredGrid::SafeDownCast(this->GetInput());
  
  // Get all point data from array named "ScheduleID"
  vtkAbstractArray *Ids = input->GetPointData()->GetAbstractArray("ScheduleID");
  if (Ids == NULL )
    {
    vtkErrorMacro(<<"PointData arrays are NULL");
    return;
    }

  // Obtain string array of point properties
  vtkStringArray* stringProperties = vtkStringArray::SafeDownCast( Ids );
  if (stringProperties == NULL)
    {
    vtkErrorMacro(<<"Property was not a String");
    return;
    }

  // Open file for writing
  ofstream outFile;
  outFile.open(this->FileName);
	if (outFile == NULL)
		{
		vtkErrorMacro(<<"Unable to open output file for writing");
		return;
		}

  // Begin formatting and writing IDs to file
  for (int i = 0; i < stringProperties->GetSize(); i++)
    {
    // Remove "ID" prefix if it exists
    // IDbb1fe500-42eb-42cd-8360-fb89d1331fef 
    // becomes bb1fe500-42eb-42cd-8360-fb89d1331fef
    if (stringProperties->GetValue(0).find("ID") != -1)
      {
      outFile << stringProperties->GetValue(i).substr(2) << "\n";
      }
    else
      {
      outFile << stringProperties->GetValue(i) << "\n";
      }
    }
  outFile.close();
}

//=================================================