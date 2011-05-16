// .NAME VulcanBlockModelReader.cxx
// Read Vulcan Resource files.
#include "vtkVulcanBlockModelReader.h"
#include "blockmodel_api.h"

#include "vtkIntArray.h"
#include "vtkPointData.h"
#include "vtkCellArray.h"
#include "vtkCleanUnstructuredGrid.h"
#include "vtkObjectFactory.h"
#include "vtkInformationVector.h"
#include "vtkInformation.h"
#include "vtkPoints.h"
#include "vtkPointData.h"
#include "vtkUnstructuredGrid.h"
#include "vtkDoubleArray.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include <string>
#include <vector>


vtkCxxRevisionMacro(vtkVulcanBlockModelReader, "$Revision: 2.0 $");
vtkStandardNewMacro(vtkVulcanBlockModelReader);

// Constructor
vtkVulcanBlockModelReader::vtkVulcanBlockModelReader()
{
  this->FileName = NULL;
  this->SetNumberOfInputPorts(0);
};

// --------------------------------------
// Destructor
vtkVulcanBlockModelReader::~vtkVulcanBlockModelReader()
{
  this->SetFileName(0);
}

// --------------------------------------  
int vtkVulcanBlockModelReader::CanReadFile( const char* fname )
{
	return 1;
}

// --------------------------------------
int vtkVulcanBlockModelReader::RequestData(vtkInformation* request,
                          vtkInformationVector** inputVector,
                          vtkInformationVector* outputVector)
{
	//time to read the point file first, and store it in a vtkPoints object		
	vtkInformation *outInfo = outputVector->GetInformationObject(0);  
  vtkUnstructuredGrid *output = vtkUnstructuredGrid::SafeDownCast( outInfo->Get(vtkDataObject::DATA_OBJECT()));
	  		
	mtk::exported_blockmodel::MTK_BlockModel VulcanReader;			
	if ( VulcanReader.Open( this->FileName ) )
		{
		vtkUnstructuredGrid *temp = vtkUnstructuredGrid::New();
		vtkPoints *points = vtkPoints::New();

		//we currently do not support vulcan files properties, as that seems to break the dlls currently
		//once we actually have users that need to read vulcan files we will become worried about this.
		//rmaynard - March 13 2009 ( IT'S FRIDAY )

		
		int index = 0;
		int size = VulcanReader.NBlocks();		
		VulcanReader.FirstBlock();    
		while(!VulcanReader.IsEof() && index < size) 
			{																		
			double bounds[6];
			VulcanReader.GetBlockExtent(&bounds[0],&bounds[1],&bounds[2],&bounds[3],&bounds[4],&bounds[5]);
			vtkIdType pts[8];
			pts[0] = points->InsertNextPoint(bounds[0],bounds[1],bounds[2]);
			pts[1] = points->InsertNextPoint(bounds[3],bounds[1],bounds[2]);
			
			pts[2] = points->InsertNextPoint(bounds[3],bounds[4],bounds[2]);
			pts[3] = points->InsertNextPoint(bounds[0],bounds[4],bounds[2]);
			
			pts[4] = points->InsertNextPoint(bounds[0],bounds[1],bounds[5]);
			pts[5] = points->InsertNextPoint(bounds[3],bounds[1],bounds[5]);
			
			pts[6] = points->InsertNextPoint(bounds[3],bounds[4],bounds[5]);
			pts[7] = points->InsertNextPoint(bounds[0],bounds[4],bounds[5]);
			
			temp->InsertNextCell(VTK_HEXAHEDRON,8,pts);
			index++;
			VulcanReader.Next();  
			}		
		temp->SetPoints(points);	
		points->Delete();	  
		
		//clean first 
		vtkCleanUnstructuredGrid *clean  = vtkCleanUnstructuredGrid::New();
		clean->SetInput( temp );
		clean->Update();
		temp->Delete();


		//copy over temp to output
		output->ShallowCopy( clean->GetOutput() );
		clean->Delete();
		}
	

	VulcanReader.Close();
  return 1;
}



// --------------------------------------
void vtkVulcanBlockModelReader::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "FileName: "
     << (this->FileName ? this->FileName : "(none)") << "\n";
}
