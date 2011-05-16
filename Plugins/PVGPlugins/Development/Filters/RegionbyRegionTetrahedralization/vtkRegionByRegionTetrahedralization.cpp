#include "vtkRegionByRegionTetrahedralization.h"
#include "vtkDataSet.h"
#include "vtkCellData.h"
#include "vtkPointData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkCellArray.h"
#include "vtkPolyData.h"
#include <math.h>

#include <stdlib.h>
#include "vtkPPolyDataNormals.h"
#include "vtkCleanPolyData.h"
#include "vtkMath.h"
#include "vtkGenericCell.h"
#include "vtkAppendFilter.h"


#include "../../..//Stable/Filters/tetgen/vtkTetgen.h"

#include <vtkstd/map>
#include <vtkstd/vector>



using namespace std;

string doubleToString(double in);


struct Internal
{
	vtkstd::map< vtkIdType, vtkstd::vector<vtkIdType> > Regions;


};


vtkCxxRevisionMacro(vtkRegionByRegionTetrahedralization, "$Revision: 1.34 $");
vtkStandardNewMacro(vtkRegionByRegionTetrahedralization);

vtkRegionByRegionTetrahedralization::vtkRegionByRegionTetrahedralization()
{
	this->RegionArray = NULL;
	this->VolumesArray = NULL;
}
vtkRegionByRegionTetrahedralization:: ~vtkRegionByRegionTetrahedralization()
{

}


