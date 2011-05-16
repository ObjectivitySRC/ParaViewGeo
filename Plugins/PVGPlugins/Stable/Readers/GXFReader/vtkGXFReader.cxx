#include "vtkGXFReader.h"
#include "vtkFloatArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkImageData.h"
#include "vtkTransform.h"
#include "vtkTransformFilter.h"

//Slightly hackish, but declared global for speed
const int POWER_TABLE[5] = {1, 90, 8100, 729000, 65610000 };	//table of GTYPE powers, precalculated for speed only 5 levels of GTYPE Compression 
const int BASE_VALUE = 37;  //Base-90 digits use ASCII characters in the range 37 to 126 ("%" to "~"),

vtkCxxRevisionMacro(vtkGXFReader, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkGXFReader);


vtkGXFReader::vtkGXFReader()
{
	//IO Properties
	this->GridBlockPosition = -1;
	this->FileName = 0;

	//default values for GFX files
  this->DummyValue = -9999; //Default gxf dummy value
  this->GType = 0;
  this->Points = 0;
  this->Rows = 0;	
	this->PointSeperation = 1;
	this->RowSeperation = 1;	
	this->Rotation = 0;
	this->Sense = 1; //default xmin to xmax, x than y read ordering
	this->Title = "Property";
	this->Transform[0] = 1.0; //scale
	this->Transform[1] = 0.0; //offset
	this->XOrigin = 0;
	this->YOrigin = 0;
		
	
  this->SetNumberOfInputPorts(0);
  this->SetNumberOfOutputPorts(1);
}

vtkGXFReader::~vtkGXFReader()
{
	delete[] this->FileName;
}

int vtkGXFReader::CanReadFile(const char *fname)
{
  return 1;
}

