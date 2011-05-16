/*=========================================================================
  Program:   Visualization Toolkit
  Module:    $ vtkFitnessCalculator.cxx $
  Authors:    Nehme Bilal & Arolde VIDJINNAGNI
  MIRARCO, Laurentian University
  Date:      
  Version:   0.1
=========================================================================*/

#include "vtkFitnessCalculator.h"

#include "vtkPointSet.h"
#include "vtkPoints.h"
#include "vtkKdTree.h"
#include "vtkKdNode.h"
#include "vtkDataSet.h"
#include "vtkPolyData.h"
#include "vtkPolyDataReader.h"
#include "vtkPolyDataMapper.h"

#include "vtkDoubleArray.h"
#include "vtkPointData.h"
#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkPolyDataSource.h"
#include "vtkPointData.h"
#include "vtkDataArray.h"	
#include "vtkDoubleArray.h"
#include "vtkMath.h"
#include "vtkIdTypeArray.h"
#include "vtkIdList.h"
#include "vtkCleanUnstructuredGrid.h"
#include "vtkLookupTable.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkKdTree.h"
#include <sstream>
#include "vtkStdString.h"

#include "vtkPythonProgrammableFilter.h"
#include <QtDebug>
#include <set>

struct ABRI_Internals
{
	vtkstd::set<vtkStdString> arrays;
};

vtkCxxRevisionMacro ( vtkFitnessCalculator, "$Revision: 0.3 $" );
vtkStandardNewMacro ( vtkFitnessCalculator );

//========================================================================================
vtkFitnessCalculator::vtkFitnessCalculator(void)
{
	this->Internal = new ABRI_Internals;
	this->geotechProp = 0;
	this->densityInput = NULL;
	this->pythonPath = NULL;
	this->requestData = NULL;
	this->helperFile = NULL;
	clearArray = true;
	this->Internal->arrays.insert("-");

	BlockSizeX=5;
	BlockSizeY=5;
	BlockSizeZ=5;

	this->AccessPosition = 0;

	this->modified = 0;
	this->numberOfFunctions = 0;
	this->RegularBlock=0;
}


//========================================================================================
vtkFitnessCalculator::~vtkFitnessCalculator(void)
{		
}


//----------------------------------------------------------------------------
int vtkFitnessCalculator::FillInputPortInformation ( int vtkNotUsed(port), vtkInformation* info )
  {
  info->Set ( vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData", 0 );
  return 1;
  }


//----------------------------------------------------------------------------
int vtkFitnessCalculator::FillOutputPortInformation(int vtkNotUsed(port), vtkInformation* info)
{
  info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkPolyData");
  return 1;
}


//--------------------------------------------------------------------------
int vtkFitnessCalculator::RequestData( vtkInformation *request, 
                         vtkInformationVector **InputVector, 
                         vtkInformationVector *outputVector )
{
	this->clearArray = true;
	this->numberOfFunctions = 0;


	vtkInformation *inputInfo = InputVector[0]->GetInformationObject ( 0 );
	vtkInformation *outInfo = outputVector->GetInformationObject ( 0 );

	this->input = vtkPolyData::SafeDownCast (
											 inputInfo->Get ( vtkDataObject::DATA_OBJECT() ) );

	this->output = vtkPolyData::SafeDownCast (
											outInfo->Get ( vtkDataObject::DATA_OBJECT() ) );


	if(this->Internal->arrays.size() == 0)
	{
		vtkWarningMacro("Please select at least one fitness function");
		return 1;
	}
	vtkStdString firstFunction = vtkStdString(*(this->Internal->arrays.begin()));
	if(firstFunction == "-")
	{
		vtkWarningMacro("Please select at least one fitness function");
		return 1;
	}


	

	vtkPythonProgrammableFilter *pythonFilter = vtkPythonProgrammableFilter::New();
	pythonFilter->SetInput(this->input);
	if(strlen(this->pythonPath))
	{
		pythonFilter->SetPythonPath(this->pythonPath);
	}
	else
	{
		vtkErrorMacro("The python Path is invalid");
		pythonFilter->Delete();
		return 0;
	}
	pythonFilter->SetScript(this->getPythonScript().c_str());
	pythonFilter->SetOutputDataSetType(VTK_POLY_DATA);

 
	

	//vtkStdString text = "\"" + vtkStdString(this->FitnessProperty) + "\"";
	//pythonFilter->SetParameter("inputAuArrayName", text);

	char str[40];
	
	sprintf(str,"%d",this->RegularBlock);
	pythonFilter->SetParameter("regularBlock", str);
	sprintf(str,"%f",this->BlockSizeX);
	pythonFilter->SetParameter("BlockSizeX", str );
	sprintf(str,"%f",this->BlockSizeY);
	pythonFilter->SetParameter("BlockSizeY", str );
	sprintf(str,"%f",this->BlockSizeZ);
	pythonFilter->SetParameter("BlockSizeZ", str );

	vtkStdString completePath("\"");
	completePath += vtkStdString(this->pythonPath) + "/" + vtkStdString(this->helperFile)+"\"";
	pythonFilter->SetParameter("helperFile", completePath.c_str());
	//pythonFilter->SetParameter("StopeSizeZ", ftoa(this->StopeSizeZ, str, 20));
	pythonFilter->SetParameter("geotechField", ("\"" + vtkStdString(this->geotechProp) + "\"").c_str());
	pythonFilter->SetParameter("densityInput", ("\"" + vtkStdString(this->densityInput) + "\"").c_str());

	sprintf(str,"%d",this->AccessPosition);
	pythonFilter->SetParameter("AccessPos", str);
	char variableName[40];
	const char* firstFunc = firstFunction.c_str();
	for(vtkstd::set<vtkStdString>::iterator it = this->Internal->arrays.begin() ; it != this->Internal->arrays.end() ; it++)
	{
		sprintf(variableName,"function%d",this->numberOfFunctions+1);
		vtkStdString functionName = vtkStdString(*it);
		const char* tmpStr = functionName.c_str();
		pythonFilter->SetParameter(variableName, ("\"" + vtkStdString(functionName) + "\"").c_str());
		//vtkWarningMacro(<<"variable: "<<variableName<<"    function: "<<tmpStr);
		this->numberOfFunctions++;
	}

	pythonFilter->Update();

	this->output->ShallowCopy(pythonFilter->GetOutput());	
	
	pythonFilter->Delete();
	
	this->output->GetPointData()->PassData(this->input->GetPointData());
	
	return 1;

}



//--------------------------------------------------------------------------
vtkStdString vtkFitnessCalculator::getPythonScript()
{
	ifstream file;
	vtkStdString scriptPath = vtkStdString(this->pythonPath) + "/" + vtkStdString(this->requestData);
	file.open(scriptPath, ios::in);
	if(!file)
	{
		vtkErrorMacro("File Error: cannot open file: "<< scriptPath.c_str());
		return 0;
	}
	
	vtkStdString script = "";
	vtkStdString line;
	while(!file.eof())
	{
		getline(file, line);
		
		script += line + "\n";
	}	

	return script;
}

void vtkFitnessCalculator::Setfunctions(const char *a)
{
	if(this->clearArray)
	{
		this->Internal->arrays.clear();
		this->clearArray = false;
	}
	this->Internal->arrays.insert(a);
}