#include "vtkPDataMineReader.h"
#include "vtkInformationVector.h"
#include "vtkInformation.h"
#include "vtkObjectFactory.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkMultiProcessController.h"
#include "vtkPolyData.h"
#include "vtkCellArray.h"
#include "vtkPoints.h"
#include "vtkDoubleArray.h"
#include "vtkStringArray.h"
#include "vtkDataArraySelection.h"
#include "vtkCleanPolyData.h"
#include "vtkStdString.h"
#include "vtkCallbackCommand.h"
#include <vtksys/ios/sstream>
#include "vtkXMLUtilities.h"
#include "vtkXMLDataElement.h"
#include "vtkCollectPolyData.h"
#include <vtksys/RegularExpression.hxx>
#include "vtkToolkits.h" 
//#include "vtkCleanCells.h"


#ifdef VTK_USE_MPI
#include "vtkMPICommunicator.h"
class vtkMPIController;
#endif


vtkCxxRevisionMacro(vtkPDataMineReader, "$Revision: 0.3 $");
vtkStandardNewMacro(vtkPDataMineReader);
vtkCxxSetObjectMacro(vtkPDataMineReader, Controller, vtkMultiProcessController);


// ---------------------------------------- Utility functions -------------------------------------------------
void VISswap_4_byte_ptr1(char* ptr)
{
   char tmp;
   for (int si=0; si<2; si++) {
     tmp = ptr[si];
     ptr[0] = ptr[7-si];
     ptr[7-si] = tmp;
   }
}

/***************************************************************************************/
void VISswap_8_byte_ptr1(char* ptr)
{
   char tmp;
   for (int si=0; si<4; si++) {
     tmp = ptr[si];
     ptr[0] = ptr[7-si];
     ptr[7-si] = tmp;
   }
}

/***************************************************************************************/
vtkPDataMineReader::vtkPDataMineReader()
  {
   this->GhostLevel = 0;  
   this->NumberOfPieces =0;
	 this->NbPieces=2; 
   this->PathName = 0; 
   this->FileName = NULL;
	 this->ConfigurationFile = NULL;
	 this->PieceFileNames = NULL;
   this->SetNumberOfInputPorts(0);

	 this->FilePattern ="%s_%d.vtk";   
  
   this->Controller = vtkMultiProcessController::GetGlobalController();
   if (this->Controller)
    {
    this->Controller->Register(this);
    } 

   this->PropertyCount = 0;  
	 this->CellDataArraySelection = vtkDataArraySelection::New();
  // Setup the selection callback to modify this object when an array
  // selection is changed.
  this->SelectionObserver = vtkCallbackCommand::New();
  this->SelectionObserver->SetCallback(&vtkPDataMineReader::SelectionModifiedCallback);
  this->SelectionObserver->SetClientData(this);
  this->CellDataArraySelection->AddObserver(vtkCommand::ModifiedEvent,this->SelectionObserver); 

  }

/***************************************************************************************/
vtkPDataMineReader::~vtkPDataMineReader()
  {		
  if (this->Controller)
    {
    this->Controller->Delete();
    this->Controller = 0;
    }

	this->SetFileName( 0 );
	 //deleting object variables
  if (this->CellDataArraySelection!=NULL)
    {
	  this->CellDataArraySelection->RemoveObserver(this->SelectionObserver);
	  this->CellDataArraySelection->Delete();
    }
  this->SelectionObserver->Delete();
  }

/***************************************************************************************/
void vtkPDataMineReader::SetNumberOfPieces(int num)
{
  int i;

  if (this->NumberOfPieces == num)
    return;
    	
  // Delete the previous file names/extents.
  for (i = 0; i < this->NumberOfPieces; ++i)
    {
    if (this->PieceFileNames[i])
      {
      delete [] this->PieceFileNames[i];
      this->PieceFileNames[i] = NULL;
      }
    }
  if (this->PieceFileNames)
    {
    delete [] this->PieceFileNames;
    this->PieceFileNames = NULL;
    }
  this->NumberOfPieces = 0;

	if (num <= 0)
    return;
    

  // Allocate new arrays
  this->PieceFileNames = new char*[num];
  for (i = 0; i < num; ++i)
    this->PieceFileNames[i] = new char[512];
     
	this->NumberOfPieces = num;
}


