// .NAME VulcanGridReader.cxx
// Read Vulcan Resource files.
#include "vtkVulcanGridReader.h"
#include "grid_api.h"

#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkObjectFactory.h"
#include "vtkInformationVector.h"
#include "vtkInformation.h"
#include "vtkPoints.h"
#include "vtkPointData.h"
#include "vtkImageData.h"
#include "vtkDoubleArray.h"

vtkCxxRevisionMacro(vtkVulcanGridReader, "$Revision: 2.0 $");
vtkStandardNewMacro(vtkVulcanGridReader);

// Constructor
vtkVulcanGridReader::vtkVulcanGridReader()
{
  this->FileName = NULL;
  this->SetNumberOfInputPorts(0);
};

// --------------------------------------
// Destructor
vtkVulcanGridReader::~vtkVulcanGridReader()
{
  this->SetFileName(0);
}

// --------------------------------------  
int vtkVulcanGridReader::CanReadFile( const char* fname )
{
	return 1;
}

// --------------------------------------
int vtkVulcanGridReader::RequestData(vtkInformation* request,
                          vtkInformationVector** inputVector,
                          vtkInformationVector* outputVector)
{
	//time to read the point file first, and store it in a vtkPoints object
	vtkInformation *outInfo = outputVector->GetInformationObject(0);  
  vtkImageData *output = vtkImageData::SafeDownCast( outInfo->Get(vtkDataObject::DATA_OBJECT()));
    
	vtkDoubleArray *values = vtkDoubleArray::New();
	values->SetName("values");

  //open
	mtk::exported_grid::MTK_Grid grid;
	if ( grid.Open(this->FileName) )
		{
		double location[4] ={0,1,0,1};
		int extents[6]={0,1,0,1,0,1};
		double spacing[3]={1,1,1};
		int dimensions[3]={1,1,1};

	
		//get the data from the grid reader
		grid.GetExtent(&location[0],&location[2],&location[1],&location[3]);
		grid.GetSpacing(&spacing[0],&spacing[1]);
		grid.GetNodes(&dimensions[0],&dimensions[1]);

		//from the dimensions, figure out the extents
		extents[1] = dimensions[0] - 1;
		extents[3] = dimensions[1] - 1;
		extents[5] = dimensions[2] - 1;
		

		//push that data to the output
		output->SetOrigin( location[0], location[2], 0 );
		output->SetSpacing( spacing);
		output->SetDimensions( dimensions );		

		output->SetExtent( extents );
		output->SetWholeExtent( extents );
		output->SetUpdateExtent( extents );
		output->AllocateScalars();

		
		double val;
		bool mask;
		
		for (int j=0; j<dimensions[1]; j++)
			{
      for (int i=0; i<dimensions[0]; i++)
				{
        grid.GetNodeValue(i,j,&val,&mask);			
				values->InsertNextValue(val);
				}
			}
		}
	output->GetCellData()->SetScalars(values);
	values->Delete();
  
  return 1;
}

// --------------------------------------
void vtkVulcanGridReader::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "FileName: "
     << (this->FileName ? this->FileName : "(none)") << "\n";
}
