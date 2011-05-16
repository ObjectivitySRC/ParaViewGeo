// .NAME MineSightReader.cxx
// Read MineSight Resource files.
#include "vtkMineSightReader.h"
#include "ci.h"
#include "cigeom.h"
#include "ciha.h"
#include "cimsr.h"
#include <list>
#include <string>
#include <iostream>

#include "vtkCellArray.h"
#include "vtkObjectFactory.h"
#include "vtkPoints.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkCellData.h"
#include "vtkSmartPointer.h"
#include "vtkIntArray.h"
#include "vtkFloatArray.h"
#include "vtkStringList.h"
#include "vtkLongArray.h"
#include <vtkOutputWindow.h>

vtkCxxRevisionMacro(vtkMineSightReader, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkMineSightReader);

// Constructor
vtkMineSightReader::vtkMineSightReader()
{
  this->FileName = NULL;
  this->SetNumberOfInputPorts(0);
};

// --------------------------------------
// Destructor
vtkMineSightReader::~vtkMineSightReader()
{
  this->SetFileName(0);
}

// --------------------------------------
void vtkMineSightReader::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "FileName: "
     << (this->FileName ? this->FileName : "(none)") << "\n";
}

// --------------------------------------
int vtkMineSightReader::RequestData(vtkInformation* request,
                          vtkInformationVector** inputVector,
                          vtkInformationVector* outputVector)
{
  // Make sure we have a file to read.
  if(!this->FileName)  {
    vtkErrorMacro("A FileName must be specified.");
    return 1;
  }
  if(strlen(this->FileName)==0)  {
    vtkErrorMacro("A NULL FileName.");
    return 1;
  }

  // Open the input file.
  GEOM* geom = ciGEOMObjectLoad(this->FileName);
  if(!geom)  {
    vtkErrorMacro("Error opening file " << this->FileName);
    return 1;
  }
  XYZ* orig = ciGEOMObjectGetOrigin(geom);
  int ElCnt = ciGEOMGetElementCount(geom);
  int MkCnt = ciGEOMGetElementMarkerCount(geom);
  int PlCnt = ciGEOMGetElementPolylineCount(geom);
  int PgCnt = ciGEOMGetElementPolygonCount(geom);
  int ShCnt = ciGEOMGetElementShellCount(geom);
/*** Info out ...
 FILE* fp = fopen("C:\\temp\\MineSightDebug.txt","w");
 fprintf(fp,"%d El, %d Mk, %d Pl, %d Pg, %d Sh\n",ElCnt,MkCnt,PlCnt,PgCnt,ShCnt);
 fclose(fp);
***/
  if (ElCnt==0) {
    vtkErrorMacro("No Elements to process for " << this->FileName);
    return 1;
  }
  vtkIdType nodes[3];
  vtkIdType* nodesptr;
  vtkSmartPointer<vtkPoints> myPointsPtr = vtkSmartPointer<vtkPoints>::New();
  vtkSmartPointer<vtkCellArray> myLCellsPtr = vtkSmartPointer<vtkCellArray>::New();
  vtkSmartPointer<vtkCellArray> myTCellsPtr = vtkSmartPointer<vtkCellArray>::New();
  vtkSmartPointer<vtkCellArray> myPtCells = vtkSmartPointer<vtkCellArray>::New();
  colorlistptr = vtkStringList::New();
  vtkIntArray* iap = vtkIntArray::New();  // to be used for final collection
  vtkIntArray* iapSH = vtkIntArray::New();  // for Shell Elements
  vtkIntArray* iapMK = vtkIntArray::New();  // for Marker Elements
  vtkIntArray* iapPL = vtkIntArray::New();  // for Polyline and Polygon Elements
	iapSH->Allocate(100*ShCnt+1);
	iapMK->Allocate(MkCnt+1);
	iapPL->Allocate(PlCnt+PgCnt+1);
  colorlistptr->AddString("1.00000 1.00000 1.00000");  // white 
  char cmfname[200], cmname[40], colorstr[30];
  strcpy(cmfname,this->FileName);
  char* cp = &cmfname[strlen(cmfname)];
  while (*cp!='.') *cp-- = '\00';
  *cp-- = '\00';
  while (*cp!='/' && *cp!='\\') cp--;
  strcpy(cmname,++cp);
  strcat(cmfname,"CM.xml");
  ((vtkDataArray*)iap)->SetName(cmname);
  ELEMENT* elem;
  XYZ* pts;
  FACE* faces;
  HA* haptr;
  RGB* colors;
  int facecnt, etype, ptid, ptid_base, colorindx, cindx=0;
	int ptcnt;
  ptid=0;

	// With Markers (points), Polylines (open), Polygons (closed), and Shells (triangulated faces,
	// it was difficult to get CellData and Property arrays of allign properly.
	// Rather than accumulating separate Point, Line and Surface CellArrays, 
	// using output->InsertNextCell(type,cnt,nodeslist) was attempted so the cells and
	// property array were built in parallel, but the polygons always picked up a stray
	// point from somewhere else on the object and couldn't be displayed correctly.
	// In the end, separate intArrays were kept for Markers, Polys and Shells, then
	// after using SetVerts(...), SetLines(...) <actually Polylines and Polygons> and
	// SetPolys(...) <Shells/Triangulated Faces>, the final data array was composed from the
	// individual intArrays in the same order as the cell types were set in the output.

	for (int i=0; i<ElCnt; i++) {
    elem = ciGEOMElementGetAt(geom, i);
    etype = ciGEOMElementGetType(elem);  // enum {UNK, polyline, polygon, marker, shell}
    pts = ciGEOMElementGetPoints(elem);
    haptr = ciGEOMElementGetHA(geom,elem);
    if (haptr!=NULL) {
      if (etype==elemMARKER) {
        colors = ciHAMarkerGetNodeColor(haptr);
      }
      else if (etype==elemPOLYLINE || etype==elemPOLYGON) {
        colors = ciHAPolyGetLineColor(haptr);
      }
      else if (etype==elemSHELL) {
        colors = ciHAShellGetFaceColor(haptr);
      }
      colorindx = ColorIndex(colors);
      cindx++;
    }
    ptid_base = ptid;
    ptcnt = ciGEOMElementGetPointCount(elem);
/*** Info out ...
 fp = fopen("C:\\temp\\MineSightDebug.txt","a");
 fprintf(fp,"Element # %d,  type=%d  point count=%d  face count=%d  colorindx=%d  ptid_base=%d\n",
   i,etype,ptcnt,ciGEOMElementGetFaceCount(elem),colorindx,ptid_base);
 fclose(fp);
***/
    nodesptr = new vtkIdType[ptcnt+1];
    for (int j=0; j<ptcnt; j++) {
      myPointsPtr->InsertPoint(ptid,pts[j].x,pts[j].y,pts[j].z);
      *(nodesptr+j) = ptid++;
    }

// Note: For Polygons, it is not necessary to append the start point close the polygon.

    if (etype==elemMARKER) {
      myPtCells->InsertNextCell(ptcnt,nodesptr);
      delete [] nodesptr;
      if (haptr!=NULL) iapMK->InsertNextValue(colorindx);
    }
    else if (etype==elemPOLYLINE) {
      myLCellsPtr->InsertNextCell(ptcnt,nodesptr);
      delete [] nodesptr;
      if (haptr!=NULL) iapPL->InsertNextValue(colorindx);
    }
    else if (etype==elemPOLYGON) {
      myLCellsPtr->InsertNextCell(ptcnt,nodesptr);
      delete [] nodesptr;
      if (haptr!=NULL) iapPL->InsertNextValue(colorindx);
    }
    else if (etype==elemSHELL) {
      faces = ciGEOMElementGetFaces(elem);
      facecnt = ciGEOMElementGetFaceCount(elem);
      for (int k=0; k<facecnt; k++) {
        nodes[0] = ptid_base+faces[k].i;
        nodes[1] = ptid_base+faces[k].j;
        nodes[2] = ptid_base+faces[k].k;
        myTCellsPtr->InsertNextCell(3, nodes);
        if (haptr!=NULL) iapSH->InsertNextValue(colorindx);
      }
			delete [] nodesptr;  // not used for SHELL
    }
  }
  ciGEOMObjectFree(geom);

	if (myLCellsPtr->GetSize()==0 && myTCellsPtr->GetSize()==0 && myPtCells->GetSize()==0) {
    vtkErrorMacro("No Cells created for " << this->FileName);
    return 0;
  }

	// Store the points and cells in the output data object.
  vtkPolyData* output = vtkPolyData::GetData(outputVector);
  output->SetPoints(myPointsPtr);
  if (MkCnt>0)
    output->SetVerts(myPtCells);
  if (myLCellsPtr->GetSize()>0)
    output->SetLines(myLCellsPtr);
  if (myTCellsPtr->GetSize()>0)
    output->SetPolys(myTCellsPtr);

	// For the current MineSight files, the single property (color index) is on CellData.
	iapSH->Squeeze();
	vtkIdType ShSize = iapSH->GetSize();
	iapPL->Squeeze();
	vtkIdType PlSize = iapPL->GetSize();
	iapMK->Squeeze();
	vtkIdType MkSize = iapMK->GetSize();

	iap->Allocate(ShSize+PlSize+MkSize+1);
	vtkIdType loopCnt;

	for (loopCnt=0; loopCnt<MkSize; loopCnt++) 
		iap->InsertNextValue(iapMK->GetValue(loopCnt));
	for (loopCnt=0; loopCnt<PlSize; loopCnt++) 
		iap->InsertNextValue(iapPL->GetValue(loopCnt));
	for (loopCnt=0; loopCnt<ShSize; loopCnt++) 
		iap->InsertNextValue(iapSH->GetValue(loopCnt));

	output->GetCellData()->AddArray(((vtkDataArray*)iap));

  int listlen = colorlistptr->GetNumberOfStrings();
  if (listlen==0) return 1;

	// Generate an xml colormap file for the object property.
  FILE* cmfp = fopen(cmfname,"w");
  fprintf(cmfp,"<ColorMap name=\"%s\" space=\"RGB\">\n",cmname);
  for (int cid=0; cid<listlen; cid++) {
    strcpy(colorstr,colorlistptr->GetString(cid));
    colorstr[7]='\00';
    colorstr[15]='\00';
    fprintf(cmfp,"  <Point x=\"%.2f\" o=\"1\" r=\"%s\" g=\"%s\" b=\"%s\" />\n",
      float(cid+0.5), &colorstr[0], &colorstr[8], &colorstr[16]);
    fprintf(cmfp,"  <Point x=\"%.2f\" o=\"1\" r=\"%s\" g=\"%s\" b=\"%s\" />\n",
      float(cid+1.49), &colorstr[0], &colorstr[8], &colorstr[16]);
  }
  fputs("</ColorMap>\n",cmfp);
  fclose(cmfp);
  colorlistptr->Delete();  // call RemoveAllItems() for complete cleanup.
  iap->Delete();
  iapMK->Delete();
  iapPL->Delete();
  iapSH->Delete();
  return 1;
}

int vtkMineSightReader::ColorIndex(RGB* ecolor)
{
  char colorstr[30];
  sprintf(colorstr,"%.5f %.5f %.5f",ecolor->r,ecolor->g,ecolor->b);
  int cindex = colorlistptr->GetIndex((const char*)colorstr);
  if (cindex>=0) return cindex+1;
  const char* ccp = new char[strlen(colorstr)+1];
  strcpy((char*)ccp,colorstr);
  colorlistptr->AddString(ccp);
  return colorlistptr->GetNumberOfStrings();
}
