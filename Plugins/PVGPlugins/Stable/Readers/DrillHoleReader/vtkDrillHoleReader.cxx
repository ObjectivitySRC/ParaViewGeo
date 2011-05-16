/*=========================================================================

  Program:   ParaViewGeo
  Module:    $RCSfile: vtkDrillHoleReader.cxx,v $

  Copyright (c) Robert Maynard
  All rights reserved.  
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkDrillHoleReader.h"

#include "vtkBitArray.h"
#include "vtkCallbackCommand.h"
#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkCompositeDataSet.h"
#include "vtkDemandDrivenPipeline.h"
#include "vtkDataArraySelection.h"
#include "vtkDoubleArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkIntArray.h"
#include "vtkObjectFactory.h"
#include "vtkPoints.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkRowQueryToTable.h"
#include "vtkStdString.h"
#include "vtkStringArray.h"
#include "vtkSQLiteDatabase.h"
#include "vtkSQLiteQuery.h"
#include "vtkAbstractArray.h"
#include "vtkCellDataToPointData.h"
#include "vtkExtractLevel.h"
#include "vtkCompositeDataPipeline.h"
#include "vtkDataSetMapper.h"
#include "vtkActor.h"

#include "vtkIdList.h"

#include <vtkstd/vector>
#include <sstream>


typedef vtkstd::vector< vtkAbstractArray* > vtkStorageBase;
typedef vtkstd::vector< vtkAbstractArray* >::iterator PropIter;

// --------------------------------------
class vtkInternalProps: public vtkStorageBase {};



vtkCxxRevisionMacro(vtkDrillHoleReader, "$Revision: 0.3 $");
vtkStandardNewMacro(vtkDrillHoleReader);

vtkDrillHoleReader::vtkDrillHoleReader()
{    
	this->DrillHoleCount = 0;
  this->FileName = 0;
	this->Filter = 0;

	this->KeepCellProp = false;
	this->CreatePointProp = false;
	
	this->NameMap = vtkBitArray::New();
  this->Database = vtkSQLiteDatabase::New();
  
  this->DrillHoles = vtkDataArraySelection::New();
  // Setup the selection callback to modify this object when an array
  // selection is changed.
  this->SelectionObserver = vtkCallbackCommand::New();
  this->SelectionObserver->SetCallback(&vtkDrillHoleReader::SelectionModifiedCallback);
  this->SelectionObserver->SetClientData(this);
  this->DrillHoles->AddObserver(vtkCommand::ModifiedEvent,this->SelectionObserver); 
     
  this->SetNumberOfInputPorts(0);
  this->SetNumberOfOutputPorts(3);
};

// --------------------------------------
vtkDrillHoleReader::~vtkDrillHoleReader()
{
  this->SetFileName(0);
  
	this->Database->Close();
  this->Database->Delete();	
  
  //deleting object variables
  if (this->DrillHoles!=NULL)
    {
	  this->DrillHoles->RemoveObserver(this->SelectionObserver);
	  this->DrillHoles->Delete();
    }
  this->SelectionObserver->Delete();

	this->NameMap->Delete();
}

// --------------------------------------
int vtkDrillHoleReader::CanReadFile( const char *fname )
{
	return 1;
}
// --------------------------------------
int vtkDrillHoleReader::RequestInformation(vtkInformation* request,
                          vtkInformationVector** inputVector,
                          vtkInformationVector* outputVector)
{
	int maxDrillHoleId = 0;
	if ( this->OpenDatabase() )
		{
		vtkSQLiteQuery *query = (vtkSQLiteQuery *)this->Database->GetQueryInstance();
		vtkstd::stringstream buffer;	
		buffer << "SELECT Collar.hole_id,DrillCurve.collar_id";
		buffer << " FROM Collar INNER JOIN DrillCurve ON Collar.id=DrillCurve.collar_id";
		buffer << " ORDER BY DrillCurve.collar_id,DrillCurve.id";  // MUST be last field in query.
	  
	  
		// Retrieve all data in one query.
		query->SetQuery(buffer.str().c_str());
		query->Execute(); //run the query
	  
		vtkStdString name;			
		int currentId = 0;
		while( query->NextRow() ) 
			{  
			// for each collar ...
			name = query->DataValue(0).ToString();    			
			currentId = query->DataValue(1).ToInt();
			
			if (!this->DrillHoles->ArrayExists(name))
				{		
				
				this->DrillHoles->AddArray(name);
				this->DrillHoles->EnableArray(name);		
				}

			if ( currentId > maxDrillHoleId )
				{
				maxDrillHoleId = currentId;
				}
			}
		
		this->CloseQuery( query );
		this->Database->Close();
		}
	
	this->DrillHoleCount = this->DrillHoles->GetNumberOfArrays();
	if ( maxDrillHoleId > this->DrillHoleCount )
		{
		this->DrillHoleCount = maxDrillHoleId;
		}

	this->NameMap->SetNumberOfValues( this->DrillHoleCount );

	for ( int i=0; i < this->NameMap->GetNumberOfTuples(); i++)
		{
		this->NameMap->SetValue(i,0);
		}

	this->SetupOutputInformation( this->GetOutputPortInformation(0) );		
	return 1;
}

// --------------------------------------
int vtkDrillHoleReader::RequestData(vtkInformation* request,
                          vtkInformationVector** inputVector,
                          vtkInformationVector* outputVector)
{

  vtkInformation *outInfo = outputVector->GetInformationObject(0);  
  vtkPolyData *curve = vtkPolyData::SafeDownCast( outInfo->Get(vtkDataObject::DATA_OBJECT()));

	outInfo = outputVector->GetInformationObject(1);  
	vtkPolyData *assay = vtkPolyData::SafeDownCast( outInfo->Get(vtkDataObject::DATA_OBJECT()));

	outInfo = outputVector->GetInformationObject(2);  
	vtkPolyData *litho = vtkPolyData::SafeDownCast( outInfo->Get(vtkDataObject::DATA_OBJECT()));
	
	
  if ( this->OpenDatabase() )
		{		
		this->ReadCurves( curve );		
		this->ReadSegment( assay , "Assay" ,"AssaySegment" );	
		this->ReadSegment( litho , "Lithology" ,"LithoSegment" );				
		this->Database->Close();	 
		} 

  return 1; 
}

// --------------------------------------
void vtkDrillHoleReader::ReadCurves( vtkPolyData *output )
{	
	vtkCellArray *cells = vtkCellArray::New();
	vtkPoints *points = vtkPoints::New();

	//use a list to keep track of the number of points, and the point Ids for each collar / curve
	vtkIdList *cellList = vtkIdList::New();

	//setup variables we want on the curves
	vtkStringArray *collarNames = vtkStringArray::New();
	collarNames->SetName("CollarId");

	vtkIntArray *id = vtkIntArray::New();
	id->SetName("Id");
		
	//so we can keep track of which cell we are reading
	int previousCollar=-1, currentCollar=0;
	
	vtkIdType currentPointId=0; //pulled out of while loop for speed
	vtkStdString name;	 //pulled out of while loop for speed
	vtkSQLiteQuery *query = (vtkSQLiteQuery *)this->Database->GetQueryInstance();
	vtkstd::stringstream buffer;	
	buffer << "SELECT Collar.hole_id,DrillCurve.id,DrillCurve.collar_id,DrillCurve.x,DrillCurve.y,DrillCurve.z";
	buffer << " FROM Collar INNER JOIN DrillCurve ON Collar.id=DrillCurve.collar_id";
	buffer << " ORDER BY DrillCurve.collar_id,DrillCurve.id";  // MUST be last field in query.
    
	// Retrieve all data in one query.
	query->SetQuery(buffer.str().c_str());
	query->Execute(); //run the query
  	
	while( query->NextRow() ) 
		{  
		// for each collar ...
		name = query->DataValue(0).ToString();   			
		if (this->DrillHoles->ArrayIsEnabled(name))
			{					
			//detect if we are on a new cell
			currentCollar = query->DataValue(2).ToInt();
			if (currentCollar != previousCollar) 
				{		
				//set the name map
				id->InsertNextValue(currentCollar);
				this->NameMap->SetValue(currentCollar, 1 );
				collarNames->InsertNextValue( name );

				if ( previousCollar != -1 )
					{					
					cellList->Squeeze();
					cells->InsertNextCell( cellList );					
					cellList->Reset();
					}

				//update previousCollar
				previousCollar = currentCollar;				
				}
						
			currentPointId =  points->InsertNextPoint( query->DataValue(3).ToFloat(), query->DataValue(4).ToFloat(), query->DataValue(5).ToFloat() );								
			cellList->InsertNextId( currentPointId );
			}		
		}					

	//have to make sure we actually have cells
	if (cells->GetNumberOfCells() > 0)
		{
		// close out final cell.	
		cellList->Squeeze();
		cells->InsertNextCell( cellList );

		output->SetPoints(points);
		output->SetLines(cells);					
			
		output->GetCellData()->AddArray( id );
		output->GetCellData()->AddArray( collarNames );		
		}
	
	//cleanup
	points->Delete();
	cells->Delete();
	collarNames->Delete();
	id->Delete();
	cellList->Delete();


	//close query
	this->CloseQuery( query );
}

// --------------------------------------
void vtkDrillHoleReader::ReadSegment( vtkPolyData *out, vtkStdString type, vtkStdString segment )
{	
	vtkPolyData *output = vtkPolyData::New();
	vtkInternalProps properties;
	vtkstd::stringstream buffer;	

	//figure out the properties that we are going to have
	vtkSQLiteQuery *query = (vtkSQLiteQuery *)this->Database->GetQueryInstance();	
	buffer << "pragma table_info (" << type <<")";
	query->SetQuery( buffer.str().c_str() );
  query->Execute();  

  while( query->NextRow() ) 
		{
		vtkAbstractArray *prop;	
		vtkStdString temp = query->DataValue(1).ToString();		
		if ( query->DataValue(2).ToString()=="TEXT" ) 
			{
			prop = vtkStringArray::New();			
			}
		else
			{
			prop = vtkDoubleArray::New();			
			}
		prop->SetName( query->DataValue(1).ToString() );
		properties.push_back( prop );    
		}



	vtkCellArray *cells = vtkCellArray::New();
	vtkPoints *points = vtkPoints::New();
	vtkIdType currentPointId;
	int collarId;
	double startPoint[3];
	double endPoint[3];

	//run through the actual table now
	query->SetQuery( this->ConstructSegmentQuery(type, segment) );
  query->Execute();

  while(query->NextRow()) 
		{
		// for each collar ...
		collarId = query->DataValue(9).ToInt();	
		if ( this->NameMap->GetValue( collarId ) == 1 )
			{		
			for (int i=0; i<3; i++) 
				{
				startPoint[i] = query->DataValue(2+i).ToDouble();
				endPoint[i] = query->DataValue(5+i).ToDouble();
				}

			//construct a cell with 2 points
			cells->InsertNextCell( 2 );
			currentPointId = points->InsertNextPoint(startPoint);
			cells->InsertCellPoint( currentPointId );
			currentPointId = points->InsertNextPoint(endPoint);
			cells->InsertCellPoint( currentPointId );
	       	
			//populate the property arrays
			vtkVariant value;
			for (unsigned int i=0; i< properties.size(); i++) 
				{
				// NOTE: The property values start at DataValue(8+i).
				// If just i is used, wrong values will be returned
				// If you ever modify the table layout, this WILL break
				value = query->DataValue(8+i);
				if ( properties.at(i)->IsNumeric() )
					{
					static_cast<vtkDoubleArray*>(properties.at(i))->InsertNextValue( value.ToDouble() );
					}
				else
					{
					static_cast<vtkStringArray*>(properties.at(i))->InsertNextValue( value.ToString() );
					}     
				}
			}
		}

	
	if ( cells->GetNumberOfCells() > 0 )
		{ 	
		output->SetPoints(points);
		output->SetLines(cells);
		//add properties to the segment
		for (unsigned int i=0; i < properties.size(); i++) 
			{
			output->GetCellData()->AddArray( properties.at(i) );
			}
		} 
       
	//cleanup
	points->Delete();
	cells->Delete();
	while( properties.size() > 0 )
    {
    vtkAbstractArray *toDel;
    toDel = properties.back();
		
		if ( toDel )
			{
			toDel->Delete();
			}
    properties.pop_back();    
    }
	
	//close query
	this->CloseQuery( query );
	if(this->CreatePointProp)
		{
		vtkCellDataToPointData *c2p =  vtkCellDataToPointData::New();
		c2p->SetInput(output);
		if(this->KeepCellProp)
			{
			c2p->PassCellDataOn();
			}
		else
			{
			c2p->PassCellDataOff();
			}
		c2p->Update();
		out->ShallowCopy(c2p->GetOutput());
		c2p->Delete();
		}
	else
		{
		out->ShallowCopy(output);
		}
	output->Delete();
}

// --------------------------------------
vtkStdString vtkDrillHoleReader::ConstructSegmentQuery( vtkStdString type, vtkStdString segment )
{
	vtkstd::stringstream buffer;	

	buffer << "SELECT " << segment <<".*," << type <<".* FROM " << type <<"";
  buffer << " INNER JOIN " << segment <<" ON " << type <<".id=" << segment;
	if ( type == "Assay")
		{
		buffer << ".assay_id";
		}
	else if ( type == "Lithology" )
		{
		buffer << ".litho_id";
		}
	buffer << " WHERE " << type <<".collar_id IN (SELECT id FROM Collar )";
	
	return buffer.str();
}


// --------------------------------------
bool vtkDrillHoleReader::OpenDatabase( )
{
					
  // Initialize SQLite connection and cursor  
	this->Database->SetDatabaseFileName( this->FileName );				
	return this->Database->Open(""); //open the DB for use		  		
}

// --------------------------------------
void vtkDrillHoleReader::CloseQuery(vtkSQLiteQuery *query )
{				
	query->SetQuery("");
	query->Execute();
	query->Delete(); 		
}
  

// -------------------------------------- 
void vtkDrillHoleReader::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "FileName: "
     << (this->FileName ? this->FileName : "(none)") << "\n";
}

//----------------------------------------------------------------------------
void vtkDrillHoleReader::SelectionModifiedCallback(vtkObject*, unsigned long,
                                             void* clientdata, void*)
  {
  static_cast<vtkDrillHoleReader*>(clientdata)->Modified();		
  }
//----------------------------------------------------------------------------
int vtkDrillHoleReader::GetDrillHoleArrayStatus(const char* name)
  {
	// if 'name' not found, it is treated as 'disabled'
  return (this->DrillHoles->ArrayIsEnabled(name));
  }
//----------------------------------------------------------------------------
// Called from within EnableAllArrays()
void vtkDrillHoleReader::SetDrillHoleArrayStatus(const char* name, int status)
  {
  if(status)
    {
    this->DrillHoles->EnableArray(name);
    }
  else
    {
    this->DrillHoles->DisableArray(name);
    }		
  }
//----------------------------------------------------------------------------
// Modified FROM vtkXMLReader.cxx
int vtkDrillHoleReader::SetFieldDataInfo(vtkDataArraySelection* CellDAS, 
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

void vtkDrillHoleReader::SetupOutputInformation(vtkInformation *outInfo)
  {
	// CellDataArraySelection is already prepared.  Don't need SetDataArraySelection()
  vtkInformationVector *infoVector = NULL;
  // Setup the Field Information for the Cell data
  if (!this->SetFieldDataInfo(this->DrillHoles,
                              vtkDataObject::FIELD_ASSOCIATION_CELLS, 
                              this->DrillHoleCount, 
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
int vtkDrillHoleReader::GetNumberOfDrillHoleArrays()
  {
  return this->DrillHoles->GetNumberOfArrays();
  }

//----------------------------------------------------------------------------
const char* vtkDrillHoleReader::GetDrillHoleArrayName(int index)
  {
  return this->DrillHoles->GetArrayName(index);
  }