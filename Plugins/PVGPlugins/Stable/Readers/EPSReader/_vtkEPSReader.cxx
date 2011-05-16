#include "vtkEPSReader.h"

#include "vtkDataArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkOnePieceExtentTranslator.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkPythonProgrammableFilter.h"
#include "vtkDoubleArray.h"
#include "vtkStringArray.h"

vtkCxxRevisionMacro(vtkEPSReader, "$Revision: 3.18 $");
vtkStandardNewMacro(vtkEPSReader);

// Constructor
vtkEPSReader::vtkEPSReader()
  {
  this->SetExecuteMethod(vtkEPSReader::ExecuteScript, this);
  this->OutputDataSetType = VTK_POLY_DATA;
  
  this->FileName = NULL;
  this->Date = NULL;  

  //work arounds for the problem of the python code can't return info
  this->PyDate = vtkStringArray::New();
  this->PyDate->SetNumberOfValues(2);
  
  this->PyPointFile = vtkStringArray::New();
  this->PyPointFile->SetNumberOfValues(1);
   
  this->PyOffset = vtkDoubleArray::New();
  this->PyOffset->SetNumberOfValues(1);
  
  //now we set this up with the proper files
  vtkStdString requestFile = PYTHON_FILE;
  requestFile += "EPSRequestData.py";
  
  //we should be able to do this a better way once we can figure out how vtkPythonPro...Filter actually does its python binding

  vtkStdString requestData;
  vtkstd::fstream file(requestFile.c_str());
  vtkstd::getline(file, requestData, file.widen('\255'));
  
  this->SetScript( requestData.c_str() );  
  
  //now we set this up with the proper files
  vtkStdString requestInfoFile = PYTHON_FILE;
  requestInfoFile += "EPSRequestInfo.py";

  vtkStdString requestInfo;
  vtkstd::fstream file2(requestInfoFile.c_str());
  vtkstd::getline(file2, requestInfo, file2.widen('\255'));
  
  this->SetInformationScript( requestInfo.c_str() );
  
  file.close();
  file2.close();
  }

// --------------------------------------
// Destructor
vtkEPSReader::~vtkEPSReader()
  {
  this->SetFileName(0);
  this->SetDate(0);
  
  this->PyOffset->Delete();
  this->PyDate->Delete();
  this->PyPointFile->Delete();
  }

//----------------------------------------------------------------------------
int vtkEPSReader::RequestInformation(
  vtkInformation*, 
  vtkInformationVector**, 
  vtkInformationVector* outputVector)
{
 
  int result = vtkPythonProgrammableFilter::RequestInformation(NULL,NULL,outputVector);        
  if (this->Date)
	{
	this->PyDate->SetValue(1, this->GetDate() );
	}
    
  return result;
}

//----------------------------------------------------------------------------
vtkStdString  vtkEPSReader::CreateReference()
{


//this needs to be cleaned up
  vtkStdString initscript = "FILENAME = '";
  initscript += this->GetFileName();
  initscript +="'\n";
      
  initscript+="from paraview import vtk\n";  
  // Set self to point to this
  char addrofthis[1024];
  sprintf(addrofthis, "%p", this);    
  char *aplus = addrofthis; 
  if ((addrofthis[0] == '0') && 
      ((addrofthis[1] == 'x') || addrofthis[1] == 'X'))
    {
    aplus += 2; //skip over "0x"
    }
  initscript += "_progfilter = vtk.vtkProgrammableFilter('";
  initscript += aplus;
  initscript += "')\n";  
 
  //set the memory references for the offset, Date, and PointFileName
  char addrofOffset[1024];
  sprintf(addrofOffset, "%p", this->PyOffset);
  char *memref = addrofOffset;
  if ((addrofOffset[0] == '0') && 
      ((addrofOffset[1] == 'x') || addrofOffset[1] == 'X'))
    {
    memref += 2; //skip over "0x"
    }
  initscript += "_Offset = vtk.vtkDoubleArray('";
  initscript += memref;
  initscript += "')\n";
  
  char addrofDate[1024];
  sprintf(addrofDate, "%p", this->PyDate);
  memref = addrofDate;
  if ((addrofDate[0] == '0') && 
       ((addrofDate[1] == 'x') || addrofDate[1] == 'X'))
     {
     memref += 2; //skip over "0x"
     }
  initscript += "_Date= vtk.vtkStringArray('";
  initscript += memref;
  initscript += "')\n";
   
  char addrofFile[1024];
  sprintf(addrofFile, "%p", this->PyPointFile);
  memref = addrofFile;
  if ((addrofFile[0] == '0') && 
      ((addrofFile[1] == 'x') || addrofFile[1] == 'X'))
    {
    memref += 2; //skip over "0x"
    }
  initscript += "_PointFile = vtk.vtkStringArray('";
  initscript += memref;
  initscript += "')\n"; 
  return initscript; 
}

// --------------------------------------
void vtkEPSReader::PrintSelf(ostream& os, vtkIndent indent)
  {
  this->Superclass::PrintSelf(os,indent);
  os << indent << "FileName: "
     << (this->FileName ? this->FileName : "(none)") << "\n";
  
  os <<indent << "Date:"
      <<(this->GetDate() ? this->GetDate() : "(none)") << "\n";
  }


