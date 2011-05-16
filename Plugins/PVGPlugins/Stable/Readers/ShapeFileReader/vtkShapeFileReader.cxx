// .NAME vtkShapeFileReader.cxx
// Read Data Files (*.shp / *.shx) ESRI data files.
// POINT, LINE, POLYGON, MESH
// Without properties
#include "vtkShapeFileReader.h"
//#include "shpread.h"
#include "shapefil.h"
#include <stdlib.h>
#include <stdio.h>
#include <list>
#include <vector>
#include <string>
#include <iostream>

#include "mgdecl.h"
#include "hpolygon.h"

#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkObjectFactory.h"
#include "vtkPoints.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkSmartPointer.h"
#include "vtkFloatArray.h"
#include "vtkStringList.h"
#include "vtkLongArray.h"
#include <vtkOutputWindow.h>
#include <vtkDoubleArray.h>
#include <vtkIntArray.h>
#include <vtkStringArray.h>

// define Gocad file type identifers.
#define POINTOBJ 1
#define LINEOBJ 2
#define FACEOBJ 3
#define MESHOBJ 4

vtkCxxRevisionMacro(vtkShapeFileReader, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkShapeFileReader);

void insertAttributes(int nFields, map<int, vtkIntArray*>& intProps, 
																	 map<int, vtkDoubleArray*>& doubleProps, 
																	 map<int, vtkStringArray*>& stringProps, 
																	 DBFHandle dbfHandle, int record)
{
	for(map<int, vtkIntArray*>::iterator it = intProps.begin(); 
		it != intProps.end(); ++it)
	{
		int f = it->first;
		int value = DBFReadIntegerAttribute( dbfHandle, record, f );
		it->second->InsertNextValue(value);
	}

	for(map<int, vtkDoubleArray*>::iterator it = doubleProps.begin(); 
		it != doubleProps.end(); ++it)
	{
		int f = it->first;
		double value = DBFReadDoubleAttribute( dbfHandle, record, f );
		it->second->InsertNextValue(value);
	}

	for(map<int, vtkStringArray*>::iterator it = stringProps.begin(); 
		it != stringProps.end(); ++it)
	{
		int f = it->first;
		string value = DBFReadStringAttribute( dbfHandle, record, f );
		it->second->InsertNextValue(value);
	}
}


// Constructor
vtkShapeFileReader::vtkShapeFileReader()
{
  this->FileName = NULL;
  this->SetNumberOfInputPorts(0);
};

// --------------------------------------
// Destructor
vtkShapeFileReader::~vtkShapeFileReader()
{
  this->SetFileName(0);
}

// --------------------------------------
void vtkShapeFileReader::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "FileName: "
     << (this->FileName ? this->FileName : "(none)") << "\n";
}

