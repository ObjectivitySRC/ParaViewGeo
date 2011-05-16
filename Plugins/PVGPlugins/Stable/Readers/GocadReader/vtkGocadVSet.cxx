/*=========================================================================

  Program:   ParaViewGeo
  Module:    $RCSfile: vtkGocadVSet.cxx,v $

  Copyright (c) Robert Maynard
  All rights reserved.  
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkGocadVSet.h"
#include "vtkObjectFactory.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkCellArray.h"


vtkCxxRevisionMacro(vtkGocadVSet, "$Revision: 0.3 $");
vtkStandardNewMacro(vtkGocadVSet);

// --------------------------------------
vtkGocadVSet::vtkGocadVSet()
{    
  int NumberOfPoints=0;
  
}

// --------------------------------------
vtkGocadVSet::~vtkGocadVSet()
{  

}

// --------------------------------------
int vtkGocadVSet::RequestData(vtkInformation* request,
                          vtkInformationVector** inputVector,
                          vtkInformationVector* outputVector)
{

  vtkInformation *outInfo = outputVector->GetInformationObject(0);
  
  vtkPolyData *output = vtkPolyData::SafeDownCast( outInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkPoints *points = vtkPoints::New();
  vtkCellArray *cells = vtkCellArray::New();
  
  int continueReading = true;
  
  continueReading = this->ReadProperties();
  
  if ( continueReading )
    {    
      //continue reading            
      continueReading = this->ReadPoints( points, "END" );  
      
     
      //now that we have the points, construct the cells
      continueReading = this->ReadCells( cells, points->GetNumberOfPoints() );      
    }
  
  //add the points
  output->SetPoints( points );
  points->Delete();
  
  //add the cells
  output->SetVerts( cells );
  cells->Delete();
  
	//call atomic requestData
	continueReading = vtkGocadAtomic::RequestData(request, inputVector, outputVector);
  return continueReading;
}


// --------------------------------------
bool vtkGocadVSet::ReadCells( vtkCellArray* cells, int numberOfPoints )
{
 //make a cell of size 1 for each point
 
 for (int i=0; i < numberOfPoints; i++)
  {
  cells->InsertNextCell( 1 );
  cells->InsertCellPoint( i );
  }  
 return 1;
}

