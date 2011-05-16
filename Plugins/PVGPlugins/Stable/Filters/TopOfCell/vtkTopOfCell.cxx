/*=========================================================================

   Program:   Visualization Toolkit
  Module:    $ vtkTopOfCell.h $
  Author:     Arolde VIDJINNAGNI& Nehme BILAL
  MIRARCO, Laurentian University
  Date:      May 22 2008
  Version:   0.1
=========================================================================*/

#include "vtkTopOfCell.h"

#include "vtkCellArray.h"
#include "vtkFloatArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkPointData.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"

#include <math.h>
vtkCxxRevisionMacro(vtkTopOfCell, "$Revision: 0.1 $");
vtkStandardNewMacro(vtkTopOfCell);

vtkTopOfCell::vtkTopOfCell()
{
}

/***************************************************************************
 Name:            RequestData
 Types of variables: input and output
 Description: 
"****************************************************************************/
int vtkTopOfCell::RequestData(vtkInformation *vtkNotUsed(request), 
	vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{
	 // get the info objects
	 vtkInformation *inInfo = inputVector[0]->GetInformationObject ( 0 );
	 vtkInformation *outInfo = outputVector->GetInformationObject ( 0 );

	 // get the input and ouptut
	 vtkPolyData *input = vtkPolyData::SafeDownCast (
						  inInfo->Get ( vtkDataObject::DATA_OBJECT() ) );

	this->Surfaces = input->GetPolys();
	this->StripSurfaces = input->GetStrips();
	this->Lines = input->GetLines();
	this->Points = input->GetVerts();

	this->PointData = input->GetPointData();
	this->CellData = input->GetCellData();
	this->PointCoords = input->GetPoints()->GetData();

	if((this->Surfaces)->GetNumberOfCells() > 0)
	{
		this->Cells = input->GetPolys();
	}
	else if((this->Lines)->GetNumberOfCells() > 0)
	{
		this->Cells = input->GetLines();
	}
	else if((this->Points)->GetNumberOfCells() > 0)
	{
		this->Cells = input->GetVerts();
	}
	else if((this->StripSurfaces)->GetNumberOfCells() > 0)
	{
		this->Cells = input->GetStrips();
	}	

	// Stores number of points in current cell
	vtkIdType* npts = new vtkIdType(); 

	// Create array of points in current cell.
	vtkIdType *pts; 

	this->numberOfCells = (this->Cells)->GetNumberOfCells();
	(this->Cells)->InitTraversal();

	//list od id(s) of the points top
	this->TopPoints= new double*[numberOfCells];

	for(int i=0; i<numberOfCells;i++)
	this->TopPoints[i]= new double[3];

	double* currentTuple;

	for(int currentCell = 0; currentCell < this->numberOfCells; currentCell++){

		(this->Cells)->GetNextCell(*npts, *&pts);
		currentTuple = (this->PointCoords)->GetTuple(pts[0]);

		if(this->TopOfCellType==4)
		{
			double Zmax= currentTuple[2];

			this->TopPoints[currentCell][0]= currentTuple[0];
			this->TopPoints[currentCell][1]= currentTuple[1];
			this->TopPoints[currentCell][2]= currentTuple[2];

			for(int currentPoint = 1; currentPoint < *npts; currentPoint++){
				currentTuple= NULL;
				currentTuple = (this->PointCoords)->GetTuple(pts[currentPoint]);

				if(currentTuple[2]> Zmax ){
					Zmax= currentTuple[2];
					this->TopPoints[currentCell][0]= currentTuple[0];
					this->TopPoints[currentCell][1]= currentTuple[1];
					this->TopPoints[currentCell][2]= currentTuple[2];
				}
			}

		}

		else if(this->TopOfCellType==5){
			double Zmin= currentTuple[2];

			this->TopPoints[currentCell][0]= currentTuple[0];
			this->TopPoints[currentCell][1]= currentTuple[1];
			this->TopPoints[currentCell][2]= currentTuple[2];

			for(int currentPoint = 1; currentPoint < *npts; currentPoint++){
				currentTuple= NULL;
				currentTuple = (this->PointCoords)->GetTuple(pts[currentPoint]);

				if(currentTuple[2]< Zmin ){
					Zmin= currentTuple[2];
					this->TopPoints[currentCell][0]= currentTuple[0];
					this->TopPoints[currentCell][1]= currentTuple[1];
					this->TopPoints[currentCell][2]= currentTuple[2];
				}
			}

		}
		else if(this->TopOfCellType==0){
			double Xmax= currentTuple[0];

			this->TopPoints[currentCell][0]= currentTuple[0];
			this->TopPoints[currentCell][1]= currentTuple[1];
			this->TopPoints[currentCell][2]= currentTuple[2];

			for(int currentPoint = 1; currentPoint < *npts; currentPoint++){
				currentTuple= NULL;
				currentTuple = (this->PointCoords)->GetTuple(pts[currentPoint]);

				if(currentTuple[0]> Xmax ){
					Xmax= currentTuple[0];
					this->TopPoints[currentCell][0]= currentTuple[0];
					this->TopPoints[currentCell][1]= currentTuple[1];
					this->TopPoints[currentCell][2]= currentTuple[2];
				}
			}

		}
		else if(this->TopOfCellType==1){
			double Xmin= currentTuple[0];

			this->TopPoints[currentCell][0]= currentTuple[0];
			this->TopPoints[currentCell][1]= currentTuple[1];
			this->TopPoints[currentCell][2]= currentTuple[2];

			for(int currentPoint = 1; currentPoint < *npts; currentPoint++){
				currentTuple= NULL;
				currentTuple = (this->PointCoords)->GetTuple(pts[currentPoint]);

				if(currentTuple[0]< Xmin ){
					Xmin= currentTuple[0];
					this->TopPoints[currentCell][0]= currentTuple[0];
					this->TopPoints[currentCell][1]= currentTuple[1];
					this->TopPoints[currentCell][2]= currentTuple[2];
				}
			}

		}
		else if(this->TopOfCellType==2){
			double Ymax= currentTuple[1];

			this->TopPoints[currentCell][0]= currentTuple[0];
			this->TopPoints[currentCell][1]= currentTuple[1];
			this->TopPoints[currentCell][2]= currentTuple[2];

			for(int currentPoint = 1; currentPoint < *npts; currentPoint++){
				currentTuple= NULL;
				currentTuple = (this->PointCoords)->GetTuple(pts[currentPoint]);

				if(currentTuple[1]> Ymax ){
					Ymax= currentTuple[1];
					this->TopPoints[currentCell][0]= currentTuple[0];
					this->TopPoints[currentCell][1]= currentTuple[1];
					this->TopPoints[currentCell][2]= currentTuple[2];
				}
			}

		}
		else if(this->TopOfCellType==3){
			double Ymin= currentTuple[1];

			this->TopPoints[currentCell][0]= currentTuple[0];
			this->TopPoints[currentCell][1]= currentTuple[1];
			this->TopPoints[currentCell][2]= currentTuple[2];

			for(int currentPoint = 1; currentPoint < *npts; currentPoint++){
				currentTuple= NULL;
				currentTuple = (this->PointCoords)->GetTuple(pts[currentPoint]);

				if(currentTuple[1]< Ymin ){
					Ymin= currentTuple[1];
					this->TopPoints[currentCell][0]= currentTuple[0];
					this->TopPoints[currentCell][1]= currentTuple[1];
					this->TopPoints[currentCell][2]= currentTuple[2];
				}
			}

		}


	}
	vtkPolyData *output = vtkPolyData::SafeDownCast (
							 outInfo->Get ( vtkDataObject::DATA_OBJECT() ) );	 	
	vtkPoints* newpoints; 
	vtkCellArray* cellarray ;   

	newpoints= vtkPoints::New(); 
	cellarray = vtkCellArray::New();
    newpoints->Allocate(this->numberOfCells);
    cellarray->Allocate(this->numberOfCells);

	for (int i=0 ; i < this->numberOfCells; i++ ) 
	{
		 newpoints->InsertNextPoint( this->TopPoints[i][0],this->TopPoints[i][1],this->TopPoints[i][2]);
		 cellarray->InsertNextCell(1);
		 cellarray->InsertCellPoint(i);
	}
	output->SetPoints(newpoints);
	output->SetVerts(cellarray);

return 1;
}

/***************************************************************************
  Name:            PrintSelf
 Types of variables:
 Description:
"****************************************************************************/
void vtkTopOfCell::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  for (int i=0 ; i < this->numberOfCells; i++ ) 
  {
	  os << indent << "Point Max: (" << this->TopPoints[i][0] << ", "
									 << this->TopPoints[i][1] << ", "
								     << this->TopPoints[i][2] << ")\n";	
  }
}

/********************************************************************************
  Name:            FillInputPortInformation
 Types of variables:
 Description:
"*********************************************************************************/
int vtkTopOfCell::FillInputPortInformation ( int vtkNotUsed(port), vtkInformation* info )
  {
  info->Set ( vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData", 0 );
  return 1;
  }