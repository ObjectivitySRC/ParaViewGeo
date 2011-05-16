
#include "vtkRegionsBoundingBox.h"
#include "vtkInformation.h"
#include "vtkObjectFactory.h"
#include "vtkPolyData.h"
#include "vtkDataArray.h"
#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkPointData.h"
#include "vtkStringArray.h"
#include "vtkSmartPointer.h"
#include "vtkStdString.h"
#include "vtkCellDataToPointData.h"
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>
#include <map>
#include <iomanip>
#include <list>
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"

using namespace std;



vtkCxxRevisionMacro(vtkRegionsBoundingBox, "$Revision: 1.27 $");
vtkStandardNewMacro(vtkRegionsBoundingBox);

vtkRegionsBoundingBox::vtkRegionsBoundingBox()
{
	this->SegmentID = NULL;
}

vtkRegionsBoundingBox::~vtkRegionsBoundingBox()
{ 
}


void vtkRegionsBoundingBox::WriteData()
{

	// Output stream that will be used to write the data to the file
	ofstream myFile;
	// Grab object data from paraview
	vtkPolyData *input = vtkPolyData::SafeDownCast(this->GetInput());

	// input returns NULL if the data is not Polygonal
	if(input == NULL)
	{
		vtkErrorMacro("Writer only accepts PolyData, input type invalid.");
		return;
	}

	myFile.open(this->FileName);
	if(!myFile)
	{
		vtkErrorMacro("Error writing to the file");
		return;
	}


	vtkPoints *inPoints = input->GetPoints();
	vtkCellArray *inCells = input->GetPolys();
	vtkDataArray *segID = input->GetCellData()->GetArray(this->SegmentID);
	if(!segID)
	{
		vtkErrorMacro(" Can't find SegmentID array ");
		return;
	}


	(inCells)->InitTraversal();
	vtkIdType *npts = new vtkIdType();	// will store the number of points in current cell 
	vtkIdType *pts;	// will store a array of points id's in current cell 
	vtkIdType num;


	vtkIdType numberOfCells = inCells->GetNumberOfCells();

	map<vtkIdType,vector<pair<vtkIdType*,vtkIdType>>> segments;

	for(vtkIdType currentCell = 0; currentCell < numberOfCells; currentCell++)
	{
		inCells->GetNextCell(*npts, *&pts);
		num = segID->GetComponent(currentCell,0);
		segments[num].push_back(pair<vtkIdType*,vtkIdType>(pts,*npts));
	}


	myFile << "1,Segments Bounding Boxes"<<endl;
	myFile << "2,id,xmin,xmax,ymin,ymax,zmin,zmax"<<endl;

	double point[3];
	vector < double > bounds(6);
	int counter;

	for(map<vtkIdType,vector<pair<vtkIdType*,vtkIdType>>>::iterator iter = segments.begin();
		iter != segments.end(); iter++)
	{
		for (int i=0; i<6; i+=2)
		{
			bounds[i] = VTK_DOUBLE_MAX;
			bounds[i+1] = VTK_DOUBLE_MIN;
		}

		for(vector< pair<vtkIdType*,vtkIdType> >::iterator it = iter->second.begin();
			it != iter->second.end(); it++)
		{
			for(vtkIdType j = 0; j < it->second; j++)
			{
				inPoints->GetPoint((it->first)[j], point);
				counter = 0;

				for(int k=0; k<6; k+=2)
				{	
					if(point[counter] < bounds[k])
						bounds[k] = point[counter];
					if(point[counter] > bounds[k+1])
						bounds[k+1] = point[counter];
					counter++;
				}
			}
		}

		/*for(unsigned int i=0; i < iter->second.size(); i++)
		{
			for(vtkIdType j = 0; j < (iter->second)[i].second; j++)
			{
				inPoints->GetPoint(j, point);

				for(int k=0; k<6; k+=2)
				{
					if(point[k] < bounds[k])
						bounds[k] = point[k];
					else if(point[k+1] > bounds[k+1])
						bounds[k+1] = point[k+1];
				}
			}
		}*/

		myFile <<"3,"<< iter->first;

		for( int i=0; i<6; i++)
		{
			myFile <<","<< bounds[i];
		}
		myFile << endl;
		
	}

	myFile.close();
	}
	

int vtkRegionsBoundingBox::FillInputPortInformation(int, vtkInformation *info)
{
	info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData");
	return 1;
}

vtkPolyData* vtkRegionsBoundingBox::GetInput()
{
	return vtkPolyData::SafeDownCast(this->Superclass::GetInput());
}

vtkPolyData* vtkRegionsBoundingBox::GetInput(int port)
{
	return vtkPolyData::SafeDownCast(this->Superclass::GetInput(port));
}

//Called with this->PrintSelf(*fp, indent)
void vtkRegionsBoundingBox::PrintSelf(ostream& fp, vtkIndent indent)
{
	//this->Superclass::PrintSelf(fp,indent);
}



