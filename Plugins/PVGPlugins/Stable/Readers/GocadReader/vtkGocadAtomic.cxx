/*=========================================================================

  Program:   ParaViewGeo
  Module:    $RCSfile: vtkGocadAtomic.cxx,v $

  Copyright (c) Robert Maynard
  All rights reserved.  
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkGocadAtomic.h"
#include "vtkObjectFactory.h"
#include "vtkExecutive.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkPoints.h"
#include "vtkPointData.h"
#include "vtkUnstructuredGrid.h"
#include "vtkStructuredGrid.h"
#include "vtkPolyData.h"
#include "vtkDataSet.h"
#include "vtkCellArray.h"
#include "vtkStringArray.h"
#include "vtkDoubleArray.h"
#include "vtkCellData.h"

#include <vtkstd/vector>
#include <vtkstd/map>
#include <sstream>


vtkCxxRevisionMacro(vtkGocadAtomic, "$Revision: 0.3 $");
vtkStandardNewMacro(vtkGocadAtomic);

typedef vtkstd::vector< vtkDoubleArray* > vtkStorageBase;
typedef vtkstd::vector< vtkDoubleArray* >::iterator PropIter;

// --------------------------------------
class vtkInternalProps: public vtkStorageBase {};


// --------------------------------------
class vtkInternalMap
{
public:    
  void SetID( int oldID, int newId );
  int GetID( int oldID );  
  vtkstd::map< int, int > map;  
  
};

// --------------------------------------
int vtkInternalMap::GetID( int oldID )
{
  return this->map[ oldID ];

}

// --------------------------------------
void vtkInternalMap::SetID( int oldID, int newId )
{
  this->map[ oldID ] = newId;
}


// --------------------------------------
vtkGocadAtomic::vtkGocadAtomic()
{    	
  this->DataType = VTK_POLY_DATA;
  this->File = NULL;
  this->Line = new vtkStdString;  
  this->PointMap = new vtkInternalMap;
  this->Properties = new vtkInternalProps;
	this->CellProperties = new vtkInternalProps;
  this->HaveProperties = false;
    
  this->SetNumberOfInputPorts(0);
  this->SetNumberOfOutputPorts(1);
};

// --------------------------------------
vtkGocadAtomic::~vtkGocadAtomic()
{  
  //clean up properties
	int cnt = this->Properties->size();
  while( this->Properties->size() > 0 )
    {
    vtkDoubleArray *toDel;
    toDel = this->Properties->back();
		
		if ( toDel )
			{
			toDel->Delete();
			}
    this->Properties->pop_back();    
    }

	while( this->CellProperties->size() > 0 )
    {
    vtkDoubleArray *toDel;
    toDel = this->CellProperties->back();
		
		if ( toDel )
			{
			toDel->Delete();
			}
    this->CellProperties->pop_back();    
    }
  
  //clean up the lookup table
  while( !this->PointMap->map.empty() )
    {
    this->PointMap->map.erase( this->PointMap->map.begin() );
    }
   
  //remove the file reference, so we can delete it safely
  this->File=NULL;
 
  delete this->File;
  delete this->Properties;  
	delete this->CellProperties;  
  delete this->PointMap;
  delete this->Line;    
}

//----------------------------------------------------------------------------
int vtkGocadAtomic::RequestDataObject(
  vtkInformation*, 
  vtkInformationVector** inputVector , 
  vtkInformationVector* outputVector)
{
  /*wow this is fucking insane, the standard
  RequestDataObject in vtkDataSetAlgorithm is currently broken (August 28, 2008 )
  when it comes to the case of the class has zero input
  
  This will fix that for only atomic and its children */
  vtkInformation *outInfo;
  vtkDataSet *output = 0;
  switch (this->DataType)
    {
    case VTK_POLY_DATA:
      outInfo = outputVector->GetInformationObject(0);
      if (!outInfo)
        {
        output = vtkPolyData::New();
        }
      else
        {
        output = vtkPolyData::SafeDownCast(
          outInfo->Get(vtkDataObject::DATA_OBJECT()));
        if (!output)
          {
          output = vtkPolyData::New();
          }
        else
          {
          return 1;
          }
        }
      this->GetExecutive()->SetOutputData(0, output);
      output->Delete();
      break;
    case VTK_UNSTRUCTURED_GRID:
      outInfo = outputVector->GetInformationObject(0);
      if (!outInfo)
        {
        output = vtkUnstructuredGrid::New();
        }
      else
        {
        output = vtkUnstructuredGrid::SafeDownCast(
          outInfo->Get(vtkDataObject::DATA_OBJECT()));
        if (!output)
          {
          output = vtkUnstructuredGrid::New();
          }
        else
          {
          return 1;
          }
        }
      this->GetExecutive()->SetOutputData(0, output);
      output->Delete();
      break;
		case VTK_STRUCTURED_GRID:
      outInfo = outputVector->GetInformationObject(0);
      if (!outInfo)
        {
        output = vtkStructuredGrid::New();
        }
      else
        {
        output = vtkStructuredGrid::SafeDownCast(
          outInfo->Get(vtkDataObject::DATA_OBJECT()));
        if (!output)
          {
          output = vtkStructuredGrid::New();
          }
        else
          {
          return 1;
          }
        }
      this->GetExecutive()->SetOutputData(0, output);
      output->Delete();
      break;
    default:
      return 0; 
    }

  return 1;          
}