// --------------------------------------
int vtkPDataMineReader::CanRead( const char* fname, FileTypes type )
{
	//okay we have to un constant the char* so that TDMFile can use it
	//TDMFile does not modify the fname so this is 'okay'
	if (fname == NULL || fname == "" || fname == " ")
		return 0;
	char *tmpName = const_cast<char *>(fname);
	
	//load the File
	TDMFile* dmFile = new TDMFile();
  dmFile->LoadFileHeader(tmpName);
	
	// Get File Type	
  FileTypes filetype = dmFile->GetFileType();
  
	//check the type of the file, since we only support wireframes ( points / triangles )
	int result = ( filetype == type );
  
  delete dmFile; 
  return result;  
}


/***************************************************************************************/
void vtkPDataMineReader::ChangeFileNameFromBuf(char *buf, char* fname)
{
	 this->SplitFileName();
	 int length1 = (int)strlen(fname)-3;
	 int length2 = (int)strlen(this->PathName);

	 char* tempFileName= new char[length1-length2+2];

	 for(int i=length2; i<=length1;i++)
	 tempFileName[i-length2]= fname[i];

	 tempFileName[length1-length2+1]='\0';
	 strupr(tempFileName);

	 memcpy((buf+(0*WordSize)),tempFileName,(2*SIZE_OF_WORD)+2); 
	 delete tempFileName;
}

/***************************************************************************************/
void vtkPDataMineReader::ChangeNLastPageRecsFromBuf(char *buf, int n, bool ByteSwapped)
{
	 float f;
   double d;

	 if (FMT64) d= (double)n;
	 else f= (float)n;
  
	 if (ByteSwapped) {
		 if (FMT64)  VISswap_8_byte_ptr1((char*)&d);
			else  VISswap_4_byte_ptr1((char*)&f);
	 }

   if (FMT64)  memcpy((buf+(27*WordSize)), &d, SIZE_OF_DOUBLE);
   else  memcpy((buf+(27*WordSize)), &f, SIZE_OF_FLOAT);	 
}

/***************************************************************************************/
void vtkPDataMineReader::ChangeNPhysicalPagesFromBuf(char *buf, int n, bool ByteSwapped)
{
	 
	 float f;
   double d;

	 if (FMT64) d= (double)n;
	 else f= (float)n;	

   if (ByteSwapped) {
		 if (FMT64)  VISswap_8_byte_ptr1((char*)&d);
			else  VISswap_4_byte_ptr1((char*)&f);
	 }

   if (FMT64)  memcpy((buf+(26*WordSize)), &d, SIZE_OF_DOUBLE);
   else  memcpy((buf+(26*WordSize)), &f, SIZE_OF_FLOAT);


}

/***************************************************************************************/
void vtkPDataMineReader::SplitFileName()
{
  if(!this->FileName)
    {
    vtkErrorMacro( << "Need to specify a filename" );
    return;
    }

  // Pull the PathName component out of the FileName.
  size_t length = strlen(this->FileName);
  char* fileName = new char[length+1];
  strcpy(fileName, this->FileName);
  char* begin = fileName;
  char* end = fileName + length;
  char* s;
  
#if defined(_WIN32)
  // Convert to UNIX-style slashes.
  for(s=begin;s != end;++s) { if(*s == '\\') { *s = '/'; } }
#endif
  
  // Extract the path name up to the last '/'.
  if(this->PathName) 
    { 
    delete [] this->PathName; 
    this->PathName = 0; 
    }
  char* rbegin = end-1;
  char* rend = begin-1;
  for(s=rbegin;s != rend;--s) 
    { 
    if(*s == '/') 
      { 
      break; 
      } 
    }
  if(s >= begin)
    {
    length = (s-begin)+1;
    this->PathName = new char[length+1];
    strncpy(this->PathName, this->FileName, length);
    this->PathName[length] = '\0';
    }
  
  // Cleanup temporary name.
  delete [] fileName;
}

