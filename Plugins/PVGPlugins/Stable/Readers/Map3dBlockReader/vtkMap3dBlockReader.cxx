#include "vtkMap3dBlockReader.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <ostream>

#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkObjectFactory.h"
#include "vtkPoints.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkSmartPointer.h"
#include "vtkIntArray.h"
#include "vtkStringArray.h"
#include "vtkStringList.h"
#include "vtkLongArray.h"
#include <vtkOutputWindow.h>

#define TRIAG 3
#define TRIAG_POINTS 9
#define X 0
#define Y 1
#define Z 2
//#define DEBUGM3D
#undef DEBUGM3D

vtkCxxRevisionMacro(vtkMap3dBlockReader, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkMap3dBlockReader);

// Constructor
vtkMap3dBlockReader::vtkMap3dBlockReader()
{
  this->FileName = NULL;
  this->SetNumberOfInputPorts(0);
};

// --------------------------------------
// Destructor
vtkMap3dBlockReader::~vtkMap3dBlockReader()
{
  this->SetFileName(0);
}

// --------------------------------------
void vtkMap3dBlockReader::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "FileName: "
     << (this->FileName ? this->FileName : "(none)") << "\n";
}

// --------------------------------------
int vtkMap3dBlockReader::RequestData(vtkInformation* request,
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

  vtkSmartPointer<vtkPoints> myPointsPtr = vtkSmartPointer<vtkPoints>::New();
  vtkSmartPointer<vtkCellArray> myCellsPtr = vtkSmartPointer<vtkCellArray>::New();
	vtkLongArray* iapLAP = vtkLongArray::New();
	vtkIntArray* BlkNumIAP = vtkIntArray::New();
	vtkIntArray* BlkTypeIAP = vtkIntArray::New();
	vtkIntArray* BlkIdIAP = vtkIntArray::New();
	vtkIntArray* iap = NULL;
	vtkIntArray* prev_iap = NULL;
	BlkNumIAP->SetName("BlkNumber");
	BlkTypeIAP->SetName("Type");
	BlkIdIAP->SetName("ActiveBlkId");
	vtkStringArray* sap = vtkStringArray::New();
	sap->SetName("BlkName");
	int tMC, propcnt = 1;

  string line;
  ifstream myfile (this->FileName);
	if (!myfile.is_open()) {
    vtkErrorMacro("Input file not opened.");
    return 0;
  }
  char inname[2048], inbuff[200], mcpropname[20];
  char blkspechdr[] = "*   N 'BLOCK NAME'";  // followed by a dash line and a blank line.
  int len_bsh = strlen(blkspechdr);
  char coordspechdr[] = "*   N   X";
  int len_csh = strlen(coordspechdr);
  int stage=0, s1cnt, s4cnt, ci, ii, ii2;
  int blkcnt=0, vertex_indx, iMS, msi, ablknm, blknm, blkid, blkcd, matcode;
	int blk_itbl[8], blk_type;
  vtkIdType* nodes = new vtkIdType[4];
	float xx, yy, zz;
	int blkfacetbl[5][4] = {0, 4, 5, 1,
													1, 5, 6, 2,
													2, 6, 7, 3,
													3, 7, 4, 0,
													4, 7, 6, 5};
#ifdef DEBUGM3D
	FILE* fpdb = fopen("C:\\temp\\m3d_db.txt","w");
	if (fpdb==NULL) {
		vtkErrorMacro("DEBUGM3D requires a C:\\temp folder.");
    return 0;
  }
	fputs("Starting Block spec processing.\n",fpdb);
	fclose(fpdb);
#endif
  while (! myfile.eof() ) //make sure the line exists
  {
    getline (myfile,line);
		strcpy(inbuff,line.c_str());
    while (strlen(inbuff)>0 && inbuff[strlen(inbuff)-1]<=' ')  inbuff[strlen(inbuff)-1] = '\00';
		switch (stage) {
    case 0:  // waiting for block header.
      if (strncmp(inbuff,blkspechdr,len_bsh)==0)  { stage=1; s1cnt=0; }
      break;
    case 1:  // skipping dash line and blank line.
      if (++s1cnt==2)  { blkcnt=0; stage=2; }
      break;
    case 2:  // process block specification lines.
      if (strlen(inbuff)<70)  { stage=3; break; }
      sscanf(inbuff," %d", &ablknm);
			blknm = ablknm%10;
      //ii = 20;
			ii2 = 0;
      while (inbuff[ii2++]!='\'');  // find start of name field
			ii = ii2;
			while (inbuff[ii++]!='\'');  // find end of name field
      if (ii>198)  break;
      strncpy(inname,&inbuff[ii2],(ii-ii2)-1);
      inname[(ii-ii2)-2] = '\0';
      while (strlen(inname)>0 && inname[strlen(inname)-1]==' ')  inname[strlen(inname)-1]='\0';
      if (strlen(inname)==0)  strcpy(inname,"blank");
			// Replace select characters in name with safe '_' character.
      for (ci=0; ci<(int)strlen(inname); ci++)  {
        if (inname[ci]==' ')
          inname[ci]='_';
        else if (inname[ci]=='/')
          inname[ci]='X';
        else if (inname[ci]=='@')
          inname[ci]='A';
      }
      //while (inname[strlen(inname)-1]=='_')  inname[strlen(inname)-1]='\00';  // clean up end of name
			for( int temp = (ii-ii2)-1; temp < strlen(inname); temp++)
				{
				inname[temp]='\0';
				}
      sscanf(&inbuff[ii]," %d %d %d %d %d %d %d %d %d",
        &blk_itbl[0], &blk_itbl[1], &blk_itbl[2], &blk_itbl[3],
        &blk_itbl[4], &blk_itbl[5], &blk_itbl[6], &blk_itbl[7],
        &blk_type);
			for (int ni=0; ni<4; ni++)  nodes[ni]=blk_itbl[ni];
			myCellsPtr->InsertNextCell(4,nodes);  // 0, 1, 2, 3 (Top)
			BlkNumIAP->InsertValue(blkcnt,blknm);
			BlkTypeIAP->InsertValue(blkcnt,blk_type);
			BlkIdIAP->InsertValue(blkcnt,ablknm);
			sap->InsertValue(blkcnt,string(inname));
			blkcnt++;
			if (blk_itbl[4]!=0) {  // need to construct the other 5 faces
				for (int facei=0; facei<5; facei++) {
					for (int nodei=0; nodei<4; nodei++) {
						nodes[nodei] = blk_itbl[blkfacetbl[facei][nodei]];
					}
					myCellsPtr->InsertNextCell(4,nodes);
					BlkNumIAP->InsertValue(blkcnt,blknm);
					BlkTypeIAP->InsertValue(blkcnt,blk_type);
					BlkIdIAP->InsertValue(blkcnt,ablknm);
					sap->InsertValue(blkcnt,string(inname));
					blkcnt++;
				}
			}
      break;
    case 3:  // wait for coordinate spec header
      if (strncmp(inbuff,coordspechdr,len_csh)==0)  { stage=4; s4cnt=0; }
      break;
    case 4:  // skipping dash line and blank line.
#ifdef DEBUGM3D
			fpdb = fopen("C:\\temp\\m3d_db.txt","a");
			fputs("Completed Block spec.\n",fpdb);
			fclose(fpdb);
#endif
      if (++s4cnt==2)  { stage=5; }
      break;
    case 5:  // process vertex coordinate specification lines.
      if (strlen(inbuff)<10 || strncmp(inbuff,"          ",10)==0)  { stage=6; break; }
      sscanf(inbuff," %d %f %f %f", &vertex_indx, &xx, &yy, &zz);
      myPointsPtr->InsertPoint(vertex_indx, xx, yy, zz);
      break;
// Minig Step Material Code Processing (case 6-8) NOT YET IMPLEMENTED ***
    case 6:  // find start of Mining Step Sequence
      if (strncmp(inbuff,"*   N    MC",11)==0)  { stage=7; }
      break;
    case 7:  // find next mining step
#ifdef DEBUGM3D
			fpdb = fopen("C:\\temp\\m3d_db.txt","a");
			fputs("Completed point specs.\n",fpdb);
			fclose(fpdb);
#endif
      if (strncmp(inbuff,"'Mining Step ",13)==0) {
        sscanf(&inbuff[13]," %d", &iMS);
				iap =vtkIntArray::New();
				sprintf(mcpropname,"MS%02d_MatCode",iMS);
				iap->SetName(mcpropname);
				vtkIdType aid = iapLAP->InsertNextValue((long)iap);
				if (iMS>1)
					prev_iap = (vtkIntArray*)iapLAP->GetValue(aid-1);
				for (int ci=0; ci<blkcnt; ci++) {
					if (iMS==1)  tMC=0;
					else tMC = prev_iap->GetValue(ci);
					iap->InsertValue(ci,tMC);
				}
        stage=8;
      }
      break;
    case 8:  // process mining step sequence block material spec
      if (strlen(inbuff)<10 || iMS>40)  { stage=7;  break; }  // end of current mining step
      sscanf(inbuff," %d %d",&blkid, &matcode);
      blkcd = blkid%256;
			for (int ci1=0; ci1<blkcnt; ci1++) {
				if (BlkIdIAP->GetValue(ci1) == blkid)
					iap->SetValue(ci1,matcode);
			}
      break;
    }
  }
  myfile.close();
#ifdef DEBUGM3D
	fpdb = fopen("C:\\temp\\m3d_db.txt","a");
	fputs("Completed Mining step processing.\n",fpdb);
	fclose(fpdb);
#endif

	vtkPolyData* output = vtkPolyData::GetData(outputVector);
  output->SetPoints(myPointsPtr);
  output->SetPolys(myCellsPtr);
	output->GetCellData()->AddArray(BlkNumIAP);
	output->GetCellData()->AddArray(BlkTypeIAP);
	output->GetCellData()->AddArray(BlkIdIAP);
	BlkNumIAP->Delete();
	BlkTypeIAP->Delete();
	BlkIdIAP->Delete();
	for (msi=0; msi<iMS; msi++) {
		output->GetCellData()->AddArray(((vtkIntArray*)iapLAP->GetValue(msi)));
		((vtkIntArray*)iapLAP->GetValue(msi))->Delete();
	}
	iapLAP->Delete();
	output->GetCellData()->AddArray(sap);
	sap->Delete();

  return 1;
}

