/*=========================================================================

  Program:   ParaViewGeo
  Module:    $RCSfile: vtkGocadTSolid.cxx,v $

  Copyright (c) Robert Maynard
  All rights reserved.  
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "vtkGocadTSolid.h"
#include "vtkUnstructuredGrid.h"
#include "vtkObjectFactory.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkCellArray.h"
#include "vtkStringArray.h"

vtkCxxRevisionMacro(vtkGocadTSolid, "$Revision: 0.2 $");
vtkStandardNewMacro(vtkGocadTSolid);


// --------------------------------------
vtkGocadTSolid::vtkGocadTSolid()
{    
  this->DataType = VTK_UNSTRUCTURED_GRID;
}

// --------------------------------------
vtkGocadTSolid::~vtkGocadTSolid()
{  

}

// --------------------------------------
int vtkGocadTSolid::RequestData(vtkInformation* request,
                          vtkInformationVector** inputVector,
                          vtkInformationVector* outputVector)
{

  vtkInformation *outInfo = outputVector->GetInformationObject(0);
  
  vtkUnstructuredGrid *output = vtkUnstructuredGrid::SafeDownCast( outInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkPoints *points = vtkPoints::New();
  vtkCellArray *cells = vtkCellArray::New();
	vtkStringArray *endConditions = vtkStringArray::New();
	endConditions->InsertNextValue("TVOLUME");
	endConditions->InsertNextValue("END");
  
  int continueReading = true;
  const int cellSize = 4;
  const int TETRA_SIZE = 5;
  
  continueReading = this->ReadProperties();
  
  if ( continueReading )
    {
    do{    
      //continue reading
      continueReading = this->ReadPoints( points, "TETRA" );  
      
      //now that we have the points, construct the cells
      continueReading = this->ReadCells( cells, cellSize, "TETRA", TETRA_SIZE, endConditions );      

      }while(this->Line->find("TVOLUME") != vtkstd::string::npos);
    }
  endConditions->Delete();

  //add the points
  output->SetPoints( points );
  points->Delete();
  
  //add the cells
  output->SetCells(VTK_TETRA, cells );
  cells->Delete();
  
	//call atomic requestData
	continueReading = vtkGocadAtomic::RequestData(request, inputVector, outputVector);
  return continueReading;
}