/***************************************************************************************/
bool vtkPDataMineReader::WriteFileHeader(char *fname)
{
	 int rdsz,wdsz;;  
	 TDMFile *file = new TDMFile();
   file->LoadFileHeader( fname );
	 bool ByteSwap= file->GetByteSwapped();

	 this->FMT64= false ; 
   this->BufferSize = SIZE_OF_BUFFER;  // default to 32 bit size.
   this->WordSize = SIZE_OF_WORD;  // default to 32 bit size.

   FILE *in;
   char buf[SIZE_OF_BUFF64];
   if ((in = fopen(fname,"rb")) == NULL)
     return false;
   
   if ((rdsz=(int)fread(buf,sizeof(char),this->BufferSize,in)) != this->BufferSize)
   {
      if (in) fclose(in);
      return false;
   }

   double* dpp = (double*)&buf[24*8];  // pointer to 25th 64-bit word.
   if (*dpp==456789.0) { 
     // read the second half of the FMT64 page.
     fread(&buf[this->BufferSize],sizeof(char),this->BufferSize,in);
     this->BufferSize = SIZE_OF_BUFF64;  // set 64 bit buffer size.
     this->WordSize = SIZE_OF_WORD64;
		 this->FMT64= true;
   }

   int start=0,end=0,n=0; 
	 int ldrl, nrpp;
	 int NPPages;
	 //this->ChangeFileNameFromBuf(buf,this->PieceFileNames[p]); 
	 // number of pages minus header page
   NPPages = file->GetNPhysicalPages()-1;
	 char temp[SIZE_OF_BUFF64];
	 strcpy(temp,buf);
  
	 for(int p=0; p< this->NbPieces; p++)
	 {
		 strcpy(buf,temp);		
     start= (p*NPPages)/this->NbPieces;
		 end= ((p+1)*NPPages)/this->NbPieces;
		 n= end-start;

		 this->ChangeNPhysicalPagesFromBuf(buf, (n+1),ByteSwap);

		 if(p==(this->NbPieces-1))
		 	 nrpp= file->GetNLastPageRecs();
		 else
		 {
			 ldrl = file->GetLogicalDataRecLen();  // record length
			 nrpp = 508/ldrl;  // number of records per page
	   }
   	 this->ChangeNLastPageRecsFromBuf(buf, nrpp, ByteSwap);
	
		 FILE *out;
		 if ((out = fopen(this->PieceFileNames[p],"wb")) == NULL)
		 	 return false;
		 		 
		 if((wdsz=(int)fwrite(buf,sizeof(char),BufferSize,out)) != BufferSize)
		 {
			 if (out) fclose(out);
				return false;
		 }			
		 fclose(out);
	 }
   fclose(in);

   delete file;

	 return true;
}

