// .NAME DataMineBlockMpdelReader.cxx
// Read DataMine binary files for single objects.
// point, perimeter (polyline), wframe<points/triangle>
// With or without properties (each property name < 8 characters)
// The binary file reading is done by 'dmfile.cxx'
//     99-04-12: Written by Jeremy Maccelari, visualn@iafrica.com

#include "vtkPDataMinePerimeterReader.h"
#include "PropertyStorage.h"
#include "dmfile.h"

#include "vtkObjectFactory.h"
#include "vtkCellArray.h"
#include "vtkPointData.h"
#include "vtkCellData.h"

vtkCxxRevisionMacro(vtkPDataMinePerimeterReader, "$Revision: 0.4 $");
vtkStandardNewMacro(vtkPDataMinePerimeterReader);


// --------------------------------------
vtkPDataMinePerimeterReader::vtkPDataMinePerimeterReader()
  {
  }

// --------------------------------------
vtkPDataMinePerimeterReader::~vtkPDataMinePerimeterReader()
  {  
  }

// --------------------------------------
int vtkPDataMinePerimeterReader::CanReadFile( const char* fname )
{
	return this->CanRead(fname, perimeter);	
}


// --------------------------------------
int vtkPDataMinePerimeterReader::ReadShrunks( int start, int end, vtkPolyData* out)
{
	vtkAppendPolyData *append = vtkAppendPolyData::New();
	vtkDataMinePerimeterReader* reader;
	//read all of pieces assigned to this process
	for (int piec = start; piec < end; ++piec)
	{
		vtkPolyData *tmp;
		reader = vtkDataMinePerimeterReader::New();
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
