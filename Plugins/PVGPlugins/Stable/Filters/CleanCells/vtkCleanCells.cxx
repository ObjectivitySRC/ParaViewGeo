//===================================================================================//
//				Mirarco Mining Innovation
//
// Filter:   Clean Faces
// Class:    vtkCleanCells
// Author:   Nehmeh Bilal
// Director: Marie-Grabrielle Valet 
// Date:     July 2008
// contact: nehmebilal@gmail.com 
//====================================================================================//

#include "vtkCleanCells.h"
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
#include <list>
#include <algorithm>
#include <stdlib.h>
#include "vtkPPolyDataNormals.h"
#include "vtkCleanPolyData.h"
#include "vtkFloatArray.h"
#include "vtkDataArray.h"

using namespace std;

vtkCxxRevisionMacro(vtkCleanCells, "$Revision: 1.34 $");
vtkStandardNewMacro(vtkCleanCells);

vtkCleanCells::vtkCleanCells()
{
  	this->PieceInvariant = 1;
	this->Tolerance = 0;
	this->AbsoluteTolerance = 0;
	this->ToleranceIsAbsolute = 0;
	this->ConvertLinesToPoints = 0;
	this->ConvertPolysToLines = 0;
	this->ConvertStripsToPolys = 1;
	this->PointMerging = 1;

}
vtkCleanCells:: ~vtkCleanCells(){}

void vtkCleanCells::CleanCellGroup(vtkCellArray *groupInput, vtkCellArray *groupOutput,
																	 vtkCellData  *inputCD, vtkCellData *outputCD)
{
// to clean cells, we will save all cells in a c++ map. for each cell we copy the points id in a vector,
// sort this vector, and add this vector as a key of the map. finaly all cells formed by the same points
// will be a unique key in the map, so we will not have any duplicated cell. 
// the map will look like: map<Pid1, Pid2, Pid3, ...>,<cellId, Pid1, Pid2, Pid3, ...> 
	map< list<int>,list<int> > cellMap;
	list<int> cellVect;
	list<int> cellVect2;

	int numberOfCells = groupInput->GetNumberOfCells();

	(groupInput)->InitTraversal();
	// will store the number of points in current cell
	vtkIdType *npts = new vtkIdType(); 
	// will store a array of points id's in current cell
	vtkIdType *pts; 

	for(int currentCell = 0; currentCell < groupInput->GetNumberOfCells(); currentCell++)
	{
		(groupInput)->GetNextCell(*npts, *&pts);
		cellVect.push_back(currentCell);	// We add the id of the cell to the vector, because when we will transfer cell 
																			// data from the input to the output, we will need the id of the input cell
																			// from where we want to copy the data

		for(int currentPoint=0 ; currentPoint<*npts; currentPoint++)
		{
			cellVect.push_back(pts[currentPoint]);		// we add the points id's to the map values and keys
			cellVect2.push_back(pts[currentPoint]);
		}
		
		cellVect.sort(); // sorting the map keys

		cellMap[cellVect2] = cellVect;		// if cellVect2 dont exist as a key in the map, a new key will be added,
																			// otherwise, the value of the key equal to cellVect2 will be replaced 
																			// by a new cellVect. 

		cellVect.clear();	
		cellVect2.clear();
	}

	// we want to transfer data from input cell located by the variable oldID, to a new ouput cell located by
	// the variable newID. In the same for loop, we will transfer the cleaned points and cells to the output.
	vtkIdType newID = 0;	
	vtkIdType oldID = 0;
	for(map<list<int>, list<int> >::iterator iter=cellMap.begin(); iter!=cellMap.end(); iter++)
	{
		oldID = iter->second.front();		// id of the old cell having the same points ids of the current cell 
		newID = groupOutput->InsertNextCell(iter->second.size()-1);
		outputCD->CopyData(inputCD, oldID, newID);

		for(list<int>::iterator vIt = iter->second.begin(); vIt!= iter->second.end(); ++vIt)
		{
			groupOutput->InsertCellPoint(*vIt);
		}
	}

	delete npts;
}


