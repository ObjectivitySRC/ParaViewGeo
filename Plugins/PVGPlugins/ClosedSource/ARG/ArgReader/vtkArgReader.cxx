/*=========================================================================

  Program:   ParaView
  Module:    $RCSfile: vtkArgReader.cxx,v $

  Copyright (c) Robert Maynard
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkArgReader.h"

#include "vtkCellArray.h"
#include "vtkCompositeDataPipeline.h"
#include "vtkDataSet.h"
#include "vtkIdList.h"
#include "vtkInformation.h"
#include "vtkMultiBlockDataSet.h"
#include "vtkMultiPieceDataSet.h"
#include "vtkObjectFactory.h"
#include "vtkPolyData.h"
#include "vtkStringArray.h"
#include "vtkXMLDataElement.h"
#include "vtkXMLUtilities.h"

//includes needed to get datamine support
#include "../../../Stable/Readers/DataMineReader/vtkDataMineWireFrameReader.cxx"
#include "../../../Stable/Readers/DataMineReader/vtkDataMineReader.cxx"
#include "../../../Stable/Readers/DataMineReader/dmfile.cxx"
#include "../../../Stable/Readers/DataMineReader/PointMap.cxx"
#include "../../../Stable/Readers/DataMineReader/PropertyStorage.cxx"


#include <sstream>
#include <iostream>
#include <fstream>
#include <vtkstd/vector>
#include <vtkstd/map>


using vtkstd::map;
using vtkstd::vector;

#define OUTFILE_SEP ","
#define GUID_ARRAY_NAME "GUID"

vtkCxxRevisionMacro(vtkArgReader, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkArgReader);

//empty namespace so we don't conflict with other files
namespace
{

#define AddBlock( mblock, method, pos ){\
	vtkPolyData *temp = vtkPolyData::New();\
	method(temp);\
	mblock->SetBlock( pos, temp );\
	temp->Delete();}\


//----------------------------------------------------------------------------
// BASIC STRING FUNCTIONS
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
//WILL CLEAR THE LIST THAT IS PASSED IN!
void split(const vtkstd::string &s, const char &delim, vtkstd::vector<vtkstd::string> &elems) 
{	
	elems.clear();
  vtkstd::stringstream ss(s);
  vtkstd::string item;
  while(vtkstd::getline(ss, item, delim)) 
		{
    elems.push_back(item);
		}    
}

//----------------------------------------------------------------------------
int toInt( const vtkstd::vector<vtkstd::string> &s, const int &index )
{
	vtkstd::stringstream buffer;
	int temp;
	buffer << s.at( index );
	buffer >> temp;
	return temp;
}
float toFloat( const vtkstd::vector<vtkstd::string> &s, const int &index )
{
	vtkstd::stringstream buffer;
	float temp;
	buffer << s.at( index );
	buffer >> temp;
	return temp;
}

float toDouble( const vtkstd::vector<vtkstd::string> &s, const int &index )
{
	vtkstd::stringstream buffer;
	double temp;
	buffer << s.at( index );
	buffer >> temp;
	return temp;
}

// trim from end
static inline vtkstd::string &rtrim(std::string &str) 
	{
  size_t endpos = str.find_last_not_of(" \t");
	if( vtkstd::string::npos != endpos )
		{
    str = str.substr( 0, endpos+1 );
		}
	return str;
	}

typedef vector<vtkStdString> PropertiesNames;
typedef map < vtkStdString, int > PropertiesType;
typedef map < vtkStdString, vtkstd::vector< vtkstd::string > > storeProp;
	
} //end of namespace

struct Internal_filesNames
{
	PropertiesNames shrunksNames;
};
//use to contain all properties + their names
struct Internal_Properties
{
	PropertiesNames names; //just contain the properties names 
	PropertiesType types; //if the propertie is a string or numerical
	PropertiesType Status; //contain the status of the properties selection 
	storeProp Store;
	int nstring;
	int ndouble;
};

//----------------------------------------------------------------------------
vtkArgReader::vtkArgReader()
{
	this->StartDate = NULL;
	this->PathName = 0; 
	this->ArgCellDataArraySelection= vtkDataArraySelection::New();	
	this->PUNOProperties= new Internal_Properties;
	this->DOTProperties=new Internal_Properties;
	this->DMstatus= false;
	this->Punostatus= false;
	this->Dotstatus= false;
	this->PunoLinksListOfFileName= new Internal_filesNames;
	this->PunoPointsListOfFileName= new Internal_filesNames;
}

//----------------------------------------------------------------------------
vtkArgReader::~vtkArgReader()
{
	if(this->ArgCellDataArraySelection!=NULL)
		this->ArgCellDataArraySelection->Delete();

	if(this->PUNOProperties!=NULL)
	{
		map < vtkStdString, vtkstd::vector< vtkstd::string > >::iterator it;
		
		if(!this->PUNOProperties->Store.empty())
		{			
			it = this->PUNOProperties->Store.begin();
			while ( it != this->PUNOProperties->Store.end() )
			{
				it->second.clear();
			++it;
			}	
			this->PUNOProperties->Store.clear();
		}
		if(!this->PUNOProperties->names.empty())
			this->PUNOProperties->names.clear();
		if(!this->PUNOProperties->Status.empty())
			this->PUNOProperties->Status.clear();
		if(!this->PUNOProperties->types.empty())
			this->PUNOProperties->types.clear();
		delete this->PUNOProperties;
	}
	
	if(this->DOTProperties!=NULL)
	{
		map < vtkStdString, vtkstd::vector< vtkstd::string > >::iterator it;
		
		if(!this->DOTProperties->Store.empty())
		{			
			it = this->DOTProperties->Store.begin();
			while ( it != this->DOTProperties->Store.end() )
			{
				it->second.clear();
			++it;
			}	
			this->DOTProperties->Store.clear();
		}
		if(!this->DOTProperties->names.empty())
			this->DOTProperties->names.clear();
		if(!this->DOTProperties->Status.empty())
			this->DOTProperties->Status.clear();
		if(!this->DOTProperties->types.empty())
			this->DOTProperties->types.clear();
		delete this->DOTProperties;
	}
	if(this->PunoLinksListOfFileName!=NULL)
	{
		if(!this->PunoLinksListOfFileName->shrunksNames.empty())
			this->PunoLinksListOfFileName->shrunksNames.clear();
		delete this->PunoLinksListOfFileName;
	}
	if(this->PunoPointsListOfFileName!=NULL)
	{
		if(!this->PunoPointsListOfFileName->shrunksNames.empty())
			this->PunoPointsListOfFileName->shrunksNames.clear();
		delete this->PunoPointsListOfFileName;
	}
		
}

//----------------------------------------------------------------------------
void vtkArgReader::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
int vtkArgReader::FillOutputPortInformation(
  int vtkNotUsed(port), vtkInformation* info)
{
  info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkMultiBlockDataSet");
  return 1;
}

//----------------------------------------------------------------------------
const char* vtkArgReader::GetDataSetName()
{
  return "vtkArgDataSet";
}

//----------------------------------------------------------------------------
void vtkArgReader::CreatePunoFilesName()
{
	vtkXMLDataElement *configElement = vtkXMLUtilities::ReadElementFromFile(this->FileName);
	vtkXMLDataElement *element;  
	vtkstd::vector< vtkstd::string > lineItems;	
	vtkstd::vector< vtkstd::string >::iterator it;	
	const char* value;
	char* temp;
	int size=0;
	if(this->PunoLinksListOfFileName->shrunksNames.empty())
		{	
		element = configElement->LookupElementWithName("Links"); //to get the stopes file path
	
		if(element != NULL)
			value = element->GetAttributeValue(0);
		split(value, ';', lineItems);
		
		for(it=lineItems.begin(); it!=lineItems.end(); it++)
			{	
			size = strlen(this->PathName)+ (*it).size();
			temp= new char[size+1];
			strcpy(temp,this->PathName);
			strcat(temp,(*it).c_str());
			temp[size]='\0';
			this->PunoLinksListOfFileName->shrunksNames.push_back(vtkStdString(temp));
			//delete[] temp;
			}
		}
	if(this->PunoPointsListOfFileName->shrunksNames.empty())
		{
		element = configElement->LookupElementWithName("Points"); //to get the stopes file path	
		if(element != NULL)
			value = element->GetAttributeValue(0);	
		split(value, ';', lineItems);

		for(it=lineItems.begin(); it!=lineItems.end(); it++)
			{	
			size = strlen(this->PathName)+ (*it).size();
			temp= new char[size+1];
			strcpy(temp,this->PathName);
			strcat(temp,(*it).c_str());
			temp[size]='\0';
			this->PunoPointsListOfFileName->shrunksNames.push_back(vtkStdString(temp));
			//delete[] temp;
			}
		}
}

//----------------------------------------------------------------------------
void vtkArgReader::CreatePunoFiles()
{
	char* link= new char[strlen(this->PathName)+15];
	strcpy(link,this->PathName);
	strcat(link,"PUNO/Links.csv");
	link[strlen(this->PathName)+14]='\0';
	this->PunoLinksFileName = vtkStdString(link);

	vtkstd::ofstream linkFile ( this->PunoLinksFileName );
	int maxvalue=0, newMaxValue=0;
	int levelId=0;
	vtkstd::string line;
	vtkstd::vector< vtkstd::string > lineItems; 
	

	vector<vtkStdString>::iterator it;	
	for(int i=0;i<this->PunoLinksListOfFileName->shrunksNames.size(); i++)
		{		
		vtkstd::ifstream file ( this->PunoLinksListOfFileName->shrunksNames.at(i), vtkstd::ifstream::in );		
		getline( file, line ); 
		if(i==0)
		  linkFile << line << endl;
		while ( file.good() )
		{			
		getline( file, line );	
		split( line, ',', lineItems );
		if ( lineItems.size() < 1 )
			{
			continue;
			}
			if( lineItems.at(1).size()>0)
			levelId = maxvalue + toInt(lineItems,1);
			else levelId = maxvalue;
			if(levelId > newMaxValue) newMaxValue= levelId;
			
			for(int j=0; j<lineItems.size(); j++)
			{
				if(j==1)
				linkFile<<levelId;
				else linkFile<<lineItems.at(j);
				if(j<(lineItems.size()-1))
					linkFile<< ",";
				else linkFile<< endl;
			}			
		}	
		maxvalue = newMaxValue;
		file.close();
		linkFile.flush( );
	}
	linkFile.close();

	char* points= new char[strlen(this->PathName)+16];
	strcpy(points,this->PathName);
	strcat(points,"PUNO/Points.csv");
	points[strlen(this->PathName)+15]='\0';
	this->PunoPointsFileName = vtkStdString(points);

	vtkstd::ofstream pointsFile ( this->PunoPointsFileName );
	maxvalue=0;
	newMaxValue=0;

	for(int i=0;i<this->PunoPointsListOfFileName->shrunksNames.size(); i++)
		{		
		vtkstd::ifstream file ( this->PunoPointsListOfFileName->shrunksNames.at(i), vtkstd::ifstream::in );		
	  getline( file, line );
		if(i==0)
			pointsFile << line << endl;
		while ( file.good() )
		{	
		getline( file, line );	
		split( line, ',', lineItems );
		if ( lineItems.size() == 0 )
			{
			continue;
			}
			if( (lineItems.at(1)).size()>0)
			levelId = maxvalue + toInt(lineItems,1);
			else levelId = maxvalue;
			if(levelId > newMaxValue) newMaxValue= levelId;
			
			for(int j=0; j<lineItems.size(); j++)
			{
				if(j==1)
				pointsFile<<levelId;
				else pointsFile<<lineItems.at(j);
				if(j<(lineItems.size()-1))
					pointsFile<< ",";
				else pointsFile<< endl;
			}		
		}
		maxvalue = newMaxValue;
		file.close();
		pointsFile.flush( );
	}
	pointsFile.close();

}
//----------------------------------------------------------------------------
void vtkArgReader::ReadComposite(vtkXMLDataElement* element, vtkCompositeDataSet* composite, const char* filePath, unsigned int &dataSetIndex)
{
	this->CreatePunoFiles(); //create the global files Points and Links of PUNO
	vtkStdString tempPoints, tempLink;
	vtkstd::map< vtkStdString, vtkStdString *> properties;
	properties[vtkStdString("Links")] = &tempLink;
	properties[vtkStdString("Points")] = &tempPoints;
	properties[vtkStdString("Asc")] = &this->DotAscFileName;
	properties[vtkStdString("Rpt")] = &this->DotRptFileName;
	properties[vtkStdString("Stopes")] = &this->DataMineFileName;	
	properties[vtkStdString("OutFile")] = &this->OutFileName;	

	//do we really need a multiblock?
  vtkMultiBlockDataSet* mblock = vtkMultiBlockDataSet::SafeDownCast(composite);
	vtkXMLDataElement* childXML;
  unsigned int numElems = element->GetNumberOfNestedElements();	

  for (unsigned int i=0; i < numElems; ++i)
    {		
    childXML = element->GetNestedElement(i);
    if (!childXML || !childXML->GetName() )
			{ 
			continue;
			}     				
		this->GetXMLFilePath( childXML, filePath, properties[childXML->GetName()] );				     		
		}		

	//make sure everything was found
	if ( tempLink == "" || tempPoints == "" ||
		this->DotAscFileName == "" || this->DotRptFileName == "" || this->DataMineFileName == "" )
		{
		vtkErrorMacro("Bad XML File.");
		return;
		}

	ActivityStorage pointStorage; //hack for decline->dev links
	ActivityStorage storage;
	this->CreateStopeActivities( storage );
	this->CreateDevelopmentActivities( storage );
	this->CreateDeclineActivities( storage );

	/*
	ToDo:		
		create graph of activities
	*/

	AddBlock( mblock, this->CreateDeclineGeometry, 0 );
	AddBlock( mblock, this->CreateDevelopmentGeometry, 1 );
	AddBlock( mblock, this->CreateStopeGeometry, 2 );

	this->WriteOutputFile( storage );
}
//----------------------------------------------------------------------------
void vtkArgReader::GetXMLFilePath( vtkXMLDataElement* xmlElem, const char* filePath, vtkStdString *&prop )
{	
	vtkstd::stringstream buffer;
	// Construct the name of the internal file.
  const char* file = xmlElem->GetAttribute("file"); 
  buffer << filePath;
	if ( buffer.str().length() )
		{
		buffer << "/";
		}
	buffer << file; 	
	if ( prop )
		{	
		prop->assign( buffer.str() );
		}	
}
//----------------------------------------------------------------------------
void vtkArgReader::CreateDeclineGeometry( vtkPolyData *decline  )
{
	vtkPoints *points = vtkPoints::New();
	vtkCellArray *lines = vtkCellArray::New();	
	vtkIdList *pointIds = vtkIdList::New();
	
	vtkstd::ifstream asc ( this->DotAscFileName , vtkstd::ifstream::in );	
	vtkstd::string line;
	vtkstd::vector< vtkstd::string > lineItems;		
	float x,y,z;
	int stage=0;
	vtkIdType cellSize=0;
	vtkStdString prevGuid, currGuid;

	//read the properties in the rpt file and map them on the Decline output
	vtkstd::ifstream rptfile ( this->DotRptFileName , vtkstd::ifstream::in );	
	 while ( rptfile.good() )
	 {		
		getline( rptfile, line );		
		split( line, ',', lineItems );
		if ( lineItems.size() == 0 )
			{
			continue;
			}
		if(strcmp((lineItems.at(0)).c_str(),"2")==0)
			{
			this->DOTProperties->Store.insert (this->DOTProperties->Store.begin(),
				vtkstd::pair<vtkStdString, vtkstd::vector< vtkstd::string >>(lineItems.at(1), lineItems) );
			}
	 }
	rptfile.close();

	vtkDataArray** PDecArrays= new vtkDataArray*[this->DOTProperties->ndouble]; 	
	vtkStringArray** SDecArrays= new vtkStringArray*[this->DOTProperties->nstring];
	int D=0, S=0;
	for(int p=0; p<this->DOTProperties->names.size(); p++)
	{
		if(this->DOTProperties->types[this->DOTProperties->names.at(p)]==0)
		{
			PDecArrays[D]= vtkDoubleArray::New();
			PDecArrays[D]->SetName((this->DOTProperties->names.at(p)).c_str());
			D++;
		}
		else {
			SDecArrays[S]= vtkStringArray::New();
			SDecArrays[S]->SetName((this->DOTProperties->names.at(p)).c_str());
			S++;
		}
	}

  while ( asc.good() )
		{				
		getline( asc, line );		
		split( line, ',', lineItems );

		if ( lineItems.size() == 0 )
			{						
			stage = 0;			
			if ( pointIds->GetNumberOfIds() == 1 )
				{
				//we have to update the last line with another point
				lines->InsertCellPoint( pointIds->GetId( 0 ) );
				lines->UpdateCellCount(  ++cellSize );
				pointIds->Reset( );
				}
			continue;
			}
		
		//parse the guid out
		currGuid = lineItems.at(1);		
		x = toFloat(lineItems,2);
		y = toFloat(lineItems,3);
		z = toFloat(lineItems,4);		

		if ( currGuid != prevGuid )
			{									
			stage++;				
			}

		if ( stage == 3 )
			{
			stage = 1;		
			lines->InsertNextCell( pointIds );		
			cellSize = pointIds->GetNumberOfIds( );
			pointIds->Reset();			
			
			D=0, S=0;
			for(int p=0; p<this->DOTProperties->names.size(); p++)
				{
					if(this->DOTProperties->types[this->DOTProperties->names.at(p)]==0)
					{
						if(this->DOTProperties->Status[this->DOTProperties->names.at(p)]==1)
							{
							if( (this->DOTProperties->Store[prevGuid].at(p+1)).size()>0)
							PDecArrays[D]->InsertNextTuple1(toDouble( this->DOTProperties->Store[prevGuid], (p+1)));
							else PDecArrays[D]->InsertNextTuple1(0);
							}
						D++;
					}
					else{ 	
						if(this->DOTProperties->Status[this->DOTProperties->names.at(p)]==1)
							SDecArrays[S]->InsertNextValue( this->DOTProperties->Store[prevGuid].at(p+1));
						S++;
					}
				}
			}	
		prevGuid = currGuid;		
		pointIds->InsertNextId( points->InsertNextPoint( x,y,z ) ); 
		}

	asc.close();
	pointIds->Delete();
	decline->SetPoints( points );
	decline->SetLines( lines );	

	D=0, S=0;
	for(int p=0; p<this->DOTProperties->names.size(); p++)
		{
		if(this->DOTProperties->types[this->DOTProperties->names.at(p)]==0)
			{
			if(this->DOTProperties->Status[this->DOTProperties->names.at(p)]==1)			
				decline->GetCellData( )->AddArray( PDecArrays[D] );
			PDecArrays[D]->Delete();			
			D++;
			}
			else
			{
			if(this->DOTProperties->Status[this->DOTProperties->names.at(p)]==1)
				decline->GetCellData( )->AddArray( SDecArrays[S] );		
			SDecArrays[S]->Delete();		
			S++;	
			}			
		}	
	delete[] PDecArrays;
	delete[] SDecArrays;
	points->Delete();
	lines->Delete();
}

