//===================================================================================//
//				Mirarco Mining Innovation
//
// Filter:   Bounding Box
// Class:    vtkBBox
// Author:   Nehmeh Bilal
// Director: Marie-Grabrielle Valet 
// Date:     July 2008
// contact: nehmebilal@gmail.com 
//====================================================================================//

#include "vtkBBox.h"
#include "vtkDataSet.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkCellArray.h"
#include "vtkPolyData.h"
#include <math.h>
#include<iostream>
//#include<conio.h>


vtkCxxRevisionMacro(vtkBBox, "$Revision: 1.34 $");
vtkStandardNewMacro(vtkBBox);

//----------------------------------------------------------------------------
vtkBBox::vtkBBox ()
{
}

//----------------------------------------------------------------------------
vtkBBox::~vtkBBox ()
{
}

//----------------------------------------------------------------------------

int vtkBBox::RequestData(vtkInformation *vtkNotUsed(request), 
	vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{
  // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  // get the input and ouptut
  vtkDataSet *input = vtkDataSet::SafeDownCast(
    inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkPolyData *output = vtkPolyData::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

	double bounds[6];
	input->GetBounds(bounds);

								/////////
	double Xcentre = 0;	 //	
	double Ycentre = 0;	 // coordinates of the boundingBox centre
	double Zcentre = 0;	 //
								/////////	

	double Xlength = (bounds[1]-bounds[0])/2;  // length of X compement of the original bounding box diveded by 2
	double Ylength = (bounds[3]-bounds[2])/2;	 // length of Y compement of the original bounding box diveded by 2	
	double Zlength = (bounds[5]-bounds[4])/2;	 // length of Z compement of the original bounding box diveded by 2	

	Xcentre = bounds[0] + Xlength;	// Xcentre = Xmin + (Xmax - Xmin)/2
	Ycentre = bounds[2] + Ylength;	// Ycentre = Ymin + (Ymax - Ymin)/2
	Zcentre = bounds[4] + Zlength;	// Zcentre = Zmin + (Zmax - Zmin)/2

	bounds[0] = Xcentre - (this->deltaX/100)*Xlength;		// new Xmin = Xcentre - [(deltaX/100)*( Xmax - Xmin)/2] 
	bounds[1] = Xcentre + (this->deltaX/100)*Xlength;		// new Xmax = Xcentre + [(deltaX/100)*( Xmax - Xmin)/2]
	bounds[2] = Ycentre - (this->deltaY/100)*Ylength;		// new Ymin = Ycentre - [(deltaY/100)*( Ymax - Ymin)/2]
	bounds[3] = Ycentre + (this->deltaY/100)*Ylength;		// new Ymax = Ycentre + [(deltaY/100)*( Ymax - Ymin)/2]
	bounds[4] = Zcentre - (this->deltaZ/100)*Zlength;		// new Zmin = Zcentre + [(deltaZ/100)*( Zmax - Zmin)/2]
	bounds[5] = Zcentre + (this->deltaZ/100)*Zlength;		// new Zmax = Zcentre + [(deltaZ/100)*( Zmax - Zmin)/2]

	
	vtkPoints *points ;
	points = vtkPoints::New();
	vtkCellArray *cells ;
	cells = vtkCellArray::New();
	points->Allocate(8);
	cells->Allocate(6);

	points->InsertNextPoint(bounds[0],bounds[2],bounds[4]);
	points->InsertNextPoint(bounds[1],bounds[2],bounds[4]);
	points->InsertNextPoint(bounds[1],bounds[3],bounds[4]);
	points->InsertNextPoint(bounds[0],bounds[3],bounds[4]);
	points->InsertNextPoint(bounds[0],bounds[2],bounds[5]);
	points->InsertNextPoint(bounds[1],bounds[2],bounds[5]);
	points->InsertNextPoint(bounds[1],bounds[3],bounds[5]);
	points->InsertNextPoint(bounds[0],bounds[3],bounds[5]);

	cells->InsertNextCell(4);
	cells->InsertCellPoint(0);
	cells->InsertCellPoint(3);
	cells->InsertCellPoint(2);
	cells->InsertCellPoint(1);

	cells->InsertNextCell(4);
	cells->InsertCellPoint(4);
	cells->InsertCellPoint(5);
	cells->InsertCellPoint(6);
	cells->InsertCellPoint(7);

	cells->InsertNextCell(4);
	cells->InsertCellPoint(0);
	cells->InsertCellPoint(1);
	cells->InsertCellPoint(5);
	cells->InsertCellPoint(4);

	cells->InsertNextCell(4);
	cells->InsertCellPoint(3);
	cells->InsertCellPoint(7);
	cells->InsertCellPoint(6);
	cells->InsertCellPoint(2);

	cells->InsertNextCell(4);
	cells->InsertCellPoint(1);
	cells->InsertCellPoint(2);
	cells->InsertCellPoint(6);
	cells->InsertCellPoint(5);

	cells->InsertNextCell(4);
	cells->InsertCellPoint(0);
	cells->InsertCellPoint(4);
	cells->InsertCellPoint(7);
	cells->InsertCellPoint(3);

	output->SetPoints(points);
	output->SetPolys(cells);

	points -> Delete();
	cells -> Delete();
  return 1;
}

//----------------------------------------------------------------------------
int vtkBBox::FillInputPortInformation(int, vtkInformation *info)
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataSet");
  return 1;
}