// --------------------------------------
int vtkGocadAtomic::RequestData(vtkInformation* request,
                          vtkInformationVector** inputVector,
                          vtkInformationVector* outputVector)
{

	vtkInformation *outInfo = outputVector->GetInformationObject(0);  
  vtkDataSet *output = vtkDataSet::SafeDownCast( outInfo->Get(vtkDataObject::DATA_OBJECT()));
  
	//need to add the properties
	for (unsigned int i=0; i < this->Properties->size(); i++)
		{      
    vtkDoubleArray *prop;
    prop = this->Properties->at( i );
		if( prop->GetNumberOfTuples() > 0 )
			{
			if ( i == 0)
				{
				output->GetPointData()->SetScalars( prop );
				}
			else
				{
				output->GetPointData()->AddArray( prop );
				}
			}
    }

	for (unsigned int i=0; i < this->CellProperties->size(); i++)
		{      
    vtkDoubleArray *prop;
    prop = this->CellProperties->at( i );
		if( prop->GetNumberOfTuples() > 0 )
			{
			if ( i == 0)
				{
				output->GetCellData()->SetScalars( prop );
				}
			else
				{
				output->GetCellData()->AddArray( prop );
				}
			}
    }
    
  return 1;
}


//----------------------------------------------------------------------------
int vtkGocadAtomic::FillOutputPortInformation(int vtkNotUsed(port), vtkInformation* info)
{
  //info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkPolyData");
	info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkDataSet");
  return 1;
}

// --------------------------------------
bool vtkGocadAtomic::ReadProperties(  )
{
  int pos = 0;
  const int PROP_LEN = 10; //len of properties string
  do
    {
    this->NextLine();       
    pos = this->Line->find("PROPERTIES"); 
    if ( pos != vtkstd::string::npos )
      {
      this->SetFileProperties( this->Line->substr( pos + PROP_LEN ) ); 
      this->HaveProperties = true;     
      }
      
    }while( this->Line->find("VRTX") == vtkstd::string::npos );
  return true;
}
// --------------------------------------

bool vtkGocadAtomic::ReadPoints(  vtkPoints *points, vtkStdString endCondition   )
{
  
  const int ID_XYZ_SIZE = 4;
  const int VRTX_LEN = 4;
  const int ATOM_LEN = 4;
  int pos = 0;  
  int pointCounter = points->GetNumberOfPoints(); //used for the lookup map
    
  while(  this->Line->find(endCondition) == vtkstd::string::npos  )
    {     
                
    pos = this->Line->find("VRTX");    
    if ( pos != vtkstd::string::npos )
      {
      //parse the line
      int size = ID_XYZ_SIZE + this->Properties->size();
      double *value = new double[size];
      this->ParseLine( this->Line->substr( pos + VRTX_LEN ), size, value ); 
      
      int id = static_cast<int>(value[0]);
      this->PointMap->SetID( id, pointCounter );      
      points->InsertPoint( pointCounter, value[1], value[2], value[3] );      
      pointCounter++;
      
      //now check if the line has properties to add
      if ( this->HaveProperties )
        {
        for (unsigned int i=0; i < this->Properties->size(); i++)
          {
          this->Properties->at( i )->InsertNextValue( value [ i + 4 ] );
          }
        }
      delete[] value;
      }
    
    //have to handle the use case of ATOM's
    //which are a reference to a previous defined point!
    pos = this->Line->find("ATOM");    
    if ( pos != vtkstd::string::npos )
      {
      //parse the line
      //atoms are duplicate points, where the first number is the ID number of the point, and the second number
      //is the index number of the point it is a duplicate of
      //ATOM ID1 ID2      
      
      int size = 2 + this->Properties->size();
      double *value = new double[size];
      this->ParseLine( this->Line->substr( pos + ATOM_LEN ), size, value ); 
      
      //grab the int's that represet the current id, and the point id it should actually be
      int atom = static_cast<int>(value[0]);
      int id = static_cast<int>(value[1]);
      
      //make the point map setup so that when you query with the atom number, you get the point
      //number from the real id that the atom is pointing at
      int tmp = this->PointMap->GetID( id );
      this->PointMap->SetID( atom,  tmp);          
      delete[] value;
      }
    this->NextLine();
    }

  return true;
}