//----------------------------------------------------------------------------
void vtkArgReader::CreateDevelopmentGeometry( vtkPolyData *dev )
{	
	vtkPoints *points = vtkPoints::New();	
	vtkstd::string line;
	vtkstd::vector< vtkstd::string > lineItems;		
	
	vtkstd::ifstream pointsFile ( this->PunoPointsFileName, vtkstd::ifstream::in );
	LevelPointMap map;
	vtkIdType id;
	int levelId,pointId,spointId;
	float x,y,z;
	
	/*NOTE: specify the string properties...the default is double..
	but those lines will be replace by one function to search
	when the properties are string or not*/
	this->PUNOProperties->types[this->PUNOProperties->names.at(0)]=1;
	this->PUNOProperties->types[this->PUNOProperties->names.at(4)]=1;
	this->PUNOProperties->types[this->PUNOProperties->names.at(5)]=1;
	this->PUNOProperties->types[this->PUNOProperties->names.at(6)]=1;
	this->PUNOProperties->nstring= 4;
	this->PUNOProperties->ndouble= this->PUNOProperties->names.size()-4;


	//create point 
	getline( pointsFile, line ); //load the header into a MAP in the future
  while ( pointsFile.good() )
		{		
		getline( pointsFile, line );
		split( line, ',', lineItems ); 				
		if ( lineItems.size() == 0 )
			{
			continue;
			}

		levelId = toInt(lineItems,1);
		pointId = toInt(lineItems,4);
		
		x = toFloat(lineItems,5);
		y = toFloat(lineItems,6);
		z = toFloat(lineItems,7);
		id = points->InsertNextPoint( x,y,z );
		map.AddPoint( levelId, pointId, id );	
		}

	dev->SetPoints( points );
	points->Delete();
	pointsFile.close();

	//create cells
	vtkstd::ifstream cellsFile ( this->PunoLinksFileName, vtkstd::ifstream::in );
	vtkCellArray *lines = vtkCellArray::New();
	vtkIdList *pointList = vtkIdList::New();
	
	vtkDataArray** Parrays= new vtkDataArray*[this->PUNOProperties->ndouble]; 	
	vtkStringArray** Sarrays= new vtkStringArray*[this->PUNOProperties->nstring];
	int D=0, S=0;
	for(int p=0; p<this->PUNOProperties->names.size(); p++)
	{
		if(this->PUNOProperties->types[this->PUNOProperties->names.at(p)]==0)
		{
			Parrays[D]= vtkDoubleArray::New();
			Parrays[D]->SetName((this->PUNOProperties->names.at(p)).c_str());
			D++;
		}
		else {
			Sarrays[S]= vtkStringArray::New();
			Sarrays[S]->SetName((this->PUNOProperties->names.at(p)).c_str());
			S++;
		}
	}
	
	getline( cellsFile, line );
	while ( cellsFile.good() )
		{
		getline( cellsFile, line );
		split( line, ',', lineItems );
		if ( lineItems.size() == 0 )
			{
			continue;
			}
		
		levelId = toInt(lineItems,1);
		pointId = toInt(lineItems,2);
		spointId = toInt(lineItems,3);	
		
		D=0, S=0;
		for(int p=0; p<this->PUNOProperties->names.size(); p++)
			{
				if(this->PUNOProperties->types[this->PUNOProperties->names.at(p)]==0)
				{
					if(this->PUNOProperties->Status[this->PUNOProperties->names.at(p)]==1)
					{
						if( (lineItems.at(p+1)).size()>0)
						Parrays[D]->InsertNextTuple1(toDouble( lineItems, p)); 
						else Parrays[D]->InsertNextTuple1(0);
					}
					D++;
				}
				else{ 	
					if(this->PUNOProperties->Status[this->PUNOProperties->names.at(p)]==1)
						Sarrays[S]->InsertNextValue(lineItems.at(p));
					S++;
				}
			}	

		pointList->InsertNextId( map.GetPointId( levelId, pointId ) );
		pointList->InsertNextId( map.GetPointId( levelId, spointId ) );
		lines->InsertNextCell( pointList );
		pointList->Reset();		
		}
	cellsFile.close();
	pointList->Delete();

	dev->SetLines( lines );

	D=0, S=0;
	for(int p=0; p<this->PUNOProperties->names.size(); p++)
		{
		if(this->PUNOProperties->types[this->PUNOProperties->names.at(p)]==0)
			{
			if(this->PUNOProperties->Status[this->PUNOProperties->names.at(p)]==1)			
				dev->GetCellData( )->AddArray( Parrays[D] );
			Parrays[D]->Delete();			
			D++;
			}
			else
			{
			if(this->PUNOProperties->Status[this->PUNOProperties->names.at(p)]==1)
				dev->GetCellData( )->AddArray( Sarrays[S] );		
			Sarrays[S]->Delete();		
			S++;	
			}			
		}	
	delete[] Parrays;
	delete[] Sarrays;

	lines->Delete();
}

