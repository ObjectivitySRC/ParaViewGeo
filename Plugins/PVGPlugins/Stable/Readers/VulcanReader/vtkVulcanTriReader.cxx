// .NAME VulcanTriReader.cxx
// Read Vulcan Resource files.
#include "vtkVulcanTriReader.h"
#include "tri_api.h"

#include "vtkCellArray.h"
#include "vtkObjectFactory.h"
#include "vtkInformationVector.h"
#include "vtkInformation.h"
#include "vtkPoints.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkDoubleArray.h"

vtkCxxRevisionMacro(vtkVulcanTriReader, "$Revision: 2.0 $");
vtkStandardNewMacro(vtkVulcanTriReader);

// Constructor
vtkVulcanTriReader::vtkVulcanTriReader()
{
  this->FileName = NULL;
  this->SetNumberOfInputPorts(0);
};

// --------------------------------------
// Destructor
vtkVulcanTriReader::~vtkVulcanTriReader()
{
  this->SetFileName(0);
}

// --------------------------------------  
int vtkVulcanTriReader::CanReadFile( const char* fname )
{
	return 1;
}

// --------------------------------------
int vtkVulcanTriReader::RequestData(vtkInformation* request,
                          vtkInformationVector** inputVector,
                          vtkInformationVector* outputVector)
{
	//time to read the point file first, and store it in a vtkPoints object		
	vtkInformation *outInfo = outputVector->GetInformationObject(0);  
  vtkPolyData *output = vtkPolyData::SafeDownCast( outInfo->Get(vtkDataObject::DATA_OBJECT()));
  
	
  vtkPoints *points = vtkPoints::New();
  vtkCellArray *cells = vtkCellArray::New();
	  
  //open
	mtk::exported_tri::MTK_Triangulation VulcanReader;
	if ( VulcanReader.Open(this->FileName) )
		{
		double x,y,z;
		int p0,p1,p2;
		int size = VulcanReader.NumPoints();
		for ( int i=0; i < size; i++)
			{	
			VulcanReader.GetPoint(i,&x,&y,&z);
			points->InsertPoint(i,x,y,z);
			}

		//now we read in the triangles
		size = VulcanReader.NumTriangles();
		vtkIdType pointIds[3];
		for ( int i=0; i < size; i++)
			{	
			VulcanReader.GetTriangle(i,&p0,&p1,&p2);
			pointIds[0] = p0;
      pointIds[1] = p1;
      pointIds[2] = p2;
			cells->InsertNextCell( 3, pointIds );			
			}
		}
      

  output->SetPoints(points);
  output->SetPolys(cells);

	points->Delete();
	cells->Delete();
  
  return 1;
}

// --------------------------------------
void vtkVulcanTriReader::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "FileName: "
     << (this->FileName ? this->FileName : "(none)") << "\n";
}