/***************************************************************************************/
int vtkPDataMineReader::WriteDrillHolesPages(char *fname)
{
	TDMFile *file = new TDMFile();
  file->LoadFileHeader( fname );
	int recordLength = file->nVars;
	int numRecords = file->GetNumberOfRecords( );
	Data *values = new Data[file->nVars];
	int BHID =-1, idSize=0;

	char *varname = new char[256]; //make it really large so we don't run the bounds
  for (int i=0; i<recordLength; i++)
		{
		file->Vars[i].GetName(varname);		
		if (strncmp(varname,"BHID",4)==0)
			{
			if ( BHID == -1 )
				{
				BHID = i;
				}
			idSize++;
			}		
		}

	int cellCount = 0;
	bool BHIDChanged = false;
	double currentCellNumber = 0;

	double *prevCellNumbers = new double[idSize];
	for (int i=0; i < idSize; i++)
		prevCellNumbers[i] = -1;

	file->OpenRecVarFile( fname );
	for (int i=0; i < numRecords; i++)
		{	
		file->GetRecVars( i, values );			
		//need to see if the string or int id has changed
		for (int j=0; j < idSize; j++)
			{
			  currentCellNumber = values[BHID+j].v;
			  if ( prevCellNumbers[j] != currentCellNumber )
				{
				//check for changes, and update prevCellNumbers at the same time
				BHIDChanged = true;
				prevCellNumbers[j] = currentCellNumber;
				}
			} 						
		if ( BHIDChanged )
			{
			BHIDChanged = false;			
			cellCount++;		
			}
	  }
	file->CloseRecVarFile( );


  
	FILE *in, *out;
	int px=0, wdsz, rdsz, count;  
  char buf[SIZE_OF_BUFF64], bufHeader[SIZE_OF_BUFF64], temp[SIZE_OF_BUFF64];
  int start=0, end=0, nbPxCells=0, currentCell=0, recordCount=0;

	for (int i=0; i < idSize; i++)
		prevCellNumbers[i] = -1;

	if ((in = fopen(fname,"rb")) == NULL)
    return false;

   if ((rdsz=(int)fread(bufHeader,sizeof(char),this->BufferSize,in)) != this->BufferSize)
		{
    if (in) fclose(in);
    return false;
		}
		strcpy(temp,bufHeader);

	for(px=0; px<this->NbPieces;px++)
	{
		// strcpy(bufHeader,temp); //because in this case i want to write the hearder information in this function

		 start= (int)((cellCount*px)/this->NbPieces);
		 end=  (int)((cellCount*(px+1))/this->NbPieces);
		 nbPxCells= end-start;
		 currentCell=0;
		 BHIDChanged=false;
		 count=0;
		 if ((out = fopen(this->PieceFileNames[px],"ab")) == NULL)
				 return 0;
		 while(currentCell<nbPxCells)
		 {
				if ((rdsz=(int)fread(buf,sizeof(char), this->BufferSize,in)) != this->BufferSize )
					{
					 if (in) fclose(in);
					 return 0;          
					}  		
				if((wdsz=(int)fwrite(buf,sizeof(char), this->BufferSize,out)) != this->BufferSize)
					{
					if (out) fclose(out);	
					return 0;
					}
				count++;
				file->GetRecVars( recordCount, values );			
			//need to see if the string or int id has changed
			for (int j=0; j < idSize; j++)
				{
					currentCellNumber = values[BHID+j].v;
					if ( prevCellNumbers[j] != currentCellNumber )
					{
					//check for changes, and update prevCellNumbers at the same time
					BHIDChanged = true;
					prevCellNumbers[j] = currentCellNumber;
					}
				} 						
			if ( BHIDChanged )
				{
				BHIDChanged = false;			
				currentCell++;		
				}

			recordCount++;
		 }

		 //this->ChangeNPhysicalPagesFromBuf(bufHeader, (count+1),ByteSwap);
		 fclose(out);	

	}
	fclose(in);	
 delete file;
 return 1;

}
	
/***************************************************************************************/
bool vtkPDataMineReader::WriteFilePages(char *fname)
{
	TDMFile *file = new TDMFile();
  file->LoadFileHeader( fname );

	// number of pages minus header page
  int numOfPages = file->GetNPhysicalPages() - 1; 
	int sizeOfRecord= 
 
  FILE *in, *out;
	int px=0, wdsz, rdsz;  
  char buf[SIZE_OF_BUFF64];
 
	if ((in = fopen(fname,"rb")) == NULL)
    return false;
   
   if ((rdsz=(int)fread(buf,sizeof(char),this->BufferSize,in)) != this->BufferSize)
   {
      if (in) fclose(in);
      return false;
   }

	 int start=0, end=0;
	 for(px=0; px<this->NbPieces;px++)
	 {
		 start= (int)((numOfPages*px)/this->NbPieces);
		 end=  (int)((numOfPages*(px+1))/this->NbPieces);
		 if ((out = fopen(this->PieceFileNames[px],"ab")) == NULL)
			 return false;
		 for (int p = start; p <end; p++)
		 {	
				if ((rdsz=(int)fread(buf,sizeof(char), this->BufferSize,in)) != this->BufferSize )
				{
					 if (in) fclose(in);
						return false;          
				}  		
				if((wdsz=(int)fwrite(buf,sizeof(char), this->BufferSize,out)) != this->BufferSize)
				{
				  if (out) fclose(out);	
				  return false;
				}
		 }
		 fclose(out);				 
	 }

	 fclose(in);  
	 delete file;
	 return true;
}