//----------------------------------------------------------------------------
void vtkArgReader::CreateStopeGeometry( vtkPolyData *stopes )
	{
	vtkDataMineWireFrameReader *dmWFReader = vtkDataMineWireFrameReader::New( );
	//all datamine properties name shoulbe be 8 character long

	dmWFReader->SetFileName( this->DataMineFileName );
	dmWFReader->SetCellDataArraySelection(this->ArgCellDataArraySelection);	
	dmWFReader->Update( );
	stopes->ShallowCopy( dmWFReader->GetOutput( ) );	
	dmWFReader->Delete( );

	/*
	vtkStringArray* testingArray= vtkStringArray::SafeDownCast(stopes->GetCellData()->GetAbstractArray("GUID"));
	int nbcells = testingArray->GetNumberOfValues();
	vtkstd::ofstream myfile ( "toto.txt");
	for(int i=0; i<nbcells; i++)
		{
		myfile<< testingArray->GetValue(i) << endl;
		//vtkWarningMacro(<< testingArray->GetValue(i));
		}
		myfile.close(); */
	}

//----------------------------------------------------------------------------
void vtkArgReader::CreateDeclineActivities( ActivityStorage &storage )
{	
	vtkstd::multimap< vtkStdString, Activity*> guidMap; //used to find connections between dot main ramp and offshoots
	vtkstd::multimap< vtkStdString, Activity*>::iterator gmIt;
	vtkstd::pair< vtkstd::multimap< vtkStdString, Activity*>::iterator, 
	vtkstd::multimap< vtkStdString, Activity*>::iterator > gmRange;
	
	this->CreateDevelopmentRelations( storage, guidMap );

	//------------
	// STAGE 2:
	//	Parse Dot asc, and generate connections between dot activies, and 
	//	dev -> development
	//------------
	vtkstd::ifstream asc ( this->DotAscFileName , vtkstd::ifstream::in );	
	vtkstd::string line;
	vtkstd::vector< vtkstd::string > lineItems;			
	
	int stage=0,levelId=0,pointId=0;
	Activity *prevAct = NULL, *currAct = NULL;
	vtkStdString prevGuid, currGuid;
	bool possibleConnection = false;

  while ( asc.good() )
		{				
		getline( asc, line );		
		split( line, ',', lineItems );

		if ( lineItems.size() == 0 )
			{			
			//empty line means we have hit a new level						
			
			//empty line means we take the last guid and connect it to the 
			// previous guid as an activity. the kicker is that the guid should have
			//all ready been added to the guid secondary map, which stores the 1 -> 3 relationships!
			gmIt = guidMap.find(prevGuid.c_str( ));
			if ( gmIt != guidMap.end() )
				{				
				currAct->AddSuccessor( gmIt->second );				
				}	

			levelId++;			
			pointId=0;
			stage = 0;
			prevAct = NULL;
			currAct = NULL;
			possibleConnection = false;
			
			continue;
			}
		
		//parse the guid out
		currGuid = lineItems.at(1);		
		
		if ( pointId == 0 )
			{
			//only first points in a 'level' can connect to a development		
			possibleConnection = true;		
			}		
		
		if ( stage == 1 )
			{
			stage = 0;			
			currAct = new Activity( DECLINE, levelId, pointId++ ); //doesn't leak since it will be delete when storage goes out of scope
			currAct->SetGUID( currGuid );

			//add in the prevGuid to this act for				
			guidMap.insert( vtkstd::pair<vtkStdString, Activity*>(prevGuid, currAct) );		

			if ( prevAct )
				{
				//connect this activity to its predecessor
				prevAct->AddSuccessor( currAct );
				}
			if ( possibleConnection )
				{								
				//look up in secondary map for connection!	
				//decline can connect to multiple developments, so we need to find all key|value matches					
				gmRange = guidMap.equal_range( prevGuid.c_str( ) );				
				for ( gmIt = gmRange.first; gmIt != gmRange.second; ++gmIt)
					{
					gmIt->second->AddSuccessor( currAct );											
					}													
				}
			storage.AddActivity( currAct );
			prevAct = currAct;
			possibleConnection = false;
			}		 	

		//check for a new guid, which means add a new activity!
		if ( currGuid != prevGuid )
			{						
			prevGuid = currGuid;
			stage++;		
			}
		}
	asc.close();
}