// --------------------------------------
bool vtkGocadAtomic::ReadCells( vtkCellArray *cells, int cellSize, vtkStdString cellName, int cellNameLength, vtkStringArray *endOfCell )
{
	bool continueReading = true;
  int pos = 0;      
  do
    {     
    //check for a normal triangle          
    pos = this->Line->find(cellName);    
    if ( pos != vtkstd::string::npos )
      {
      //read the line
      int *value = new int[cellSize];
      this->ParseLine( this->Line->substr( pos + cellNameLength ), cellSize, value );
      
      //now that we have the line, create the cell
      cells->InsertNextCell( cellSize );
      for (int i=0; i < cellSize ; i++)
        {
        int tmp = this->GetPointMapID( value[i] );
        cells->InsertCellPoint( tmp );      
        }
      delete[] value;
      }                 
    //read the next line 
    this->NextLine();

		for( int conditionCount = 0; conditionCount < endOfCell->GetNumberOfValues(); conditionCount++)
			{
				if( this->Line->find( endOfCell->GetValue( conditionCount ) ) != vtkstd::string::npos )
					{
					continueReading = false;
					}
			}
        
    }while( continueReading );
		//while(  this->Line->find(endOfCell) == vtkstd::string::npos && this->Line->find("END") == vtkstd::string::npos    );
    
  return true;    
}


// --------------------------------------
void vtkGocadAtomic::NextLine( )
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
void vtkGocadAtomic::ParseASCIIFileLine(vtkStdString line, int length, vtkStringArray *values){
  vtkStdString substr = line;
  
  int x = substr.length();
  
  values->InsertNextValue( substr.substr(1, substr.length()-1) );
}

// --------------------------------------
void vtkGocadAtomic::ParseLine(vtkStdString line, int length, double *values){
  vtkStdString::size_type loc;
  vtkStdString substr = line;
  std::stringstream stream;
  
  int x = 0;
  do{
		//loc = substr.find(" ", 1);
		loc = substr.find_first_not_of(" ", 1);
		loc = substr.find_first_of(" ", loc);
    //loc = substr.find("	", 1);
		if( loc < 0 || loc == vtkStdString::npos || loc == substr.length()-1 )
	 {
		 loc = substr.find("	", 1);
	 }

   stream << substr.substr(0,loc);
   stream >> values[x++];
   stream.clear();
      
   if (loc != vtkStdString::npos){
    substr = substr.substr(loc);
   }
  }while(loc != vtkStdString::npos && x < length);  
}

// --------------------------------------
void vtkGocadAtomic::ParseLine(vtkStdString line, int length, int *values){
  vtkStdString::size_type loc;
  vtkStdString substr = line;
  std::stringstream stream;
  
  int x = 0;
  do{
   loc = substr.find(" ", 1);
	 loc = substr.find_first_not_of(" ", loc);

   stream << substr.substr(0,loc);
   stream >> values[x++];
   stream.clear();
      
   if (loc != vtkStdString::npos){
    substr = substr.substr(loc);
   }
  }while(loc != vtkStdString::npos && x < length);  
}

// --------------------------------------
void vtkGocadAtomic::ParseLine(vtkStdString line, int length, vtkStringArray *values){
  vtkStdString::size_type loc;
  vtkStdString substr = line;
	vtkStdString tempString = "";
  std::stringstream stream;
  
  //int x = 0;
	int x = values->GetNumberOfValues();
  do{
   loc = substr.find(" ", 1);
      
 
   stream << substr.substr(0,loc);
	 stream >> tempString;
	 // Remove surrounding quotes
	 if ( tempString.substr(0, 1) == "\"" )
	 {
		 values->InsertValue(x, tempString.substr(1, tempString.length()-2) );
	 }
   stream.clear();
	 x++;
      
   if (loc != vtkStdString::npos){
    substr = substr.substr(loc);
   }
  }while(loc != vtkStdString::npos && x < length);  
}