int vtkCleanCells::RequestData(vtkInformation *vtkNotUsed(request), 
	vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{
  // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  // get the input and ouptut
  vtkPolyData *input = vtkPolyData::SafeDownCast(
    inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkPolyData *output = vtkPolyData::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

//------------------------------------------------ Clean Filter ------------------------------------------------------
	// here we call the clean filter to clean points before cleaning faces

	 vtkCleanPolyData *connect1 = vtkCleanPolyData::New();
	 connect1->SetInput(input);
	 connect1->SetPieceInvariant(this->PieceInvariant);
	 connect1->SetTolerance(this->Tolerance);
	 connect1->SetAbsoluteTolerance(this->AbsoluteTolerance);
	 connect1->SetToleranceIsAbsolute(this->ToleranceIsAbsolute);
	 connect1->SetConvertLinesToPoints(this->ConvertLinesToPoints);
	 connect1->SetConvertPolysToLines(this->ConvertPolysToLines);
	 connect1->SetConvertPolysToLines(this->ConvertStripsToPolys);
	 connect1->SetPointMerging(this->PointMerging);
	 connect1->Update();
	 input->ShallowCopy(connect1->GetOutput());
	 connect1->Delete();
//---------------------------------------------------------------------------------------------------------------

  vtkPointData *inputPD = input->GetPointData();
  vtkCellData  *inputCD = input->GetCellData();

// we want to transfer the points and cells data from the input to the output, but we don't know how many cells we
// will have after cleaning faces, that why we use the folowing way to transfer the data.

	vtkPolyData *outputTemp = vtkPolyData::New();

  vtkPointData *outputPD = outputTemp->GetPointData();		
  vtkCellData  *outputCD = outputTemp->GetCellData();
  inputPD = input->GetPointData();
  inputCD = input->GetCellData();
  outputPD->CopyAllocate(inputPD);
  outputCD->CopyAllocate(inputCD);

	
	// MLivingstone
	// Make sure we check Polys, Lines and Verts
	vtkCellArray *polyCells = input->GetPolys();
	vtkCellArray *polyCellsOut = vtkCellArray::New();
	if(polyCells->GetNumberOfCells() > 0)
		{
		CleanCellGroup(polyCells, polyCellsOut, inputCD, outputCD);
		}

	vtkCellArray *lineCells = input->GetLines();
	vtkCellArray *lineCellsOut = vtkCellArray::New();
	if(lineCells->GetNumberOfCells() > 0)
		{
		CleanCellGroup(lineCells, lineCellsOut, inputCD, outputCD);
		}

	vtkCellArray *vertCells = input->GetVerts();
	vtkCellArray *vertCellsOut = vtkCellArray::New();
	if(vertCells->GetNumberOfCells() > 0)
		{
		CleanCellGroup(vertCells, vertCellsOut, inputCD, outputCD);
		}

	this->inPoints = input->GetPoints();

	// Make sure out point data is copied properly
	outputTemp->GetPointData()->PassData(input->GetPointData());

	// Create a PolyData that will be used to create normals for the final output
	outputTemp->SetPoints(this->inPoints);

	if(polyCellsOut->GetNumberOfCells() > 0)	
		outputTemp->SetPolys(polyCellsOut);
	if(lineCellsOut->GetNumberOfCells() > 0)
		outputTemp->SetLines(lineCellsOut);
	if(vertCellsOut->GetNumberOfCells() > 0)
		outputTemp->SetVerts(vertCellsOut);

	// Calculate the normals
	vtkPolyDataNormals* connect= vtkPPolyDataNormals::New();
	connect->SetInput(outputTemp);
	connect->SetFeatureAngle(90);
	// Must be 0 or all the points/cells that were removed will be created again
	connect->SetSplitting(0);
	connect->SetConsistency(1);
	connect->SetFlipNormals(0);
	connect->SetNonManifoldTraversal(1);
	connect->SetComputeCellNormals(0);

	connect->Update();
	output->ShallowCopy(connect->GetOutput());		
	
	connect->Delete();

	polyCellsOut->Delete();
	lineCellsOut->Delete();
	vertCellsOut->Delete();
	// /MLivingstone

	outputTemp->Delete();

  return 1;
}

//----------------------------------------------------------------------------
int vtkCleanCells::FillInputPortInformation(int, vtkInformation *info)
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataSet");
  return 1;
}