//----------------------------------------------------------------------------
void vtkArgReader::CreateDevelopmentRelations( ActivityStorage &storage, vtkstd::multimap< vtkStdString, Activity* > &guidMap  )
	{	
	//------------
	// STAGE 1:
	//	Re-Parse Development, need to generate CoordinateActivities, so we can
	// actually connect decline to development in stage 2
	//------------

	vtkstd::string line;
	vtkstd::vector< vtkstd::string > lineItems;			
		
	vtkstd::ifstream pointsFile ( this->PunoPointsFileName, vtkstd::ifstream::in );
	LevelGuidMap map; //needed to figure out links between start and end points
		
	//create point MAP
	int levelId=0,pointId=0;
	vtkStdString guid;
	getline( pointsFile, line ); //load the header into a MAP in the future
  while ( pointsFile.good() )
		{		
		getline( pointsFile, line );
		split( line, ',', lineItems ); 				
		if ( lineItems.size() == 0 )
			{
			continue;
			}

		levelId = toInt(lineItems,1);
		pointId = toInt(lineItems,4);
		guid = lineItems.at( 0 );		
		map.AddGuid( levelId, pointId, guid );					
		}
	pointsFile.close();

	//create Link between locator and storage of activites
	Activity *realAct;
	int spointId=0;	
	vtkstd::ifstream cellsFile ( this->PunoLinksFileName, vtkstd::ifstream::in );	
	getline( cellsFile, line );
	while ( cellsFile.good() )
		{
		getline( cellsFile, line );
		split( line, ',', lineItems );
		if ( lineItems.size() == 0 )
			{
			continue;
			}
		
		levelId = toInt(lineItems,1);
		pointId = toInt(lineItems,2);
		spointId = toInt(lineItems,3);
				
		guid = map.GetGuid( levelId, spointId );		
		realAct = storage.GetActivity( DEVELOPMENT, levelId, pointId);	
		if ( realAct )
			{
			//guids are shared between the decline asc file and the puno points file
			guidMap.insert( guidMap.begin( ), vtkstd::pair< vtkStdString, Activity *> ( guid, realAct ) );
			}
		}	
	cellsFile.close();
	}