int vtkRegionByRegionTetrahedralization::RequestData(vtkInformation *vtkNotUsed(request),
	vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{
  // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  // get the input and ouptut
  vtkPolyData *input = vtkPolyData::SafeDownCast(
    inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkUnstructuredGrid *output = vtkUnstructuredGrid::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

	vtkCellArray *inCells = input->GetPolys();
	if(!inCells)
	{
		vtkErrorMacro("The input must contain triangular cells");
		return 1;
	}

	vtkPoints *inPoints = input->GetPoints();
	if(!inPoints)
	{
		vtkErrorMacro("can't read input points");
		return 1;
	}

	vtkDataArray *regionsArray = input->GetCellData()->GetArray(this->RegionArray);
	if(!regionsArray)
	{
		vtkErrorMacro("can't find region array");
		return 1;
	}


	vtkIdType npts; 


	this->internals = new Internal();

	// for loop on all cells
	for(vtkIdType currentCell = 0; currentCell < input->GetNumberOfCells(); currentCell++)
	{
		this->internals->Regions[regionsArray->GetComponent(currentCell,0)].push_back(currentCell); 
	}

	vtkAppendFilter *append = vtkAppendFilter::New();
	vtkTetgen *tetgen;
	vtkPolyData *tetgenInput;
	vtkCellArray *cells;
	vtkPoints *points;
	vtkIntArray *tempRegionArray; 

	vtkCell* currentCell;
	vtkPoints *currentPoints;
	vtkIdType currentId;


	//inCells->InitTraversal();

	for(vtkstd::map< vtkIdType, vtkstd::vector<vtkIdType> >::iterator it = this->internals->Regions.begin();
		it != this->internals->Regions.end(); it++)
	{
		tetgen = vtkTetgen::New();
		tetgenInput = vtkPolyData::New();
		cells = vtkCellArray::New();
		points = vtkPoints::New();
		currentId = 0;

		tempRegionArray = vtkIntArray::New();
		tempRegionArray->SetName("region");

		for(vtkstd::vector<vtkIdType>::iterator iter = it->second.begin();
			iter != it->second.end(); iter++)
		{
			currentCell = input->GetCell(*iter);
			npts = currentCell->GetNumberOfPoints();
			cells->InsertNextCell(npts);
			currentPoints = currentCell->GetPoints();
			
			tempRegionArray->InsertNextValue(it->first);
			for(int pCounter=0; pCounter<npts; pCounter++)
			{
				cells->InsertCellPoint(currentId++);
				points->InsertNextPoint(currentPoints->GetPoint(pCounter));
			}
		}
		tetgenInput->SetPolys(cells);
		tetgenInput->SetPoints(points);
		//tetgenInput->Update();
		tetgenInput->GetCellData()->AddArray(tempRegionArray);
		tetgen->SetInput(tetgenInput);
		tetgen->SettetgenCommand(1);
		//tetgen->SetWithPredefinedRegions(1);
		//tetgen->SetRegionArray("region");
		tetgen->Update();
		append->AddInput(tetgen->GetOutput());
		
		tetgen->Delete();
		tetgenInput->Delete();
		cells->Delete();
		points->Delete();
		tempRegionArray->Delete();
	}

	append->Update();
	output->DeepCopy(append->GetOutput());
	append->Delete();
/*
	double currentPoint[3];
	double p1[3];
	double p2[3];
	double p3[3];
	double p4[3];
	double volume = 0;
	map < int,vector < vtkGenericCell* > > regions;
	map < int,double > volumes;
	int currentRegion;
	string vArray;

	vtkIntArray *regionNumbers;
	regionNumbers = vtkIntArray::SafeDownCast(input->GetCellData()->GetArray(this->RegionArray));

	if(!regionNumbers)
	{
		vtkErrorMacro("region array is null");
		return 1;
	}

	vtkGenericCell *cell;

	for (vtkIdType i=0; i<input->GetNumberOfCells(); i++)
	{
		cell = vtkGenericCell::New();
		input->GetCell(i, cell);
		if(cell->GetNumberOfPoints() != 4)
		{
			vtkErrorMacro("The input of this filter must be tetrahedralised");
			return 1;
		}

		currentRegion = regionNumbers->GetValue(i);
		regions[currentRegion].push_back(cell);
	}


	for(map <  int,vector < vtkGenericCell* > >::iterator it = regions.begin();
		it != regions.end(); it++)
	{
		for(vector < vtkGenericCell* >::iterator iter = it->second.begin();
			iter != it->second.end(); iter++)
		{
			// get the 4 vertices of the current tetrahedron
			input->GetPoint((*iter)->GetPointId(0),currentPoint);
			this->copyPoint(currentPoint, p1);
			input->GetPoint((*iter)->GetPointId(1),currentPoint);
			this->copyPoint(currentPoint, p2);
			input->GetPoint((*iter)->GetPointId(2),currentPoint);
			this->copyPoint(currentPoint, p3);
			input->GetPoint((*iter)->GetPointId(3),currentPoint);
			this->copyPoint(currentPoint, p4);
			volume += this->tetrahedronVolume(p1, p2, p3, p4);
			(*iter)->Delete();
		}
		volumes[it->first] = volume;
		volume = 0;
	}

	for(map < int,double >::iterator it = volumes.begin();
		it != volumes.end(); it++)
	{
		//vtkErrorMacro("volume"<<it->first<<"= "<< it->second);
		vArray += doubleToString(it->second);
		vArray += "|";
		//this->VolumesArray->InsertNextValue(it->second);
	}

	this->VolumesArray = new char[vArray.length()];
	strcpy(this->VolumesArray, vArray.c_str());

	*/

	return 1;

}


//----------------------------------------------------------------------------
void vtkRegionByRegionTetrahedralization::copyPoint(double *src, double *dest)
{
	dest[0] = src[0];
	dest[1] = src[1];
	dest[2] = src[2];
}


//----------------------------------------------------------------------------
double vtkRegionByRegionTetrahedralization::tetrahedronVolume(double *p1, double *p2, double *p3, double *p4)
{
	// volume = | v1.(v2xv3)|/6

	double v1[3];
	double v2[3];
	double v3[3];
	double cross[3];

	for(int i=0 ; i<3 ; i++) v1[i] = p1[i] - p4[i];
	for(int i=0 ; i<3 ; i++) v2[i] = p2[i] - p4[i];
	for(int i=0 ; i<3 ; i++) v3[i] = p3[i] - p4[i];

	vtkMath::Cross(v2, v3, cross);
	return fabs( vtkMath::Dot(v1, cross))/6;
}

//----------------------------------------------------------------------------
int vtkRegionByRegionTetrahedralization::FillInputPortInformation(int, vtkInformation *info)
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataSet");
  return 1;
}



/*
//---------------------------------------------------------------------------------------------
string doubleToString(double in)
{
	std::stringstream out;
	out << in;
	return out.str();
}
//---------------------------------------------------------------------------------------------
*/