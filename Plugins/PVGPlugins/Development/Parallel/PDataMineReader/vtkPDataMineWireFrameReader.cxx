
#include "vtkPDataMineWireFrameReader.h"
#include "PropertyStorage.h"
#include "dmfile.h"

#include "vtkObjectFactory.h"
#include "vtkCellArray.h"
#include "vtkPointData.h"
#include "vtkCellData.h"

vtkCxxRevisionMacro(vtkPDataMineWireFrameReader, "$Revision: 0.4 $");
vtkStandardNewMacro(vtkPDataMineWireFrameReader);


// --------------------------------------
vtkPDataMineWireFrameReader::vtkPDataMineWireFrameReader()
  {
		this->PointFileName = NULL;
		this->TopoFileName = NULL;
		this->StopeSummaryFileName = NULL;
		this->UseStopeSummary = false;
  }

// --------------------------------------
vtkPDataMineWireFrameReader::~vtkPDataMineWireFrameReader()
  {  
  }

// --------------------------------------
int vtkPDataMineWireFrameReader::CanReadFile( const char* fname )
{
	return ( this->CanRead( fname, wframepoints )
		|| this->CanRead( fname, wframetriangle )
		|| this->CanRead( fname, stopesummary ) );
}


// --------------------------------------
int vtkPDataMineWireFrameReader::ReadShrunks( int start, int end, vtkPolyData* out)
{
	vtkAppendPolyData *append = vtkAppendPolyData::New();
	vtkDataMineWireFrameReader* reader;
	//read all of pieces assigned to this process
	for (int piec = start; piec < end; ++piec)
	{
		vtkPolyData *tmp;
		reader = vtkDataMineWireFrameReader::New();
		reader->SetFileName(this->GetPieceFileName(piec));
		reader->SetPointFileName(this->PointFileName);
		reader->SetTopoFileName(this->TopoFileName);
		reader->SetStopeSummaryFileName(this->StopeSummaryFileName);
		reader->SetUseStopeSummary(this->UseStopeSummary);
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