//----------------------------------------------------------------------------
void vtkArgReader::CreateDevelopmentActivities( ActivityStorage &storage )
{
	const int POINT_ID1_POSITION = 3;	

	//we need to open the links file first
	vtkstd::ifstream linksFile ( this->PunoLinksFileName , vtkstd::ifstream::in );	
	vtkstd::string line;
	vtkstd::vector< vtkstd::string > lineItems;
	int pointId;	
	int levelId;

	getline( linksFile, line ); //skip header
  while ( !linksFile.eof() )
		{				
		getline( linksFile, line );		//read next line
		split( line, ',', lineItems );  //split the line

		if ( lineItems.size() < 1 )
			{			
			//bad line, or empty line. so ignore it
			continue;
			}

		levelId = toInt( lineItems, 1 );		
		pointId = toInt( lineItems, 2 ); //make this a map so it 'robust'		

		//create the activity
		Activity *act = new Activity( DEVELOPMENT, levelId, pointId);		
		act->SetGUID( lineItems.at( 0 ) );	
		storage.AddActivity( act ); 	
		}

	//now have to add Development -> Development links and the Development -> Stopes links
	linksFile.clear( );
	linksFile.seekg( 0, vtkstd::ios::beg ); //have to clear before you seek and after 
	linksFile.clear( );
	int linkId;	

	getline( linksFile, line );
	while ( !linksFile.eof() )
		{
		getline( linksFile, line );
		split( line, ',', lineItems );  //split the line, will clear lineItems
		if ( lineItems.size() == 0 )
			{
			continue;
			}
		
		levelId = toInt( lineItems, 1 );		
		pointId = toInt( lineItems, 2 ); //make this a map so it 'robust'		
		linkId = toInt( lineItems, 3 ); 

		Activity *act = storage.GetActivity( DEVELOPMENT, levelId, pointId);

		Activity *other = storage.GetActivity( STOPES, levelId, pointId ); 
		if ( other )
			{
			other->AddSuccessor( act ); 
			}

		other = storage.GetActivity( DEVELOPMENT, levelId, linkId );
		if ( other )
			{
			act->AddSuccessor( other );	
			}		
		}	
  
	linksFile.close();
}