/***************************************************************************************/
int vtkPDataMineReader::CreateDataMinePieces()
{	
	this->WriteFileHeader(this->FileName);	
/*
	if(this->CanRead(this->FileName, drillhole)==1)
	 {
		 this->WriteDrillHolesPages(this->FileName);
	 }
	else*/ this->WriteFilePages(this->FileName);

	return 1;
}


/***************************************************************************************/
int vtkPDataMineReader::CreatePieceFileNames(int numPieces)
{
	int length = (int)strlen(this->FileName);
	char* tempFileName= new char[length-3];
	strncpy(tempFileName,this->FileName, (length-4));	
	tempFileName[length-4]='\0';

	this->FilePattern ="%s_%d.dm";  
	FILE *in;
	for(int idx=0; idx<numPieces; idx++)
	{
		this->PieceFileNames[idx]=new char[length+strlen(this->FilePattern)+20];
		sprintf(this->PieceFileNames[idx], this->FilePattern, tempFileName, idx);

		if((in= fopen(this->PieceFileNames[idx],"wb"))==NULL)
			return 0;		
		if(in) fclose(in);	
	}
  delete tempFileName;
	return 1;		
}

int vtkPDataMineReader::ReadConfigurationsFile()
{
	vtkXMLDataElement *configElement = vtkXMLUtilities::ReadElementFromFile(this->ConfigurationFile);
	vtkXMLDataElement *element;  

	element = configElement->LookupElementWithName("PDataArray");
	if(element != NULL)
	{
		int value = 0;
		int result = element->GetScalarAttribute("NumberOfComponents",value);
		if(result <= 0)
		{
			vtkErrorMacro("The Number Of Components is not valid...Aborting");
			return 0;
		}
		else{
			this->NbPieces = value;
			this->SetNumberOfPieces(this->NbPieces); 
		}	
	}

	if(this->NbPieces>0)
	{
		char* pattern= new char[10];
		pattern= "%s%d"; 
		char* filename = new char[512];

		for(int p=0; p<this->NbPieces; p++)
		{
			sprintf(filename, pattern, "Piece", p);
			element = configElement->LookupElementWithName(filename);
			if(element != NULL)
			{
				const char* value = element->GetAttributeValue(0);
				strcpy(this->PieceFileNames[p], value);
				
				cout<< this->PieceFileNames[p] <<endl;
			}
			else {
				vtkErrorMacro("The source file "<<p<<"is missing...Aborting");
				return 0;
			}
		}
	}	

	return 1;
}
/***************************************************************************************/
char* vtkPDataMineReader::GetPieceFileName(int Piec)
{
	int length = (int)strlen(this->FileName);
	char* tempFileName= new char[length-3];
	strncpy(tempFileName,this->FileName, (length-4));	
	tempFileName[length-4]='\0';

	this->FilePattern ="%s_%d.dm";  
  char* name= new char[512];
	sprintf(name, this->FilePattern, tempFileName, Piec);
  delete tempFileName;
	return name;
}

