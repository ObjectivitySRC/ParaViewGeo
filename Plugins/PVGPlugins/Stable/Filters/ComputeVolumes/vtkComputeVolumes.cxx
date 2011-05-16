#include "vtkComputeVolumes.h"
#include "vtkDataSet.h"
#include "vtkCellData.h"
#include "vtkPointData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkCellArray.h"
#include "vtkPolyData.h"
#include <math.h>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <algorithm>
//#include <conio.h>
//#include <windows.h>
#include <stdlib.h>
#include "vtkPPolyDataNormals.h"
#include "vtkCleanPolyData.h"
#include "vtkMath.h"
#include "vtkGenericCell.h"
#include<sstream>
#include<fstream> 



using namespace std;

string doubleToString(double in);


vtkCxxRevisionMacro(vtkComputeVolumes, "$Revision: 1.34 $");
vtkStandardNewMacro(vtkComputeVolumes);

vtkComputeVolumes::vtkComputeVolumes()
{
	this->RegionArray = NULL;
	this->VolumesArray = NULL;
}
vtkComputeVolumes:: ~vtkComputeVolumes()
{

}


int vtkComputeVolumes::RequestData(vtkInformation *vtkNotUsed(request), 
	vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{
  // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  // get the input and ouptut
  vtkUnstructuredGrid *input = vtkUnstructuredGrid::SafeDownCast(
    inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkUnstructuredGrid *output = vtkUnstructuredGrid::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

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

	return 1;

}


//----------------------------------------------------------------------------
void vtkComputeVolumes::copyPoint(double *src, double *dest)
{
	dest[0] = src[0];
	dest[1] = src[1];
	dest[2] = src[2];
}


//----------------------------------------------------------------------------
double vtkComputeVolumes::tetrahedronVolume(double *p1, double *p2, double *p3, double *p4)
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
int vtkComputeVolumes::FillInputPortInformation(int, vtkInformation *info)
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataSet");
  return 1;
}




//---------------------------------------------------------------------------------------------
string doubleToString(double in)
{
	std::stringstream out;				
	out << in;				
	return out.str();
}
//---------------------------------------------------------------------------------------------