//----------------------------------------------------------------------------
void vtkArgReader::CreateStopeActivities( ActivityStorage &storage )
{	
	vtkstd::ifstream pointsFile ( this->PunoPointsFileName , vtkstd::ifstream::in );
	vtkstd::string line;
	vtkstd::vector< vtkstd::string > lineItems;	
	
	vtkstd::string stopeGuid;
	
	int pointId;	
	int levelId;	

	getline( pointsFile, line ); //load the header into a MAP in the future
  while ( pointsFile.good() )
		{		
		getline( pointsFile, line );		//read next line
		split( line, ',', lineItems );  //split the line, will clear lineItems

		if ( lineItems.size() < 1 )
			{			
			//bad line, or empty line. so ignore it
			continue;
			}

		stopeGuid = lineItems.at(3);
		levelId = toInt( lineItems, 1 );		
		pointId = toInt( lineItems, 4 ); //make this a map so it 'robust'		

		if ( stopeGuid.size( ) > 1 ) //1 for " "
			{			
			Activity *act = new Activity( STOPES, levelId, pointId);														
			act->SetGUID( stopeGuid ); 			
			storage.AddActivity( act ); //storage will delete the DevelopmentActivity Pointer						
			}					
		}	 	

	//cleanup
	pointsFile.close();			
}

//----------------------------------------------------------------------------
void vtkArgReader::WriteOutputFile( ActivityStorage &storage )
	{
	//write out the standard header
	vtkstd::ofstream ofile ( this->OutFileName );
	
	//header is hardcoded as I have no reports otherwise ( rmaynard, sep 16 2009 )
	ofile << "1,Project Details"<< endl;
	ofile << "2,Key,Value"<< endl;

	if ( this->StartDate )
		{
		ofile << "3,StartDate,"<< this->StartDate << endl;
		}
	else
		{
		ofile << "3,StartDate,2009-01-01"<< endl;
		}
	ofile << "3,Title,Mirarco"<< endl;
	ofile.flush( );

	//PST files have op codes that are the first item on each line.
	//1 = new section
	//2 = header
	//3 = data to be parsed with that header

	//write out all the activities
	this->DumpStopesToFile( ofile );	
	this->DumpDevelopmentToFile( ofile );	
	this->DumpDeclineToFile( ofile );
	
	//write out the activity list
	ofile << storage.GenerateDependencies( );						
	ofile.close( );
	}

