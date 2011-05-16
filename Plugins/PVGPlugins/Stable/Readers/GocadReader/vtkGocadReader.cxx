/*=========================================================================

  Program:   ParaViewGeo
  Module:    $RCSfile: vtkGocadReader.cxx,v $

  Copyright (c) Robert Maynard
  All rights reserved.  
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkGocadReader.h"
#include "vtkGocadAtomic.h"
#include "vtkGocadTSurf.h"
#include "vtkGocadTSolid.h"
#include "vtkGocadPLine.h"
#include "vtkGocadVSet.h"
#include "vtkGocadVoxet.h"
#include "vtkGocadSGrid.h"

#include "vtkMultiBlockDataSet.h"
#include "vtkCompositeDataSet.h"
#include "vtkObjectFactory.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkSmartPointer.h"
#include "vtkPoints.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkStdString.h"

#include <vtkstd/stack>
#include <sstream>

#include "GocadColorMap.h"


vtkCxxRevisionMacro(vtkGocadReader, "$Revision: 0.1 $");
vtkStandardNewMacro(vtkGocadReader);

class vtkInternalGocadGroups
{
public:  
  vtkstd::stack< vtkSmartPointer< vtkMultiBlockDataSet> > groups;  
  vtkstd::stack< vtkStdString > groupNames;        
};


vtkGocadReader::vtkGocadReader()
{  

	this->ColorMap = new GocadColorMap();

  this->Line = new vtkStdString;
  this->FileName = 0;
  this->Internal = new vtkInternalGocadGroups;
	this->RGB = new double[4];
	this->RGB[0] = this->RGB[1] = this->RGB[2] = this->RGB[3] = 1;
     
  this->SetNumberOfInputPorts(0);
  this->SetNumberOfOutputPorts(1);
};

// --------------------------------------
vtkGocadReader::~vtkGocadReader()
{	
  this->SetFileName(0);
  delete this->Line;
  
  
  if (this->Internal)
    {
    delete this->Internal;
    }

	delete[] this->RGB;

	delete this->ColorMap;
}

// --------------------------------------
int vtkGocadReader::RequestData(vtkInformation* request,
                          vtkInformationVector** inputVector,
                          vtkInformationVector* outputVector)
{

  vtkInformation *outInfo = outputVector->GetInformationObject(0);  
  vtkMultiBlockDataSet *output = vtkMultiBlockDataSet::SafeDownCast( outInfo->Get(vtkDataObject::DATA_OBJECT()));
  
  
  //open the file
  bool fileGood = this->OpenFile( this->FileName );
  if (!this->File || !fileGood)
    {
    vtkErrorMacro("Failed to load the file");
    return 0;
    }  
  
  //read the file
  this->ReadFile( ); 
  
  //grab the output  
  output->DeepCopy( this->Internal->groups.top() );
  
  while( !this->Internal->groups.empty() )
    {
    this->Internal->groups.pop();
    this->Internal->groupNames.pop();
    }
      
  return 1;
  
  
}

// --------------------------------------
bool vtkGocadReader::ReadFile(  ) 
 {
  //read the entire file
  
  //need a parent group
  vtkStdString *name = new vtkStdString;
  *name = "GoCad";

  this->CreateBlock( name );    
  do
    {
    
    int objectType = this->ReadGObject( name );  
    
    switch (objectType)
      {    
      case CLOSE_GROUP:
        //found the end of a block, pop the block, add it to the top
        this->AppendBlock(); 
        break;
      case OPEN_GROUP:
        //we have a group!
        this->CreateBlock( name );
        break;        
      case TSURF:        
      case PLINE:
      case VSET:              
      case TSOLID:
			case VOXET:
			case SGRID:
        this->CreateObject( name, objectType );
        break; 
			case NOT_SUPPORTED:
				break;
      }   		
    }while( this->File->good() );      
   
  this->CloseFile();


	//cleanup
	delete name;

  return 1;
}



int vtkGocadReader::ReadGObject( vtkStdString *name )
  {    
  int mode = NOT_SUPPORTED;
  this->NextLine();  
      
  if ( this->Line->find("GOCAD") != vtkstd::string::npos )
    {
    //have a gocad header, figure out the mode    
    if ( this->Line->find("Group") != vtkstd::string::npos )
      {
      //found a HeterogeneousGroup  or a Homogeneous group
      mode = this->OPEN_GROUP;      
      this->ReadGroupHeader( name );
      return mode;
      }
    if ( this->Line->find("TSurf") != vtkstd::string::npos )
      {
      mode = this->TSURF;
      }
    else if ( this->Line->find("PLine") != vtkstd::string::npos )
      {
      mode = this->PLINE;
      }
    else if ( this->Line->find("VSet")  != vtkstd::string::npos )
      {
      mode = this->VSET;
      }
    else if ( this->Line->find("TSolid") != vtkstd::string::npos )
      {
      mode = this->TSOLID;
      }
		else if ( this->Line->find("Voxet") != vtkstd::string::npos )
      {
      mode = this->VOXET;
      }
		else if ( this->Line->find("SGrid") != vtkstd::string::npos )
      {
      mode = this->SGRID;
      }
    
    //not a group header, read a normal one
		if ( mode != NOT_SUPPORTED )
			{
			this->ReadHeader( name );
			}
		else if ( mode == NOT_SUPPORTED )
			{
			vtkErrorMacro("GoCad Object type not supported");
			}
		return mode;
    //now that we have read the GoCad line, we need read some more information
    }
  else if ( this->Line->find("END_MEMBERS") != vtkstd::string::npos )
    {
    //block was closed, return 0
    return this->CLOSE_GROUP;
    }
  //the header was not on the current line, return with a -1
  return -1;
}

// --------------------------------------
void vtkGocadReader::ReadHeader( vtkStdString *name )
{
  //we need to read the header from the Line "Header"
  //down to the closing }
  this->NextLine();  
	
	if ( this->Line->find("HDR") != vtkstd::string::npos )
		{
		while ( this->Line->find("HDR") != vtkstd::string::npos )
			{
			//okay we found a proper header continue
			this->ParseHeaderLine( name );
			}
		}
  else if ( this->Line->find("HEADER") != vtkstd::string::npos )
    {		
    //okay we found a proper header continue    
    while( this->Line->at(0) != '}' )
      {  			
			this->ParseHeaderLine( name );        
      }
    }
  else
    {		
    //no header!, use subsection of the file name for the object name!
    
    //time to parse the file name to get the last section of the name
		vtkStdString fName=this->FileName;
		
		//Find position final '\' that occurs just before the file name
		int slashPosition = fName.find_last_of('\\');
		//sometimes path contains the other slash ('/')
		if(slashPosition == -1)
			slashPosition = fName.find_last_of('/');
	
		//Add one to slashPosition so that the slash is not included
		slashPosition = slashPosition+1;

		//Find position of '.' that occurs before the file extension
		int dotPosition = fName.find_last_of('.');
				
	
		*name = fName.substr(slashPosition, slashPosition+dotPosition);
    }

}

// --------------------------------------
void vtkGocadReader::ParseHeaderLine( vtkStdString *name )
{	
	int namePos = 0;
	int colorPos = 0;
  const int NAME_LENGTH = 5;
	const int COLOR_LENGTH = 13;
  this->NextLine();
  
	//name detection
	namePos = this->Line->find("name:");
  if ( namePos != vtkstd::string::npos)
    {
    //okay store the name of the object        
    *name = this->Line->substr(namePos+NAME_LENGTH);                
    }

	colorPos = this->Line->find("*solid*color:");
	if ( colorPos != vtkstd::string::npos)
    {
    //okay time to parse the color into an rgb
		vtkstd::string color;
		color = this->Line->substr(colorPos+COLOR_LENGTH);                
		
		if ( color.find(".") == vtkstd::string::npos )
			{
			//have to use a table lookup to get the RGB
			this->ColorMap->GetColor(color, this->RGB);
			}
		else
			{
			//parse the rgb and alpha channel
			vtkstd::istringstream buffer(color, istringstream::in);
			double value;
			int i = 0;
			while ( buffer >> value )
				{					
				this->RGB[i++] = value;
				if ( i == 3)
					{
					//so we don't overrun the array
					break;
					}
				}
			}
		}
}
					
// --------------------------------------
void vtkGocadReader::ReadGroupHeader( vtkStdString *name )
{
  //we need to read the header from the Line "Header"
  //down to end of the group header, which is marked by BEGIN_MEMBERS
  this->NextLine();  
  if ( this->Line->find("HEADER") != vtkstd::string::npos )
    {
    //okay we found a proper header continue
    int namePos = 0;
    const int NAME_LENGTH = 5;
    while( this->Line->find("BEGIN_MEMBERS") == vtkstd::string::npos )
      {
      this->NextLine();
      namePos = this->Line->find("name:");      
      if ( namePos != vtkstd::string::npos)
        {
        //okay store the name of the object
        *name = this->Line->substr(namePos + NAME_LENGTH);
        }
      }
    }
  else
    {
    //no header!, use subsection of the file name for the object name!
    //time to parse the file name to get the last section of the name
		vtkStdString fName=this->FileName;
		
		//Find position final '\' that occurs just before the file name
		int slashPosition = fName.find_last_of('\\');
		//sometimes path contains the other slash ('/')
		if(slashPosition == -1)
			slashPosition = fName.find_last_of('/');
	
		//Add one to slashPosition so that the slash is not included
		slashPosition = slashPosition+1;

		//Find position of '.' that occurs before the file extension
		int dotPosition = fName.find_last_of('.');
				
	
		*name = fName.substr(slashPosition, slashPosition+dotPosition);
    }

}

// --------------------------------------
void vtkGocadReader::CreateBlock( vtkStdString *name )
{
  vtkMultiBlockDataSet *block = vtkMultiBlockDataSet::New();                             
  this->Internal->groups.push( block );
  this->Internal->groupNames.push( name->c_str() );        
  block->Delete();   
}

// --------------------------------------
void vtkGocadReader::CreateObject( vtkStdString *name, int type )
{
  vtkMultiBlockDataSet *block  = vtkMultiBlockDataSet::New();
  block->DeepCopy( this->Internal->groups.top() );    
  this->Internal->groups.pop();         
  int pos = block->GetNumberOfBlocks();
  
  //create the data type
   
  vtkGocadAtomic *data; 
   
  switch (type)
      {         
      case TSURF: 
        data = vtkGocadTSurf::New();        
        break;                
      case PLINE:
        data = vtkGocadPLine::New();        
        break;      
      case TSOLID:  
        data = vtkGocadTSolid::New();               
        break;
      case VSET:               
        data = vtkGocadVSet::New();                 
        break;
			case VOXET:
				data = vtkGocadVoxet::New();
				break;
			case SGRID:
				data = vtkGocadSGrid::New();
				break;
      }
  	
  data->SetFile( this->File );
	data->SetFilePath( this->FileName );
  
  data->Update();        
  
  //set the block
  block->SetBlock( block->GetNumberOfBlocks(), data->GetOutput() );                
      
  //now set the name of the block       
  block->GetMetaData( pos )->Set( vtkCompositeDataSet::NAME(), name->c_str() );
  
  //data->UpdateFilePos( this->File );        
  data->Delete();
  
  //now add block back on the top
  this->Internal->groups.push( block );
  block->Delete();
}

// --------------------------------------
void vtkGocadReader::AppendBlock( )
{
  if (this->Internal->groups.size() > 1)
    {
    //old top of stack
    vtkMultiBlockDataSet *Oldblock = vtkMultiBlockDataSet::New();
    Oldblock->DeepCopy( this->Internal->groups.top() );
    this->Internal->groups.pop();    
    
    //new top of stack
    vtkMultiBlockDataSet *block  = vtkMultiBlockDataSet::New();
    block->DeepCopy( this->Internal->groups.top() );    
    this->Internal->groups.pop();    
                        
    //make old a subblock of new top                         
    int pos = block->GetNumberOfBlocks();

    //set the oldblock, as sub block of the current top of the stack
    block->SetBlock( pos, Oldblock );

    //set the block name
    vtkStdString name = this->Internal->groupNames.top();
    this->Internal->groupNames.pop(); //remove the name
    block->GetMetaData( pos )->Set( vtkCompositeDataSet::NAME(), name ); //set the name of the block  
           
    //cleanup memory   
    Oldblock->Delete();            
    
    //now add block back on the top
    this->Internal->groups.push( block );
    block->Delete();
    }
}


//----------------------------------------------------------------------------
bool vtkGocadReader::OpenFile( const char* filename )
{
  this->File = new ifstream(filename, ios::in);    
  
  if ( this->File->bad() || this->File->fail() )
    {
    //there was a problem loading the file
    return false;
    }
  return true;
}
 
 //----------------------------------------------------------------------------
void vtkGocadReader::CloseFile( ){  
  if (this->File)
    {    
    delete this->File;
    }
}

// --------------------------------------
void vtkGocadReader::NextLine( )
{
 
 bool lineFound = false;
 while ( !this->File->eof() && !lineFound )
  {
    getline( *this->File, *this->Line );
       
    //make sure we go to the next 'real' line
    if (this->Line->length() > 0 )
      {
      //line is not empty
      if (this->Line->at( 0 ) != '#' )
        {       
        lineFound = true;        
        }
      }        
  }
}


// -------------------------------------- 
void vtkGocadReader::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "FileName: "
     << (this->FileName ? this->FileName : "(none)") << "\n";
}
