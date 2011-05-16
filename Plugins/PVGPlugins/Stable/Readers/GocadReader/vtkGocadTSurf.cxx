/*=========================================================================

  Program:   ParaViewGeo
  Module:    $RCSfile: vtkGocadTSurf.cxx,v $

  Copyright (c) Robert Maynard
  All rights reserved.  
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkGocadTSurf.h"
#include "vtkObjectFactory.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkCellArray.h"
#include "vtkStringArray.h"

vtkCxxRevisionMacro(vtkGocadTSurf, "$Revision: 0.2 $");
vtkStandardNewMacro(vtkGocadTSurf);


// --------------------------------------
vtkGocadTSurf::vtkGocadTSurf()
{    
  
}

// --------------------------------------
vtkGocadTSurf::~vtkGocadTSurf()
{  

}

// --------------------------------------
int vtkGocadTSurf::RequestData(vtkInformation* request,
                          vtkInformationVector** inputVector,
                          vtkInformationVector* outputVector)
{

  vtkInformation *outInfo = outputVector->GetInformationObject(0);
  
  vtkPolyData *output = vtkPolyData::SafeDownCast( outInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkPoints *points = vtkPoints::New();
  vtkCellArray *cells = vtkCellArray::New();

	vtkStringArray *endConditions = vtkStringArray::New();
	endConditions->InsertNextValue("TFACE");
	endConditions->InsertNextValue("BSTONE");
	endConditions->InsertNextValue("END");
  
  int continueReading = true;
  
  continueReading = this->ReadProperties();
  
  if ( continueReading )
    {
    do{    
      //continue reading
            
      continueReading = this->ReadPoints( points, "TRGL" );  
      
      const int cellSize = 3;
      const int TFACE_SIZE = 4;
      //now that we have the points, construct the cells
			continueReading = this->ReadCells( cells, cellSize, "TRGL", TFACE_SIZE, endConditions );

      }while(this->Line->find("TFACE") != vtkstd::string::npos);
    }
	endConditions->Delete();
  
  //add the points
  output->SetPoints( points );
  points->Delete();
  
  //add the cells
  output->SetPolys( cells );
  cells->Delete();
 
	//call atomic requestData
	continueReading = vtkGocadAtomic::RequestData(request, inputVector, outputVector);
  return continueReading;
}