//----------------------------------------------------------------------------
void vtkArgReader::DumpStopesToFile( vtkstd::ofstream &ofile )
{
	//datamine vars
	vtkStdString temp;
	TDMFile *file = new TDMFile();	
	file->LoadFileHeader( this->DataMineFileName.c_str( ) );		
	file->OpenRecVarFile( this->DataMineFileName.c_str( ) );			

	const int nVars = file->nVars;
	const int numRecords = file->GetNumberOfRecords( );
	int *isNumeric = new int[ nVars ]; //1 = numeric, 0 = first row in a string, -1 = in a multi col string 
	Data *values = new Data[ nVars ];
	char *varName = new char[9]; //datamine rows are only 8 characters long, ever.
	char *prevName = new char[9];

	//vars needed to add { to guids
	int sguid=-1, guidLength=-1;

	ofile << "1,Stopes" << endl; //print code to signal new block
	ofile << "2,Description,"; // synatx candy needed for file format
	for (int i=0; i< nVars; ++i) //write out header info, and create numeric mapping
		{
		temp = file->Vars[i].GetName(varName);
		isNumeric[i] = file->Vars[i].TypeIsNumerical( );
		if ( strncmp( varName, prevName, 8 ) != 0 )
			{
			ofile << rtrim(temp);	
			if ( i < nVars-1 )
				{
				ofile << OUTFILE_SEP; //no trailing comma
				}
			strcpy( prevName, varName );
			}
		else if ( isNumeric[i] == 0 )
			{
			isNumeric[i] = -1; //signal that this is a row that is part of a larger string
			}					

		if (strncmp(varName,"GUID",4)==0)
			{
			guidLength++;
			if ( sguid < 0 )
				{
				sguid = i;			
				}
			}	
		}	
	ofile << endl;	
	for ( int i=0; i < numRecords; ++i ) //dump the file into the stream
		{
		file->GetRecVars( i, values );
		ofile << "3,Stope,"; // syntax candy needed, plus the description type is currently handcoded in
		vtkstd::stringstream buffer;

		//Note: We can only trust the first 4 characters in the string of values. I have seen
		//cases where the last 4 are filed with 'bad' info that needs to be ignored
		isNumeric[0] == 1? ofile << values[0].v : ofile << vtkstd::string(values[0].c).substr(0,4);		
		for ( int j=1; j < nVars; ++j )
			{						
			if ( isNumeric[j] != -1 ) //need to loop from 1 to print out the commas in the right spot
				{
				ofile << OUTFILE_SEP;						
				}		
			isNumeric[j] == 1? ofile << values[j].v : ofile << vtkstd::string(values[j].c).substr(0,4);		
			} 
		ofile << endl;		
		ofile.flush( );
		}
	delete[] varName;
	delete[] prevName;
	delete[] isNumeric;
	delete[] values;
	file->CloseRecVarFile();
	delete file;

}

//----------------------------------------------------------------------------
void vtkArgReader::DumpDevelopmentToFile( vtkstd::ofstream &ofile )
{
	vtkstd::string line;
	vtkstd::ifstream file ( this->PunoLinksFileName, vtkstd::ifstream::in );		
	ofile <<"1,PUNO" << endl;
	getline( file, line ); //read the header
	ofile << "2,Description," << line << endl; //write the header
	while ( file.good() )
		{	
		getline( file, line );
		if ( line.length() > 0 )
			{
			ofile << "3,PUNO," << line << endl;
			}
		}
	file.close();
	ofile.flush( );
}

//----------------------------------------------------------------------------
void vtkArgReader::DumpDeclineToFile( vtkstd::ofstream &ofile )
{
	vtkstd::string line;
	vtkstd::ifstream file ( this->DotRptFileName, vtkstd::ifstream::in );
	ofile << "1,Decline"<<endl;
	ofile << "2,Description,GUID,groupID,barrier_intersection,x,y,z,bearing,description,length,gradient,devel_cost,haul_cost,level_cost,total_cost" << endl; //bug #1794 for reference to this order
	while ( file.good( ) )
		{
		getline( file, line );
		if ( line.size() > 0 && line.at(0) == '2' )
			{
			line.replace(0,1,"3"); //replace the dot data_type, with our mode, since data type is not needed
			//only lines that start with 2 are lines that we want to print
			//welcome to magic number land, enjoy your stay, and be thankfull that I documented this at all.			

			line.insert(2,"Decline,"); //have to add in a description component after the first comma
			ofile << line << endl; 
			}
		}
	file.close();
	ofile.flush();
}

// --------------------------------------
int vtkArgReader::RequestInformation(vtkInformation* request,
                          vtkInformationVector** inputVector,
                          vtkInformationVector* outputVector)
{
	//update property list
	this->Superclass::RequestInformation(request, inputVector, outputVector);
	this->SplitFileName();
	this->CreatePunoFilesName();
	this->UpdateDataSelection();	
	return 1;		
}

//----------------------------------------------------------------------------
void vtkArgReader::SetArraySelection(const char*name, int status)
{	
	vtkStdString temp= name;
	if(status<3)
		{
		if (!this->ArgCellDataArraySelection->ArrayExists(name))
			{		
			this->ArgCellDataArraySelection->AddArray(name);
			this->ArgCellDataArraySelection->DisableArray(name);	
			this->DMHeaders+= "0"+ temp+ "|";
			if(!this->DMstatus) this->DMstatus=true;
			}
		}
	else if( (status>=3)&&(status<=4))
		{
			if(this->PUNOProperties->Status.find(temp)==this->PUNOProperties->Status.end())
			{
				this->PUNOProperties->Status.insert (this->PUNOProperties->Status.begin(),
																						vtkstd::pair<vtkStdString, int>(temp, 0) );
				this->PUNOProperties->types.insert (this->PUNOProperties->types.begin(),
																						vtkstd::pair<vtkStdString, int>(temp, 0) );
				this->PUNOProperties->names.push_back(temp);
				this->PunoHeaders+= "1"+ temp+ "|";
				if(!this->Punostatus) this->Punostatus=true;
			}
		}
	else if( (status>=5)&&(status<=6))
		{
			if(this->DOTProperties->Status.find(temp)==this->DOTProperties->Status.end())
			{
				this->DOTProperties->Status.insert (this->DOTProperties->Status.begin(),
																						vtkstd::pair<vtkStdString, int>(temp, 0) );
				this->DOTProperties->types.insert (this->DOTProperties->types.begin(),
																						vtkstd::pair<vtkStdString, int>(temp, 0) );
				this->DOTProperties->names.push_back(temp);
				this->DotHeaders+= "2"+ temp+ "|";
				if(!this->Dotstatus) this->Dotstatus=true;
			}
		}

	if(status==0)
		this->ArgCellDataArraySelection->DisableArray(name);
	else if ( (status>0) && (status<=2))
		this->ArgCellDataArraySelection->EnableArray(name);
	else if(status==3)
		this->PUNOProperties->Status[vtkStdString(name)]= 0;
	else if(status==4)
		this->PUNOProperties->Status[vtkStdString(name)]= 1;
	else if(status==5)
		this->DOTProperties->Status[vtkStdString(name)]= 0;
	else if(status==6)
		this->DOTProperties->Status[vtkStdString(name)]= 1;
	
	this->Modified();
}