// --------------------------------------
int vtkShapeFileReader::RequestData(vtkInformation* request,
                          vtkInformationVector** inputVector,
                          vtkInformationVector* outputVector)
{
  // Make sure we have a file to read.
  if(!this->FileName)  {
    vtkErrorMacro("A FileName must be specified.");
    return 0;
  }
  if(strlen(this->FileName)==0)  {
    vtkErrorMacro("A NULL FileName.");
    return 0;
  }

	//check for the .shx & .shp our selves.
	vtkstd::string shxName = this->FileName;
	int shxpos = shxName.find_last_of(".");
	shxName.replace(shxpos,4,".shx");
  ifstream shp(this->FileName);
	ifstream shx( shxName.c_str() );
	
	bool ReadFailed = false;
	if ( !shx.good() )
		{
		shx.close();
		shxName.replace(shxpos,4,".SHX");
		shx.open( shxName.c_str() );
		if ( !shx.good() )
			{
			ReadFailed = true;			
			vtkErrorMacro("Could not find the .SHX or .shx file " );
			}		
		}

	if ( !shp.good() )
		{
		vtkErrorMacro("Could not find the .shp file" );
		}
	shp.close();
	shx.close();

	if ( ReadFailed )
		{
		return 0;
		}	
	
	
  

  vtkIdType* nodes = NULL;
  vtkIdType fnodes[4];
  vtkSmartPointer<vtkPoints> myPointsPtr = vtkSmartPointer<vtkPoints>::New();
  vtkSmartPointer<vtkCellArray> myCellsPtr = vtkSmartPointer<vtkCellArray>::New();
  vtkSmartPointer<vtkCellArray> faceCellsPtr = vtkSmartPointer<vtkCellArray>::New();
  vtkFloatArray* fap = NULL;
  int filetype = 0;
  HPolygon poly;
  int nPolygons = 0;
  double baseZ;
	vtkLongArray* fapLAP = vtkLongArray::New();

  // Get SHP Handle (SHPInfo struct pointer.
  SHPHandle shpih = SHPOpen(this->FileName, "rb"); 
	if ( shpih == NULL )
		{
		//incase the .shx is missing or the shape file reader failed		
		fapLAP->Delete();
		return 0;
		}

	SHPObject* shpop;


  switch (shpih->nShapeType)
  {
  case SHPT_NULL:
    vtkErrorMacro("NULL Object type." << this->FileName);
    SHPClose(shpih);
		fapLAP->Delete();
    return 0;
    break;
  case SHPT_POINT:
  case SHPT_POINTZ:
  case SHPT_POINTM:
  case SHPT_MULTIPOINT:
  case SHPT_MULTIPOINTZ:
  case SHPT_MULTIPOINTM:
    filetype = POINTOBJ;
    break;
  case SHPT_ARC:  // Polyline
  case SHPT_ARCZ:
  case SHPT_ARCM:
    filetype = LINEOBJ;
    break;
  case SHPT_POLYGON:
  case SHPT_POLYGONZ:
  case SHPT_POLYGONM:
    filetype = FACEOBJ;  // (default) form triangulated surfaces.
    // arbitrary limit set to avoid triangulation overload (e.g. ArcGIS\ArcTutor\Map\airport.mdb.shp)
    for (int ri0=0; ri0<shpih->nRecords; ri0++) {
      shpop = SHPReadObject(shpih,ri0);
      if (shpop->nVertices > 100)  { filetype = LINEOBJ; break; } // form polygon outline only.
    }
    break;
  case SHPT_MULTIPATCH:
    filetype = MESHOBJ;
    nPolygons = 0;
    for (int ri0=0; ri0<shpih->nRecords; ri0++) {
      shpop = SHPReadObject(shpih,ri0);
      if (shpop->nSHPType!=SHPP_TRISTRIP && shpop->nSHPType!=SHPP_TRIFAN)
        nPolygons += shpop->nParts;
    }
    if (nPolygons>0)  poly.mtabSize.resize(nPolygons);
    break;
  default:
    vtkErrorMacro("Unknown Object type." << this->FileName);
    SHPClose(shpih);
		fapLAP->Delete();
    return 0;
  }
  	
  char errbuff[80];
  int pid=0, pidBase, ei, ei0, si, pi, vi, nodecnt, polypart=0;

	
	shxName.replace(shxpos,4,".dbf");
	DBFHandle dbfHandle = DBFOpen(shxName.c_str(), "rb"); // rb means: "read-only binary"
	//if(dbfHandle == NULL)
	//{
		//TODO: before returning, allocated memory should be deleted.
		//TODO: maybe the absence of the "dbf" file should be supported, in this case
		// only the geometry will be loaded.
		//vtkErrorMacro("coudn't find the dbf file");
		//return 0;
	//}

	//if(shpih->nRecords != dbfHandle->nRecords)
	//{
	//	vtkErrorMacro("the number of records in the shp file must be equal to the number of records in the dbf file");
	//	return 0;
	//}

	int nFields = 0;
	if(dbfHandle)
		nFields = DBFGetFieldCount(dbfHandle);

	map<int, vtkIntArray*> intProps;
	map<int, vtkDoubleArray*> doubleProps;
	map<int, vtkStringArray*> stringProps;

  char *pszFilename = NULL;
  int		nWidth, nDecimals;
  char	szTitle[12];
	
	if(dbfHandle)
	{
		// read the fields information from the dbf file and creates the associated arrays 
		for( int i = 0; i < nFields; ++i )
		{
			DBFFieldType	eType;
			char chNativeType;

			chNativeType = DBFGetNativeFieldType( dbfHandle, i );

			eType = DBFGetFieldInfo( dbfHandle, i, szTitle, &nWidth, &nDecimals );
			if( eType == FTInteger )
			{
				intProps[i] = vtkIntArray::New();
				intProps[i]->SetName(szTitle);
			}
			else if( eType == FTDouble )
			{
				doubleProps[i] = vtkDoubleArray::New();
				doubleProps[i]->SetName(szTitle);
			}
			else if( eType == FTString)
			{
				stringProps[i] = vtkStringArray::New();
				stringProps[i]->SetName(szTitle);
			}
		}
	}

  for (int ri=0; ri<shpih->nRecords; ri++) 
	{
    shpop = SHPReadObject(shpih,ri);  
    if (shpop->nSHPType != shpih->nShapeType && shpih->nShapeType != SHPT_MULTIPATCH) 
		{
      sprintf(errbuff,"Object type (%d) not match overall type (%d), skipped record %d",
        (int)shpop->nSHPType, (int)shpih->nShapeType, ri+1);
      vtkErrorMacro("Error: " << errbuff);
      continue;  // skip it.
    }
    switch (filetype)
    {
    case POINTOBJ:
      if (nodes!=NULL)  delete nodes;
      nodes = new vtkIdType[shpop->nVertices];
      for (vi=0; vi<shpop->nVertices; vi++) {
          myPointsPtr->InsertPoint(pid,shpop->padfX[vi],shpop->padfY[vi],shpop->padfZ[vi]);
					//insertAttributes(nFields, intProps, doubleProps, stringProps, dbfHandle, ri);
          nodes[vi] = pid++;
          // shpop->padfM[vi]);
      }  
      myCellsPtr->InsertNextCell(shpop->nVertices, nodes);
			insertAttributes(nFields, intProps, doubleProps, stringProps, dbfHandle, ri);
      delete nodes;
      nodes = NULL;
      break;
    case LINEOBJ:
      for (pi=0; pi<shpop->nParts; pi++) {
        si = shpop->panPartStart[pi];
        ei = (pi==shpop->nParts-1) ? shpop->nVertices : shpop->panPartStart[pi+1];
        if (nodes!=NULL)  delete nodes;
        nodes = new vtkIdType[ei-si];
        for (vi=si; vi<ei;vi++) {
          myPointsPtr->InsertPoint(pid,shpop->padfX[vi],shpop->padfY[vi],shpop->padfZ[vi]);
					//insertAttributes(nFields, intProps, doubleProps, stringProps, dbfHandle, ri);
          nodes[vi-si] = pid++;
        }
        myCellsPtr->InsertNextCell(ei-si, nodes);
				insertAttributes(nFields, intProps, doubleProps, stringProps, dbfHandle, ri);
      }  
      break;
    case FACEOBJ:						
      poly.mtabSize.resize(shpop->nParts);  // HGRD
      baseZ = shpop->padfZ[0];  // default Z for polygon(s) of this record.
      for (pi=0; pi<shpop->nParts; pi++) {
        si = shpop->panPartStart[pi];
        ei = (pi==shpop->nParts-1) ? shpop->nVertices : shpop->panPartStart[pi+1];
        ei0 = ei;
        if (ei>si+200)  { ei=si+200; nodecnt=201; }
        else  { nodecnt = ei-si; }
        poly.mtabSize[pi]=nodecnt;
				polypart++;
        for (vi=si; vi<ei;vi++) {
          poly.mtabPnt.insert( poly.mtabPnt.end(),Vect2D(shpop->padfX[vi],shpop->padfY[vi]) );
        }
        if (ei != ei0)
          poly.mtabPnt.insert( poly.mtabPnt.end(),Vect2D(shpop->padfX[si],shpop->padfY[si]) );
      }
      pidBase = pid;
      // *** DO TRIANGULATION ON poly(s) for this record AND BUILD FACE CELLS ***
      poly.Triangulate();
      for (int pti=0; pti<(int)poly.mtabPnt.size(); pti++)
			{
        myPointsPtr->InsertPoint(pid++,poly.mtabPnt[pti].X(),poly.mtabPnt[pti].Y(),baseZ);
				//insertAttributes(nFields, intProps, doubleProps, stringProps, dbfHandle, ri);
			}
      for (int tri=0; tri<(int)poly.mtabCell.size(); tri++) {
        for (int ni=0; ni<3; ni++)
          fnodes[ni] = pidBase + poly.mtabCell[tri].Index(ni);
        faceCellsPtr->InsertNextCell(3, fnodes);
				insertAttributes(nFields, intProps, doubleProps, stringProps, dbfHandle, ri);
      }
      break;
    case MESHOBJ:
      for (pi=0; pi<shpop->nParts; pi++) {
        if (shpop->nSHPType==SHPP_TRISTRIP || shpop->nSHPType==SHPP_TRIFAN) {
          pidBase = pid;
          si = shpop->panPartStart[pi];
          ei = (pi==shpop->nParts-1) ? shpop->nVertices : shpop->panPartStart[pi+1];
          for (vi=si; vi<ei;vi++) {
            myPointsPtr->InsertPoint(pid++,shpop->padfX[vi],shpop->padfY[vi],shpop->padfZ[vi]);
						//insertAttributes(nFields, intProps, doubleProps, stringProps, dbfHandle, ri);
          }
          switch (shpop->nSHPType)
          {
          case SHPP_TRISTRIP:
            for (int sti=pidBase+2; sti<pid; sti++) {
              fnodes[0] = sti-2;
              fnodes[1] = sti-1;
              fnodes[2] = sti;
              faceCellsPtr->InsertNextCell(3, fnodes);
							insertAttributes(nFields, intProps, doubleProps, stringProps, dbfHandle, ri);
            }
            break;
          case SHPP_TRIFAN:
            for (int sti=pidBase+2; sti<pid; sti++) {
              fnodes[0] = pidBase;
              fnodes[1] = sti-1;
              fnodes[2] = sti;
              faceCellsPtr->InsertNextCell(3, fnodes);
							insertAttributes(nFields, intProps, doubleProps, stringProps, dbfHandle, ri);
            }
            break;
          }
        }
        else {  // all polygon (RING) cases
          // Combine all Rings before triangulation to produce correct holes.
          baseZ = shpop->padfZ[0];  // default Z for ALL polygon(s).
          for (pi=0; pi<shpop->nParts; pi++) {
            si = shpop->panPartStart[pi];
            ei = (pi==shpop->nParts-1) ? shpop->nVertices : shpop->panPartStart[pi+1];
            poly.mtabSize[polypart++]=ei-si;
            for (vi=si; vi<ei;vi++) {
              poly.mtabPnt.insert( poly.mtabPnt.end(),Vect2D(shpop->padfX[vi],shpop->padfY[vi]) );
            }
          }
        }
      }
      break;
    }
    SHPDestroyObject(shpop);
  }
  if (filetype==MESHOBJ && nPolygons>0) {
    // *** DO TRIANGULATION ON accumulated poly(s) AND BUILD FACE CELLS ***
    pidBase = pid;
    poly.Triangulate();
    for (int pti2=0; pti2<(int)poly.mtabPnt.size(); pti2++)
		{
      myPointsPtr->InsertPoint(pid++,poly.mtabPnt[pti2].X(),poly.mtabPnt[pti2].Y(),baseZ);
			//insertAttributes(nFields, intProps, doubleProps, stringProps, dbfHandle, ri);
		}
    for (int tri2=0; tri2<(int)poly.mtabCell.size(); tri2++) {
      for (int ni=0; ni<3; ni++)
        fnodes[ni] = pidBase + poly.mtabCell[tri2].Index(ni);
      faceCellsPtr->InsertNextCell(3, fnodes);
			//insertAttributes(nFields, intProps, doubleProps, stringProps, dbfHandle, ri);
    }
  }
  SHPClose(shpih);
  if (nodes!=NULL)  { delete nodes; nodes=NULL; }

	int propcnt = 0;

  // Store the points and cells in the output data object.
  vtkPolyData* output = vtkPolyData::GetData(outputVector);
  output->SetPoints(myPointsPtr);
 
  if (filetype==POINTOBJ)
    output->SetVerts(myCellsPtr);
  else if (filetype==LINEOBJ)
    output->SetLines(myCellsPtr);
  else if (filetype==FACEOBJ) {
    output->SetPolys(faceCellsPtr);  // HGRD triangulated processing
  }
  else  // MESHOBJ
    output->SetPolys(faceCellsPtr);

  for (int pi=0; pi<propcnt; pi++)
    {
    ((vtkFloatArray*)fapLAP->GetValue(pi))->Delete();
    }
		fapLAP->Delete();


		for(map<int, vtkIntArray*>::iterator it = intProps.begin(); 
			it != intProps.end(); ++it)
		{
			output->GetCellData()->AddArray(it->second);
			it->second->Delete();
		}

		for(map<int, vtkDoubleArray*>::iterator it = doubleProps.begin(); 
			it != doubleProps.end(); ++it)
		{
			output->GetCellData()->AddArray(it->second);
			it->second->Delete();
		}

		for(map<int, vtkStringArray*>::iterator it = stringProps.begin(); 
			it != stringProps.end(); ++it)
		{
			output->GetCellData()->AddArray(it->second);
			it->second->Delete();
		}

		if(dbfHandle)
			DBFClose( dbfHandle );
  return 1;
}