int vtkGXFReader::RequestInformation(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{
	
	bool fileGood = this->OpenFile( this->FileName );
  if (!this->File || !fileGood)
    {
    vtkErrorMacro("Failed to load the file");
    return 0;
    }  
	
	GXFDataBlock *block = new GXFDataBlock();

	int tempPos = 0;
	while( this->File->good() && block->Name != "#GRID" )
		{
		
		//we store the position in the file before we read the next block.
		//the reason for this is so that we can skip right down to the #GRID block
		// in request data
		tempPos = this->File->tellg( ); 

		NextDataBlock( block );	
		if( block->Name == "#DUMMY" )
			{			
			block->Buffer >> this->DummyValue;
			}
		else if( block->Name == "#GTYPE" )
			{			
			block->Buffer >> this->GType;
			}
		else if (block->Name == "#GRID")
			{
			//we found the place in file that the grid block is at
			this->GridBlockPosition = tempPos;				
			}	
		else if ( block->Name == "#POINTS" )
			{			
			block->Buffer >> this->Points;
			}
		else if( block->Name == "#PTSEPARATION" )
			{			
			block->Buffer >> this->PointSeperation;
			}
		else if( block->Name == "#ROWS" )
			{			
			block->Buffer >> this->Rows;
			}
		else if( block->Name == "#ROTATION" )
			{			
			block->Buffer >> this->Rotation;
			}
		else if( block->Name == "#RWSEPARATION" )
			{			
			block->Buffer >> this->RowSeperation;
			}
		else if( block->Name == "#SENSE" )
			{			
			block->Buffer >> this->Sense;
			}
		else if( block->Name == "#TITLE" )
			{			
			block->Buffer >> this->Title;
			}
		else if( block->Name == "#TRANSFORM" )
			{			
			block->Buffer >> this->Transform[0];
			//if we also have a offset too
			if ( block->Buffer.good() )
				{
				block->Buffer >> this->Transform[1];
				}
			}
		else if ( block->Name == "#XORIGIN" )
			{
			block->Buffer >> this->XOrigin;
			}
		else if ( block->Name == "#YORIGIN" )
			{
			block->Buffer >> this->YOrigin;
			}
		}	
	//cleanup grid code
	delete block;
	this->CloseFile();

  return 1;
}

int vtkGXFReader::RequestData(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{
	bool fileGood = this->OpenFile( this->FileName );
	//Check for badly formatted files
	if (!this->File || !fileGood )
    {
    vtkErrorMacro("Failed to load the file");
    return 0;
    }  

	if (this->GridBlockPosition == -1)
		{
		vtkErrorMacro("Failed to Find #GRID block" );
		return 0;
		}

	if ( this->Points == 0 || this->Rows == 0 )
		{
		vtkErrorMacro("You have to set both #POINTS and #ROWS");
		return 0;
		}
  
	//construct the output
	vtkInformation *outInfo = outputVector->GetInformationObject(0);  
	vtkImageData *output = vtkImageData::SafeDownCast( outInfo->Get(vtkDataObject::DATA_OBJECT()) );
	
	//done checking if the file is bad, now read the #GRID block
	this->File->seekg( this->GridBlockPosition );

	//the actual read
	GXFDataBlock *block = new GXFDataBlock();
	this->NextDataBlock( block, true ); //read the #GRID block
	this->UncompressBlock( block ); //uncompress the block if needed
	
	//create the property array
	vtkFloatArray *prop = vtkFloatArray::New();
	this->CreateProperty( block, prop );	
	delete block;

	//no need for file, close it
	this->CloseFile();

	//set the property
	output->GetPointData()->AddArray( prop );
	prop->Delete();
	
	int dims[3] = {1,1,1};
	double rowSep, pointSep;	
	if ( this->Sense == 1 || this->Sense == -2 ||  this->Sense == 3 || this->Sense == -4)
		{		
		dims[0] = this->Points;
		dims[1] = this->Rows;		
		pointSep = this->PointSeperation; 
		rowSep = this->RowSeperation;		
		}
	else
		{
		dims[1] = this->Points;
		dims[0] = this->Rows;		
		rowSep = this->PointSeperation; 
		pointSep = this->RowSeperation;		
		}

	output->SetOrigin(0,0,0);
	output->SetSpacing(pointSep,rowSep,0);	
	output->SetDimensions( dims );

	//set all these so state files will work
	int extent[6] = {0,dims[0]-1,0,dims[1]-1,0,dims[2]-1};
	outInfo->Set( vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), extent, 6 );	
	output->SetExtent( extent );
	output->SetWholeExtent( extent );
	output->SetUpdateExtent( extent );	
	
  return 1;
}

//----------------------------------------------------------------------------
void vtkGXFReader::CreateProperty( GXFDataBlock *block, vtkFloatArray* prop )
{				
	prop->Allocate( this->Points * this->Rows );
	prop->SetName(this->Title.c_str());
	prop->SetNumberOfValues( this->Points * this->Rows );

	double value;
	int pointSize, rowSize, pos;
	
	rowSize = this->Rows;
	pointSize = this->Points;		
	if ( this->Sense >= 3 || this->Sense <= -3 )
		{
		vtkErrorMacro(<<"We currently on support 1,-1,2,-2 sense values" );
		}
	if ( this->Sense == 1)
		{		
		//right handed read		
		//bottom left
		for ( int i=0; i < rowSize; i++ )
			{
			pos = ( i * pointSize );
			for ( int j=0; j < pointSize; j++ )
				{
				block->Buffer >> value;
				prop->InsertValue( pos++, value );
				}
			}
		}
	else if( this->Sense == 2 )
		{
		//right handed read		
		//top left to bottom left	
		for ( int i=0; i < rowSize; i++  )
			{										
			for ( int j= (pointSize-1); j >= 0; j--  )
				{				
				pos = ( rowSize * j ) + i;
				block->Buffer >> value;
				prop->InsertValue( pos, value );						
				}
			}		
		}
	else if ( this->Sense == -1 )
		{
		//left handed read		
		//bottom left to top left	
		for ( int i=0; i < rowSize; i++  )
			{										
			for ( int j= 0; j < pointSize; j++  )
				{				
				pos = ( rowSize * j ) + i;
				block->Buffer >> value;
				prop->InsertValue( pos, value );																
				}
			}			
		}
	else if( this->Sense == -2 )
		{
		//left handed read		
		//top left to top right
		for ( int i= (rowSize-1); i >= 0; i-- )
			{
			pos = (i * pointSize );
			for ( int j=0; j < pointSize; j++ )
				{
				block->Buffer >> value;
				prop->InsertValue( pos++, value );											
				}
			}		
		}	
	return;
}

//----------------------------------------------------------------------------
bool vtkGXFReader::OpenFile( const char* filename )
{
  this->File = new ifstream(filename, ios::in );    
  
  if ( this->File->bad() || this->File->fail() )
    {
    //there was a problem loading the file
    return false;
    }
  return true;
}
 
 //----------------------------------------------------------------------------
void vtkGXFReader::CloseFile( ){  
  if (this->File)
    {    
	this->File->close();
    delete this->File;
    }
}

// --------------------------------------

//WARNING: THIS METHOD WILL CLEAR THE BLOCK YOU PASS IN
void vtkGXFReader::NextDataBlock( GXFDataBlock* block, bool ParseGrid )
{ 
	//first we clear the block
	block->Name.clear();	
	block->Buffer.str( "" );
	block->Buffer.clear(); //clear the eof flag, since we have emptied the string ( very important )
	
	vtkstd::string line;
	bool commandFound = false;	
	//find the name of the command
	while ( !this->File->eof() && !commandFound )
		{
    getline( *this->File, line );
       
    //make sure we go to the next 'real' line
    if (line.length() == 0 )
      {        
			//short circut empty lines, so we don't get nesting
			continue;
      }        
		else if ( line.at(0) == '#' )
			{
			//we found our command, start buffering!
			this->TrimWhiteSpaces( line );
			block->Name = line;
			commandFound = true;			
			}
		}
	
	if ( !commandFound )
		{		
		//no command found, very very bad place to be
		return;
		}

	if ( block->Name == "#GRID" && !ParseGrid )
		{
		//don't read the block info during Request Information so we run faster!
		return;
		}

	//keep reading lines, till we hit a new block
	//we ignore comment  and empty lines
	char peeked;
	while ( !this->File->eof())
		{				
		//if we see a command, stop reading
		peeked = this->File->peek();
		if ( peeked == '#' )
			{			
			break;
			}

		getline( *this->File, line );		
		if ( this->GType == 0 || block->Name != "#GRID"   )
			{			
			// commas are part of the base90 format, so we can only replace
			// commas when they are part of a command, or a grid that is not compressed
			this->ReplaceCommas( line );
			}

		if ( block->Name == "#GRID" )
			{			
			//grid blocks can have any ascii character from 37 to 127
			block->Buffer << line;
			if ( this->GType == 0)
				{			
				//only on now uncompressed do we add spaces, otherwise it screws up the 
				//uncompressing algorithm
				block->Buffer << " ";
				}
			}
		else if ( isdigit( peeked ) || peeked == '"' || peeked == '-')
			{				
			//only add lines that start with a numeric character or a quote
			//this is for everything but #GRID blocks
			block->Buffer << line << " ";			
			}				
		
		}		  	
}
// --------------------------------------
void vtkGXFReader::TrimWhiteSpaces( vtkstd::string& str)
{
  // Trim Both leading and trailing spaces
  size_t startpos = str.find_first_not_of(" \t"); 
  size_t endpos = str.find_last_not_of(" \t"); 

  // if all spaces or empty return an empty string
	if(( vtkstd::string::npos == startpos ) || ( vtkstd::string::npos == endpos))
		{
    str = "";
		}
  else
		{
		str = str.substr( startpos, endpos-startpos+1 );      
		}
}

// --------------------------------------
void vtkGXFReader::ReplaceCommas( vtkstd::string& str )
{
	//replace all commas with spaces
	int i;
	while ( ( i= str.find(",") ) != vtkstd::string::npos )
		{
		str.replace(i,1," ");
		}
}

// --------------------------------------
void vtkGXFReader::UncompressBlock( GXFDataBlock* block )
{		

	if ( this->GType == 0 )
		{
		return; //not compressed
		}
	
	const int REPEAT_CODE = -24573; //Unique number for repeat command
	const int DUMMY_CODE = -32764; //Unique number for dummy command
	vtkstd::stringstream tempBuffer; //temp buffer that holds uncompressed stream
	
	vtkErrorMacro(<<block->Buffer.str() );
	double value = 0;
	double repeats = 1;
	while ( block->Buffer.good() )
		{				
		//convert the value to decimal
		value = this->ConvertBaseNinety( block );

		//check if we hit dummy code
		if ( value == DUMMY_CODE )
			{
			//replace the value with the specified dummy value
			//we ignore the transform, and buffer it straight			
			tempBuffer << this->DummyValue << " ";	
			continue;
			}						
		else if ( value == REPEAT_CODE ) //check if we hit a repeat code
			{
			//we first read the number to convert
			repeats = this->ConvertBaseNinety( block );

			//transform repeats, so it the proper value
			repeats = (repeats * this->Transform[0] ) + this->Transform[1];

			//read the number of times to repeat it
			value = this->ConvertBaseNinety( block );
			//convert value based on transform
			value = (value * this->Transform[0] ) + this->Transform[1];

			//we buffer the value potential multiple times 			
			for ( int i=0; i < repeats; i++ )
				{		
				tempBuffer << value << " ";		
				}		
			}
		else
			{
			//regular buffer
			double temp = value * this->Transform[0];
			temp+= this->Transform[1];
			value = temp;
			//value = (value * this->Transform[0] ) + this->Transform[1];
			tempBuffer << value << " ";		
			}				
		}		

	//copy the buffer
	block->Buffer.str( tempBuffer.str() );
	block->Buffer.clear(); //clear error log from iterating the buffer
	vtkErrorMacro(<<block->Buffer.str() );
}

// --------------------------------------
double vtkGXFReader::ConvertBaseNinety( GXFDataBlock* block )
	{		
	double value = 0;
	char input;	
	int num = 0;	

	for ( int i=this->GType-1; i >= 0; i-- )
		{			
		block->Buffer >> input;
		num = static_cast<int>(input);
		value += (num - BASE_VALUE) * POWER_TABLE[i];	
		}			
	return value;
}

// --------------------------------------
void vtkGXFReader::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

