
#include "vtkPDataMinePointReader.h"
#include "PropertyStorage.h"
#include "dmfile.h"

#include "vtkObjectFactory.h"
#include "vtkCellArray.h"

vtkCxxRevisionMacro(vtkPDataMinePointReader, "$Revision: 0.4 $");
vtkStandardNewMacro(vtkPDataMinePointReader);


// --------------------------------------
vtkPDataMinePointReader::vtkPDataMinePointReader()
  {
  }

// --------------------------------------
vtkPDataMinePointReader::~vtkPDataMinePointReader()
  {  
  }

// --------------------------------------
int vtkPDataMinePointReader::CanReadFile( const char* fname )
{
	return this->CanRead(fname, point);	
}


// --------------------------------------
int vtkPDataMinePointReader::ReadShrunks( int start, int end, vtkPolyData* out)
{
	vtkAppendPolyData *append = vtkAppendPolyData::New();
	vtkDataMinePointReader* reader;
	for (int piec = start; piec < end; ++piec)
	{
		vtkPolyData *tmp;
		reader = vtkDataMinePointReader::New();
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
