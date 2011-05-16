
#include "vtkPDataMineDrillHoleReader.h"
#include "PropertyStorage.h"
#include "dmfile.h"

#include "vtkObjectFactory.h"
#include "vtkCellArray.h"
#include "vtkPointData.h"
#include "vtkCellData.h"

vtkCxxRevisionMacro(vtkPDataMineDrillHoleReader, "$Revision: 0.4 $");
vtkStandardNewMacro(vtkPDataMineDrillHoleReader);


// --------------------------------------
vtkPDataMineDrillHoleReader::vtkPDataMineDrillHoleReader()
  {
  }

// --------------------------------------
vtkPDataMineDrillHoleReader::~vtkPDataMineDrillHoleReader()
  {  
  }

// --------------------------------------
int vtkPDataMineDrillHoleReader::CanReadFile( const char* fname )
{
	return this->CanRead( fname, drillhole );	
}


// --------------------------------------
int vtkPDataMineDrillHoleReader::ReadShrunks( int start, int end, vtkPolyData* out)
{
	vtkAppendPolyData *append = vtkAppendPolyData::New();
	vtkDataMineDrillHoleReader* reader;
	//read all of pieces assigned to this process
	for (int piec = start; piec < end; ++piec)
	{
		vtkPolyData *tmp;
		reader = vtkDataMineDrillHoleReader::New();
		reader->SetFileName(this->GetPieceFileName(piec));
		reader->SetCellDataArraySelection(this->CellDataArraySelection);
		tmp = reader->GetOutput();
		if (tmp && tmp->GetDataObjectType() != VTK_POLY_DATA)
		{
			vtkWarningMacro("Expecting PolyData in file: " << this->PieceFileNames[piec]);
		}
		else	append->AddInput(tmp);
	
		reader->Delete();
	}
	append->Update();
	out->CopyStructure(append->GetOutput());
	out->GetFieldData()->PassData(append->GetOutput()->GetFieldData());
	out->GetCellData()->PassData(append->GetOutput()->GetCellData());
	out->GetPointData()->PassData(append->GetOutput()->GetPointData());
	append->Delete();

	return 1;
}