// --------------------------------------
void vtkArgReader::UpdateDataSelection()
{		
		vtkXMLDataElement *configElement = vtkXMLUtilities::ReadElementFromFile(this->FileName);
		vtkXMLDataElement *element;  
		element = configElement->LookupElementWithName("Stopes"); //to get the stopes file path
		const char* value;
		if(element != NULL)
		{
			value = element->GetAttributeValue(0);
		}
		char* name= new char[strlen(this->PathName)+strlen(value)+1];
		strcpy(name,this->PathName);
		name[strlen(this->PathName)]='\0';
		strcat(name,value);

		if(!this->DMstatus)
		{
			TDMFile* dmFile = new TDMFile();	
			if (!dmFile->LoadFileHeader(name) )
				{
				return;
				}
			char *varname = new char[256]; 
			for (int i=0; i<dmFile->nVars; i++)
				{
				dmFile->Vars[i].GetName(varname);
				
				if (!this->ArgCellDataArraySelection->ArrayExists(varname))
					{		
					this->ArgCellDataArraySelection->AddArray(varname);
					this->ArgCellDataArraySelection->DisableArray(varname);	
					this->DMHeaders += "0"+ vtkStdString(varname) + "|";
					}
				} 
			delete[] varname;
			delete[] name;
			delete dmFile;
			this->DMstatus = true;
		}

	if(!this->Punostatus)
		{
			if(this->PunoLinksListOfFileName->shrunksNames.empty())
			{
			vtkErrorMacro( << "Need to specify a Puno Link FileName" );
			}
		else
			{
			vtkstd::ifstream linksFile ( this->PunoLinksListOfFileName->shrunksNames.at(0), vtkstd::ifstream::in );
			vtkstd::string line;
			vtkstd::vector< vtkstd::string > lineItems;			

			getline( linksFile, line );
			split( line, ',', lineItems );
			vtkstd::vector< vtkstd::string >::iterator it;	
			for(it=lineItems.begin(); it!=lineItems.end(); it++)
				{
				this->PUNOProperties->names.push_back(*it);
				this->PUNOProperties->types.insert (this->PUNOProperties->types.begin(), vtkstd::pair<vtkStdString, int>(*it, 0) );
				this->PUNOProperties->Status.insert (this->PUNOProperties->Status.begin(), vtkstd::pair<vtkStdString, int>(*it, 0) );
				this->PunoHeaders+= "1"+ vtkStdString(*it)+ "|";
				}
			//determine wich property is string or numeric property
 			getline( linksFile, line );
			split( line, ',', lineItems );
			vtkStdString myString;
			int currentP=0;
			this->PUNOProperties->nstring =0;

			for(it=lineItems.begin(); it!=lineItems.end(); it++)
				{	
				myString = *it;
				if( myString != "" && this->isNumeric( myString ) )
					{
					this->PUNOProperties->types[this->PUNOProperties->names.at(currentP)]=0;
					}
				else if( myString != "")
					{
					this->PUNOProperties->types[this->PUNOProperties->names.at(currentP)]=1;
					this->PUNOProperties->nstring++;
					}
				currentP++;
				}
			this->PUNOProperties->ndouble= this->PUNOProperties->names.size()-this->PUNOProperties->nstring;
			this->Punostatus = true;
			linksFile.close();
			}
		}
	
	if(!this->Dotstatus)
		{
		vtkstd::ifstream rptfile ( this->PunoLinksListOfFileName->shrunksNames.at(0), vtkstd::ifstream::in );	
		vtkstd::string line;
		vtkstd::vector< vtkstd::string >::iterator it;
		vtkstd::vector< vtkstd::string > lineItems;
		line = "GUID,groupID,barrier_intersection,x,y,z,bearing,description,length,gradient,devel_cost,haul_cost,level_cost,total_cost";

		split( line, ',', lineItems );	
	
		for(it=lineItems.begin(); it!=lineItems.end(); it++)
			{
			this->DOTProperties->names.push_back(*it);
			this->DOTProperties->types.insert (this->DOTProperties->types.begin(), vtkstd::pair<vtkStdString, int>(*it, 0) );
			this->DOTProperties->Status.insert (this->DOTProperties->Status.begin(), vtkstd::pair<vtkStdString, int>(*it, 0) );
			this->DotHeaders+= "2"+ vtkStdString(*it)+ "|";
			}
		//determine wich property is string or numeric property
 		getline( rptfile, line );
		split( line, ',', lineItems );
		if(strcmp(lineItems.at(0).c_str(),"2")!=0)
		{
			getline( rptfile, line );
			split( line, ',', lineItems );
		}
	
		vtkStdString myString;
		int currentP=0;
		this->DOTProperties->nstring =0;
		for(it=lineItems.begin(); it!=lineItems.end(); it++)
			{	
			myString = *it;
			if( myString != "" && this->isNumeric( myString ) )
				{
				this->DOTProperties->types[this->DOTProperties->names.at(currentP)]=0;
				}
			else if( myString != "")
				{
				this->DOTProperties->types[this->DOTProperties->names.at(currentP)]=1;
				this->DOTProperties->nstring++;
				}
			currentP++;
			}

			this->DOTProperties->ndouble= this->DOTProperties->names.size() -this->DOTProperties->nstring;
			this->Dotstatus =true;
			rptfile.close();
		}
		this->ArraysInfo = new char[ this->DMHeaders.size()+this->PunoHeaders.size()+this->DotHeaders.size()];
		strcpy(this->ArraysInfo, this->DMHeaders.c_str());
		strcat(this->ArraysInfo, this->PunoHeaders.c_str());	
		strcat(this->ArraysInfo, this->DotHeaders.c_str());	
	
	this->SetupOutputInformation( this->GetOutputPortInformation(0) );		
}

void vtkArgReader::SplitFileName()
{
  if(!this->FileName)
    {
    vtkErrorMacro( << "Need to specify a FileName" );
    return;
    }

  // Pull the PathName component out of the FileName.
  int length = strlen(this->FileName);
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

//----------------------------------------------------------------------------
bool vtkArgReader::isNumeric(vtkStdString value)
{
	if( (value.at(0)!='-') &! isdigit(value.at(0)) )
	{
		return false;
	}

	for(unsigned int i=1; i<value.length(); i++)
	{
		if( (!isdigit(value[i])) &! (value[i] == '.') )
		{
			return false;
		}
	}
	return true;
	}



#undef OUTFILE_SEP