// --------------------------------------
void vtkGocadAtomic::SetFileProperties(vtkStdString line){

  vtkStdString::size_type loc;
  vtkStdString substr = line;
	vtkStdString tempString = "";
	// Ensure that there is no space at the end of the line
	substr.erase(substr.find_last_not_of(" ") + 1);
  
  bool continueParse = true;
  while( continueParse ){
    std::stringstream stream;
    
    loc = substr.find(" ", 1);
    
    //our end case
    if (loc == vtkStdString::npos) 
			{
			continueParse = false;
			loc = substr.size();
			}
                   
    //grab the property names
    stream << substr.substr(0,loc);             
    
		//create the property array list
		vtkDoubleArray *prop = vtkDoubleArray::New();
		tempString = stream.str().c_str();
		if( tempString.substr(0,1) == "\"" )
		{
			tempString = tempString.substr(1, tempString.length()-2);
		}
		prop->SetName( tempString );
		//prop->SetName( stream.str().c_str() );      
		this->Properties->push_back( prop );
            
    substr = substr.substr(loc,substr.size());
      
  }
    
}

// --------------------------------------
void vtkGocadAtomic::SetFileCellProperties(vtkStdString line){

  vtkStdString::size_type loc;
  vtkStdString substr = line;
	vtkStdString tempString = "";
	// Ensure that there is no space at the end of the line
	substr.erase(substr.find_last_not_of(" ") + 1);
  
  bool continueParse = true;
  while( continueParse ){
    std::stringstream stream;
    
    loc = substr.find(" ", 1);
    
    //our end case
    if (loc == vtkStdString::npos) 
			{
			continueParse = false;
			loc = substr.size();
			}
                   
    //grab the property names
    stream << substr.substr(0,loc);             
    
		//create the property array list
		vtkDoubleArray *prop = vtkDoubleArray::New();
		tempString = stream.str().c_str();
		if( tempString.substr(0,1) == "\"" )
		{
			tempString = tempString.substr(1, tempString.length()-2);
		}
		prop->SetName( tempString );
		//prop->SetName( stream.str().c_str() );      
		this->CellProperties->push_back( prop );
            
    substr = substr.substr(loc,substr.size());
      
  }
    
}

// --------------------------------------
void vtkGocadAtomic::SetFilePath( char* FileName )
{
	vtkStdString fullName = FileName;
	//Find position final '\' that occurs just before the file name
	int slashPosition = fullName.find_last_of('\\');
		//sometimes path contains the other slash ('/')
	if(slashPosition == -1)
		slashPosition = fullName.find_last_of('/');
	//Add one to slashPosition so that the slash is included
	slashPosition = slashPosition+1;

	//const char* temp = fullName.substr(0, slashPosition).c_str().str();
  this->FilePath = fullName.substr(0, slashPosition);
}

// --------------------------------------
void vtkGocadAtomic::SetFile( ifstream *file )
{  
  this->File = file;  
}

// --------------------------------------
void vtkGocadAtomic::SetPointMapID( int oldID, int newID )
{
  this->PointMap->SetID( oldID, newID );
}

// --------------------------------------
int vtkGocadAtomic::GetPointMapID( int oldID )
{
  return this->PointMap->GetID( oldID );
}


// --------------------------------------
void vtkGocadAtomic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);  
}


bool vtkGocadAtomic::InsertPropertyTuple( unsigned int index, double* tuple )
{
	if ( index >= 0 && index < this->Properties->size() )
		{
		this->Properties->at( index )->InsertNextTuple( tuple );
		return true;
		}
		
 return false;
}

bool vtkGocadAtomic::InsertPropertyValue( unsigned int index, double value )
{
	if ( index >= 0 && index < this->Properties->size() )
		{
		this->Properties->at( index )->InsertNextValue( value );
		return true;
		}
		
 return false;
}

int vtkGocadAtomic::GetPropertySize()
{
	return (int)this->Properties->size();
}

int vtkGocadAtomic::GetPropertyTupleCount( int pos)
{
return (int) (this->Properties->at( pos )->GetNumberOfTuples());
}

bool vtkGocadAtomic::InsertCellPropertyTuple( unsigned int index, double* tuple )
{
	if ( index >= 0 && index < this->CellProperties->size() )
		{
		this->CellProperties->at( index )->InsertNextTuple( tuple );
		return true;
		}
		
 return false;
}

bool vtkGocadAtomic::InsertCellPropertyValue( unsigned int index, double value )
{
	if ( index >= 0 && index < this->CellProperties->size() )
		{
		this->CellProperties->at( index )->InsertNextValue( value );
		return true;
		}
		
 return false;
}

int vtkGocadAtomic::GetCellPropertySize()
{
	return (int)this->CellProperties->size();
}