/*=========================================================================

  Program:   ParaViewGeo
  Module:    $RCSfile: vtkGocadPLine.cxx,v $

  Copyright (c) Robert Maynard
  All rights reserved.  
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkGocadPLine.h"
#include "vtkObjectFactory.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkCellArray.h"
#include "vtkStringArray.h"

vtkCxxRevisionMacro(vtkGocadPLine, "$Revision: 0.2 $");
vtkStandardNewMacro(vtkGocadPLine);


// --------------------------------------
vtkGocadPLine::vtkGocadPLine()
{    

}

// --------------------------------------
vtkGocadPLine::~vtkGocadPLine()
{  

}

// --------------------------------------
int vtkGocadPLine::RequestData(vtkInformation* request,
                          vtkInformationVector** inputVector,
                          vtkInformationVector* outputVector)
{

  vtkInformation *outInfo = outputVector->GetInformationObject(0);
  
  vtkPolyData *output = vtkPolyData::SafeDownCast( outInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkPoints *points = vtkPoints::New();
  vtkCellArray *cells = vtkCellArray::New();
	vtkStringArray *endConditions = vtkStringArray::New();
	endConditions->InsertNextValue("ILINE");
	endConditions->InsertNextValue("END");
  
  int continueReading = true;
  
  continueReading = this->ReadProperties();
 
  if ( continueReading )
    {
    do{    
      //continue reading
            
      continueReading = this->ReadPoints( points, "SEG" );  
      
      //now that we have the points, construct the cells and points
      const int cellSize = 2;
      const int SEG_SIZE = 3;
      continueReading = this->ReadCells( cells, cellSize, "SEG", SEG_SIZE, endConditions );

      }while(this->Line->find("ILINE") != vtkstd::string::npos);
    }
  endConditions->Delete();

  //add the points
  output->SetPoints( points );
  points->Delete();
  
  //add the cells
  output->SetLines( cells );
  cells->Delete();

	//call atomic requestData
	continueReading = vtkGocadAtomic::RequestData(request, inputVector, outputVector);  
  return continueReading;
}



