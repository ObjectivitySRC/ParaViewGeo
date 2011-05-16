/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkTimeThreshold.cxx,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/*
	vtkTimeThreshold
	Threshold data based on dates (MSDate, Julian, Modified Julian, Matlab support).
	Also converts string dates. Example 7/15/2008 to MSDate 39644 .

	Matthew Livingstone, Robert Maynard
*/
#include "vtkTimeThreshold.h"
#include "TimeConverter.h"

#include "vtkCell.h"
#include "vtkCellData.h"
#include "vtkIdList.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkUnstructuredGrid.h"
#include "vtkStringArray.h"
#include "vtkDoubleArray.h"
#include "vtkPolyData.h"

vtkCxxRevisionMacro(vtkTimeThreshold, "$Revision: 1.80 $");
vtkStandardNewMacro(vtkTimeThreshold);

// Construct with lower threshold=0, upper threshold=1, and threshold 
// function=upper AllScalars=1.
vtkTimeThreshold::vtkTimeThreshold()
{    
  this->LowerChar = 0;
  this->UpperChar = 0;
	this->Threshold = vtkThreshold::New();
    
  //need to set the type of threshold function to use
  //since we need between, and upper is the threshold default
	this->Threshold->ThresholdBetween( this->LowerThreshold, this->UpperThreshold);
}

vtkTimeThreshold::~vtkTimeThreshold()
{
	this->Threshold->Delete();
}

int vtkTimeThreshold::RequestData(vtkInformation *request,vtkInformationVector **inputVector,vtkInformationVector *outputVector)
{
	// MLivingstone START
	vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  // get the input and ouptut
  vtkDataSet *input = vtkDataSet::SafeDownCast(
    inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkUnstructuredGrid *output = vtkUnstructuredGrid::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

	// Array to hold the converted date values
	vtkDoubleArray* convertedArray = vtkDoubleArray::New();

	// Check to see if we have been given a StringArray
	vtkStringArray *inScalars = vtkStringArray::SafeDownCast( this->GetInputAbstractArrayToProcess(0,inputVector) );
	vtkStdString origName = this->GetInputAbstractArrayToProcess(0,inputVector)->GetName();
  
	// If we have string data, we have to convert it to a double format,
	// as vtkThreshold operates on double values
  if (inScalars)
    {

		// Copy name of array
		origName = inScalars->GetName();
		// Change the name so we can find it later
		origName = origName.append("Conv");

		convertedArray->SetName( origName );

		// TimeConverter will be used to convert the date to which ever
		// format has been selected
		TimeConverter timeC = TimeConverter();

		for( int currProp = 0; currProp < inScalars->GetNumberOfValues(); currProp++ )
			{
			int year, month, day;
			// String *should* come is as dd/mm/yyyy
			sscanf(inScalars->GetValue(currProp),"%d/%d/%d",&day,&month,&year);  
			
			// Pass values to TimeConverter so it can do the hard work
			timeC.setMonth( month );
			timeC.setDay( day );
			timeC.setYear( year );

			// Will hold the value of the converted date
			double convertedValue;

			// Determine which format the has been selected, 
			// and get TimeConverter to do the respective conversion
			switch(this->TimeFormat)
				{ 
				case VTK_MSDATE:
					convertedValue = timeC.GetMSDate();
					break;    
				case VTK_JULIAN:
					convertedValue = timeC.GetJulianDate();
					break;
				case VTK_JULIAN_M:                  
					convertedValue = timeC.GetModifiedJulianDate();       
					break;
				case VTK_MATLAB:                  
					convertedValue = timeC.GetMatlabDate();   
					break;      
				}

			// Store the converted values
			convertedArray->InsertNextValue( convertedValue );
			}
		}

  //convert the char to doubles, and store them
  this->Convert();

	// Store cell or point property
	// 0 = cell, 1 = point
	// Initialized to -1 incase we don't actually need to remove anything
	int cellPoint = -1;

	if( inScalars )
		{

		// Determine if property is a Cell or Point property
		if( convertedArray->GetNumberOfTuples() == input->GetNumberOfCells() )
			{
			cellPoint = 0;
			input->GetCellData()->AddArray( convertedArray );
			}
		else
			{
			cellPoint = 1;
			input->GetPointData()->AddArray( convertedArray );
			}
		}

	// Clean up new array
	convertedArray->Delete();
		
	this->Threshold->SetInput( input );

	// Tell Threshold which array we are using to filter data
	this->Threshold->SetInputArrayToProcess(0,0,0,vtkDataObject::FIELD_ASSOCIATION_POINTS_THEN_CELLS,
																					origName);
	// Set the threshold limits
	this->Threshold->ThresholdBetween( this->LowerThreshold, this->UpperThreshold );
	// Force an update
	this->Threshold->Update();

	// Grab threshold results
	output->CopyStructure( this->Threshold->GetOutput() );
	output->GetPointData()->PassData( this->Threshold->GetOutput()->GetPointData() );
  output->GetCellData()->PassData( this->Threshold->GetOutput()->GetCellData() );

	// Remove the newly added array from input
	if( cellPoint == 0 )
		{
		input->GetCellData()->RemoveArray( origName );
		}
	if( cellPoint == 1 )
		{
		input->GetPointData()->RemoveArray( origName );
		}

	return 1;
	// MLivingstone END
}


void vtkTimeThreshold::Convert()
{
  cout << this->UpperChar << " : " << this->LowerChar << endl;
  TimeConverter timeC = TimeConverter();
  timeC.Parse(this->UpperChar);      
  switch(this->TimeFormat)
    { 
    case VTK_MSDATE:
      this->UpperThreshold = timeC.GetMSDate();
      timeC.Parse(this->LowerChar);
      this->LowerThreshold = timeC.GetMSDate();
      break;    
    case VTK_JULIAN:
      this->UpperThreshold = timeC.GetJulianDate();
      timeC.Parse(this->LowerChar);                  
      this->LowerThreshold = timeC.GetJulianDate();        
      break;
    case VTK_JULIAN_M:                  
      this->UpperThreshold = timeC.GetModifiedJulianDate();        
      timeC.Parse(this->LowerChar);
      this->LowerThreshold = timeC.GetModifiedJulianDate();        
      break;
      case VTK_MATLAB:                  
      this->UpperThreshold = timeC.GetMatlabDate();        
      timeC.Parse(this->LowerChar);
      this->LowerThreshold = timeC.GetMatlabDate();    
      break;      
    }                
}
int vtkTimeThreshold::FillInputPortInformation(int, vtkInformation *info)
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataSet");
  return 1;
}

void vtkTimeThreshold::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