/***************************************************************************************/
int vtkPDataMineReader::RequestInformation(vtkInformation* request,
                          vtkInformationVector** inputVector,
                          vtkInformationVector* outputVector)
{
	//update property list
	this->UpdateDataSelection();

  vtkInformation *outInfo=outputVector->GetInformationObject(0);
  outInfo->Set(vtkStreamingDemandDrivenPipeline::MAXIMUM_NUMBER_OF_PIECES(),-1);

	return 1;	
}
/***************************************************************************************/
int vtkPDataMineReader::RequestData(vtkInformation* request,
                          vtkInformationVector** inputVector,
                          vtkInformationVector* outputVector)
{ 
	vtkInformation *outInfo = outputVector->GetInformationObject(0);  
  vtkPolyData *output = vtkPolyData::SafeDownCast( outInfo->Get(vtkDataObject::DATA_OBJECT()));
  

	if (!this->Controller || this->Controller->GetNumberOfProcesses()<= 1)
	{	
		if( strlen(this->ConfigurationFile)<=1 )
		 {
			 this->SetNumberOfPieces(this->NbPieces);
			 this->CreatePieceFileNames(this->NbPieces);
			 this->CreateDataMinePieces();
		 }
		 else {
			 if(this->ReadConfigurationsFile()==0)
				{
					vtkErrorMacro("The configuration file is not valid or source files are missing...Aborting");
				}		
		 }
		 this->ReadShrunks(0, this->NbPieces, output);	

		return 1;
	} 

#ifndef VTK_USE_MPI

	 if( strlen(this->ConfigurationFile)<=1 )
	 {
		this->SetNumberOfPieces(this->NbPieces);
		this->CreatePieceFileNames(this->NbPieces);
		this->CreateDataMinePieces();	
	 }
	 else{
		  //we have all shrunks files already so we just need to distribute them on the processes
			if(this->ReadConfigurationsFile()==0)
			{
				vtkErrorMacro("The configuration file is not valid or source files are missing...Aborting");
			}		
	 }
	 this->ReadShrunks(0, this->NbPieces, output);

	 return 1;

#else

	this->NumProcesses = this->Controller->GetNumberOfProcesses();
	this->MyId    = this->Controller->GetLocalProcessId();
	
	if(this->NumProcesses>1) 
	{
		int metaData[1];
		metaData[0]=0;				
			
		if(this->MyId==0)
		{
			 if( strlen(this->ConfigurationFile)<=1 )
			 {		
				  this->SetNumberOfPieces(this->NbPieces);
					this->CreatePieceFileNames(this->NbPieces);			
					this->CreateDataMinePieces();		
					metaData[0]=1;
			 }
			 else
			 {
					if(this->ReadConfigurationsFile()==0)
					{
						vtkErrorMacro("The configuration file is not valid or source files are missing...Aborting");
						return 0;
					}		
					else metaData[0]=1;
			 }
		}			

		if( strlen(this->ConfigurationFile)>1 )
		{
			int n[1];

			if(this->MyId!=0) n[0]=0;
			else n[0]= this->NbPieces;
			this->Controller->Broadcast(n, 1, 0);
			this->NbPieces= n[0];
		}

		if(this->MyId!=0)
			this->SetNumberOfPieces(this->NbPieces);

		if( (strlen(this->ConfigurationFile)<=1)&&(this->MyId!=0))
		{
			for(int p=0; p<this->NbPieces; p++)
				strcpy(this->PieceFileNames[p],this->GetPieceFileName(p));					
		}

		if(strlen(this->ConfigurationFile)>1){
			for(int p=0; p<this->NbPieces; p++)
				this->Controller->Broadcast(this->PieceFileNames[p],strlen(this->PieceFileNames[p]), 0);
		}

		this->Controller->Broadcast(metaData, 1, 0);

    this->StartPiece= (this->MyId * this->NbPieces)/ this->NumProcesses;
		this->EndPiece=   (( this->MyId+1) * this->NbPieces)/ this->NumProcesses;

		if(metaData[0]==1) 
			this->ReadShrunks(this->StartPiece, this->EndPiece, output);				
	}			
	return 1;

#endif

}

// --------------------------------------
void vtkPDataMineReader::UpdateDataSelection()
{		      
	//first step is to grab from the file the header information
	TDMFile* dmFile = new TDMFile();
  
	//properties are located on the triangles
	if (!dmFile->LoadFileHeader( this->GetFileName() ) )
		{
		return;
		}
			
	char *varname = new char[256]; 
	this->PropertyCount = dmFile->nVars;		     
	for (int i=0; i<dmFile->nVars; i++)
		{
		dmFile->Vars[i].GetName(varname);
		if (!this->CellDataArraySelection->ArrayExists(varname))
			{		
			this->CellDataArraySelection->AddArray(varname);
			this->CellDataArraySelection->DisableArray(varname);		
			}
		}
  
	delete[] varname;
	delete dmFile;
	this->SetupOutputInformation( this->GetOutputPortInformation(0) );		
	
}

//----------------------------------------------------------------------------
void vtkPDataMineReader::SelectionModifiedCallback(vtkObject*, unsigned long,
                                             void* clientdata, void*)
  {
  static_cast<vtkPDataMineReader*>(clientdata)->Modified();
  }

// Called from within EnableAllArrays()
void vtkPDataMineReader::SetCellArrayStatus(const char* name, int status)
  {
  if(status)
    this->CellDataArraySelection->EnableArray(name);
  else
    this->CellDataArraySelection->DisableArray(name);
  }

//----------------------------------------------------------------------------
// Modified FROM vtkXMLReader.cxx
int vtkPDataMineReader::SetFieldDataInfo(vtkDataArraySelection* CellDAS, 
                                   int association, int numTuples,
                                   vtkInformationVector *(&infoVector))
{
  if (!CellDAS)  
    {
    return 1;
    }
  
  int i, activeFlag;
  const char *name;

  vtkInformation *info = NULL;

  if (!infoVector)  
    {
    infoVector = vtkInformationVector::New();
    }

  /** Part 2 - process data for each array/property **/
  // Cycle through each data array - CellDAS entry.
	activeFlag = 0;
	for (i=0; i<CellDAS->GetNumberOfArrays(); i++) 
    {
    info = vtkInformation::New();
    
    info->Set(vtkDataObject::FIELD_ASSOCIATION(), association);
    info->Set(vtkDataObject::FIELD_NUMBER_OF_TUPLES(), numTuples);

    name = CellDAS->GetArrayName( i );
    info->Set(vtkDataObject::FIELD_NAME(), name);
    info->Set(vtkDataObject::FIELD_ARRAY_TYPE(), 1);
		info->Set(vtkDataObject::FIELD_NUMBER_OF_COMPONENTS(), 1);

		activeFlag |= 1 << i;
    info->Set(vtkDataObject::FIELD_ACTIVE_ATTRIBUTE(), activeFlag);
    infoVector->Append( info );
    info->Delete();
    }
  return 1;
  }

//----------------------------------------------------------------------------
// Modified FROM vtkXMLDataReader.cxx
void vtkPDataMineReader::SetupOutputInformation(vtkInformation *outInfo)
  {
	// CellDataArraySelection is already prepared.  Don't need SetDataArraySelection()
  vtkInformationVector *infoVector = NULL;
  // Setup the Field Information for the Cell data
  if (!this->SetFieldDataInfo(this->CellDataArraySelection,
                              vtkDataObject::FIELD_ASSOCIATION_CELLS, 
                              this->PropertyCount, 
                              infoVector))
    {
    vtkErrorMacro("Error return from SetFieldDataInfo().");
    return;
    }
  if (infoVector)
    {
    outInfo->Set(vtkDataObject::CELL_DATA_VECTOR(), infoVector);
    infoVector->Delete();
    }
	else 
    {
    vtkErrorMacro("Error infoVector NOT SET IN outInfo.");
	  }
  }
//----------------------------------------------------------------------------
int vtkPDataMineReader::GetCellArrayStatus(const char* name)
  {
	// if 'name' not found, it is treated as 'disabled'
  return (this->CellDataArraySelection->ArrayIsEnabled(name));
  }
//----------------------------------------------------------------------------
int vtkPDataMineReader::GetNumberOfCellArrays()
  {
  return this->CellDataArraySelection->GetNumberOfArrays();
  }
//----------------------------------------------------------------------------
const char* vtkPDataMineReader::GetCellArrayName(int index)
  {
  return this->CellDataArraySelection->GetArrayName(index);
  }
/***************************************************************************************/
void vtkPDataMineReader::PrintSelf(ostream& os, vtkIndent indent)
  {
  this->Superclass::PrintSelf(os,indent);
  os << indent << "FileName: "
     << (this->FileName ? this->FileName : "(none)") << "\n";
  }
  