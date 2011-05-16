
#include "vtkMap3dWriter.h"
#include "vtkInformation.h"
#include "vtkObjectFactory.h"
#include "vtkPolyData.h"
#include "vtkDataArray.h"
#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkPointData.h"
#include "vtkStringArray.h"
#include "vtkSmartPointer.h"
#include "vtkStdString.h"
#include "vtkCellDataToPointData.h"
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>
#include <map>
#include <iomanip>
#include <list>
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"

using namespace std;


void writeSegment(ofstream& myFile, 
				map<int,vector<pair<int*,int>>>::iterator &segment,
				int Type, double Thickness, double Width);

 void writeBlock(ofstream& myFile, map<int,
		vector<pair<int*,int>>>::iterator &segment,
		int Type, double Thickness, double Width);

void writeFace(ofstream &myFile, vector<pair<int*,int>>::iterator &cell,
							 int Type, double Thickness, double Width);
void writeTriangle(ofstream &myFile, vector<pair<int*,int>>::iterator &cell,
									 int Type, double Thickness, double Width);


vtkCxxRevisionMacro(vtkMap3dWriter, "$Revision: 1.27 $");
vtkStandardNewMacro(vtkMap3dWriter);

vtkMap3dWriter::vtkMap3dWriter()
{
	this->SegmentID = NULL;
	this->MiningBlk = NULL;

}

vtkMap3dWriter::~vtkMap3dWriter()
{ 
}

/*
int vtkMap3dWriter::RequestData ( vtkInformation *vtkNotUsed ( request ),
				vtkInformationVector **inputVector, vtkInformationVector *outputVector )
{

  // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);

  // get the input and ouptut
  input = vtkPolyData::SafeDownCast(
    inInfo->Get(vtkDataObject::DATA_OBJECT()));


	return 1;
}
*/


void vtkMap3dWriter::WriteData()
{

	// Output stream that will be used to write the data to the file
	ofstream myFile;
	// Grab object data from paraview
	input = vtkPolyData::SafeDownCast(this->GetInput());

	// input returns NULL if the data is not Polygonal
	if(input == NULL)
	{
		vtkErrorMacro("Writer only accepts PolyData, input type invalid.");
		return;
	}


	myFile.open(this->FileName);
	if(!myFile)
	{
		vtkErrorMacro("Error writing to the file");
		return;
	}

	myFile<<
"* ------------------------------------------------------------------------------\n"
"* MAP3D Version  1.51\n"
"* ------------------------------------------------------------------------------\n"
"* PROJECT TITLE - one line of data (maximum 70 characters)     \n"
"* ------------------------------------------------------------------------------\n"
"                                                               \n"
"'South Mine 830 Ore Body                                              2005:04:19'\n"
"\n"
"                                                               \n"
"* ------------------------------------------------------------------------------\n"
"* CONTROL PARAMETERS - one line of data                        \n"
"* ------------------------------------------------------------------------------\n"
"* NLD  - number of load steps (10000)                          \n"
"* NIT  - number of iterations (10000)                          \n"
"* NPS  - number of planes of symmetry (0)                      \n"
"* RPAR - maximum relaxation parameter (1.2)                    \n"
"* STOL - stress tolerance (0.1% of far field stress)                   [MPa:psi]\n"
"* AG   - minimum grid side length (dimension of interest)          [metres:feet]\n"
"* AL   - minimum element side length (dimension of interest)       [metres:feet]\n"
"* DOL  - D/L ratio for grid-element discretization    (1)      \n"
"* DON  - D/L ratio for element-element discretization (0.5)    \n"
"* DOC  - D/L ratio for coefficient lumping            (1)      \n"
"* DOE  - D/L ratio for element-grid lumping           (2)      \n"
"* DOG  - D/L ratio for grid-element lumping           (2)      \n"
"* DOR  - maximum element aspect ratio                 (5)      \n"
"* ------------------------------------------------------------------------------\n"
"* NLD,NIT,NPS, RPAR,STOL, AL,AG must be specified              \n"
"* DOL,DON,DOC,DOE,DOG,DOR are optional                         \n"
"* ------------------------------------------------------------------------------\n"
"*  NLD   NIT NPS RPAR   STOL      AL      AG    DOL    DON    DOC   DOE    DOG  \n"
"* ------------------------------------------------------------------------------\n"
"                                                               \n"
"  10000  10000   0  1.0    0.030000    30.000    20.000    1.0    0.5    1.0    2.0    2.0    5.0\n"
"                                                               \n"
"* ------------------------------------------------------------------------------\n"
"* BLOCK SPECIFICATION LIST - one line per block - end list with N=0 \n"
"* ------------------------------------------------------------------------------\n"
"* N - block identification number - also defines colour 1,6,11  etc. ... blue  \n"
"*                                                       2,7,12  etc. ... green \n"
"*                                                       3,8,13  etc. ... yellow\n"
"*                                                       4,9,14  etc. ... red   \n"
"*                                                       5,10,15 etc. ... grey  \n"
"* 'BLOCK NAME' - maximum of 20 characters must appear in single quotes\n"
"* I1,I2,I3,I4             - coordinate numbers of corners of plates\n"
"* I1,I2,I3,I4,I5,I6,I7,I8 - coordinate numbers of corners of blocks\n"
"* TYPE - block type -  1 for Fictitious Force elements - excavation surfaces   \n"
"*                      2 for Displacement Discontinuites - fault planes        \n"
"*                     98 for inactive blocks (excavations)     \n"
"*                     99 for inactive planes (faults)          \n"
"* THICKNESS - thickness for TYPE 2 blocks                          [metres:feet]\n"
"* WIDTH - maximum width                                            [metres:feet]\n"
"* ------------------------------------------------------------------------------\n"
"* N, I1,I2,I3,I4 must be specified                             \n"
"*    I5,I6,I7,I8,TYPE,THICNESS,SPACING,'BLOCK NAME' are optional\n"
"* ------------------------------------------------------------------------------\n"
"*   N 'BLOCK NAME'              I1    I2    I3    I4    I5    I6    I7    I8 TYPE   THICK WIDTH\n"
"* ------------------------------------------------------------------------------\n"<<endl;


	this->inPoints = input->GetPoints();
	this->inCells = input->GetPolys();
	this->BlkNum = input->GetCellData()->GetArray(this->SegmentID);
	if(BlkNum == NULL)
	{
		vtkErrorMacro(" Can't find SegmentID array ");
		return;
	}
	if(this->WithActivities)
	{
		this->MiningBlock = input->GetCellData()->GetArray(this->MiningBlk);
		if(MiningBlock == NULL)
		{
			vtkErrorMacro(" Can't find Mining Block array ");
			//return;
		}
	}

	(inCells)->InitTraversal();
	vtkIdType *npts = new vtkIdType();	// will store the number of points in current cell 
	vtkIdType *pts;	// will store a array of points id's in current cell 
	int num;
	int MBlock;

	this->numberOfCells = inCells->GetNumberOfCells();

	map<int,vector<pair<int*,int>>> segments;
	map<int,vector<int>> activities;

	if(WithActivities)
	{
		for(int currentCell = 0; currentCell < numberOfCells; currentCell++)
		{
			inCells->GetNextCell(*npts, *&pts);
			num = BlkNum->GetComponent(currentCell,0);
			MBlock = MiningBlock->GetComponent(currentCell,0);
			segments[num].push_back(pair<int*,int>(pts,*npts));
			activities[MBlock].push_back(num); 
		}
	}

	else
	{
		for(int currentCell = 0; currentCell < numberOfCells; currentCell++)
		{
			inCells->GetNextCell(*npts, *&pts);
			num = BlkNum->GetComponent(currentCell,0);
			segments[num].push_back(pair<int*,int>(pts,*npts));
		}
	}

	for(map<int,vector<pair<int*,int>>>::iterator segment = segments.begin();
		segment != segments.end(); segment++)
	{
		writeSegment(myFile,segment,Type,Thickness,Width);
	}

	myFile<<endl<<"      0"<<endl<<
"* ------------------------------------------------------------------------------\n"
"* COORDINATE SPECIFICATION LIST - one line per coordinate - end list with N=0\n"
"* ------------------------------------------------------------------------------\n"
"*   N   X (East)  Y (North)   Z (Elev)                             [metres:feet]\n"
"* ------------------------------------------------------------------------------\n"<<endl;

	double currentTuple[3];
	for(int currentPoint = 0; currentPoint<inPoints->GetNumberOfPoints(); currentPoint++)
	{
		inPoints->GetPoint(currentPoint,currentTuple);
		myFile<<setw(7)<<currentPoint+1<<setw(20)<<currentTuple[0]<<setw(20)
			<<currentTuple[1]<<setw(20)<<currentTuple[2]<<endl;
	}
	myFile<<endl<<"    0"<<endl;
	myFile<<
"* ------------------------------------------------------------------------------\n"
"* MATERIAL PROPERTIES LIST - 3 lines per material - end list with N=0\n"
"* ------------------------------------------------------------------------------\n"
"* LINE 1 - STRESS STATE SPECIFICATION - 1 line per material    \n"
"* N    S1,S2,S3   dS1,dS2,dS3   T1,P1,T3,Surf   S=S+dS*(Z-Surf)\n"
"* ------------------------------------------------------------------------------\n"
"* N           - material number - 1 host rock mass - 2,3... for other materials \n"
"* S1,S2,S3    - far field stress values at depth Surf                  [MPa:psi]\n"
"* dS1,dS2,dS3 - variation with depth  S = S + dS.(Z-Surf)   [MPa/metre:psi/foot]\n"
"* T1          - trend of S1 from Y (north) towards X (east)            [degrees]\n"
"* P1          - plunge of S1 (+) positive down from horizontal plane   [degrees]\n"
"* T3          - trend of S3 from Y (north) towards X (east)            [degrees]\n"
"* Surf        - elevation for S1,S2,S3                             [metres:feet]\n"
"* ------------------------------------------------------------------------------\n"
"* LINE 2 - ELASTIC PROPERTY SPECIFICATION - 1 line per material\n"
"* ------------------------------------------------------------------------------\n"
"* MT=0, 0,0,     0,0,     GN,GS (element type 1 or 2)          \n"
"* MT=1, Ep,Er,   PRp,PRr, GN,GS (element type 1 or 2)          \n"
"*       Ep,Er   - Young's modulus - peak and residual values           [MPa:psi]\n"
"*       PRp,PRr - Poisson's ratio - peak and residual values  \n"
"*       GN,GS   - viscous modulus - normal and shear components        [MPa:psi]\n"
"* MT=2, Bp,Br,   Sp,Sr,   GN,GS (element type 1 or 2)          \n"
"*       Bp,Br   - Bulk modulus - peak and residual values              [MPa:psi]\n"
"*       Sp,Sr   - Shear modulus - peak and residual values             [MPa:psi]\n"
"* MT=3, KNp,KNr, KSp,KSr, GN,GS (element type 2 only)          \n"
"*       KNp,KNr - normal stiffness - peak and residual     [MPa/metres:psi/feet]\n"
"*       KSp,KSr - shear stiffness - peak and residual      [MPa/metres:psi/feet]\n"
"* 'MATERIAL NAME' - maximum of 20 characters must appear in single quotes\n"
"* ------------------------------------------------------------------------------\n"
"* LINE 3 - STRENGTH PARAMETER SPECIFICATION - 1 line per material\n"
"* ------------------------------------------------------------------------------\n"
"* MF=0  no strength parameters specified (elastic response only)\n"
"* MF=1, Top,Tor,Cop,Cor,Sop,Sor,PHIp,PHIr for Mohr-Coulomb (element type 1 or 2)\n"
"*       To - tension cut-off - normally 0 or negative                  [MPa:psi]\n"
"*       Co - pillar strength - field scale                             [MPa:psi]\n"
"*       So - joint cohesion - only use for type 2 elements             [MPa:psi]\n"
"*       PHI- friction angle - rock mass value                          [degrees]\n"
"* MF=2, Top,Tor,scp,scr,mp,mr,sp,sr for Hoek-Brown (element type 1 only)\n"
"*       To - tension cut-off - normally 0 or negative                  [MPa:psi]\n"
"*       sc - unconfined compressive strength - lab scale               [MPa:psi]\n"
"*       m  - mi*Exp[(RMR-100)/28] - Hoek-Brown parameter       \n"
"*       s  -    Exp[(RMR-100)/9]  - Hoek-Brown parameter       \n"
"* ------------------------------------------------------------------------------\n"
"* N, S1,S2,S3 must be specified, GN,GS are optional            \n"
"* ------------------------------------------------------------------------------\n"
"* N   S1,S2,S3 dS1,dS2,dS3 T1,P1,T3,Surf,P,dP   S=S+dS*(Z-Surf)\n"
"* MT  0 1,Ep/r,PRp/r,GN,GS 2,Bp/r,Sp/r,GN,GS 3,KNp/r,KSp/r,GN,GS 'MATERIAL NAME'\n"
"* MF  0 1,Top,Tor,Cop,Cor,Sop,Sor,PHIp,PHIr  2,Top,Tor,scp,scr,mp,mr,sp,sr\n"
"* ------------------------------------------------------------------------------\n"
"* Host Material                                                \n"
"  1  1.60E+1  3.00E+1  6.00E+1  0.00E+0  0.00E+0  0.00E+0    0   0    0            0.000000  0.00E+0  0.00E+0  0.00E+0  0.00E+0  0.00E+0  0.00E+0  0.00E+0  0.00E+0  0.00E+0  0.00E+0\n"
"  1  6.00E+4  6.00E+4  2.50E-1  2.50E-1  0.00E+0  0.00E+0  1.00E+0  0.00E+0  1.00E+0  1.00E+0 'Host Material #1                        '\n"
" -1  0.00E+0  0.00E+0  6.00E+1  6.00E+1  0.00E+0  0.00E+0  3.00E+1  3.00E+1  0.00E+0\n"
"                                                               \n"
"  2  1.60E+1  3.00E+1  6.00E+1  0.00E+0  0.00E+0  0.00E+0    0   0    0            0.000000  0.00E+0  0.00E+0  0.00E+0  0.00E+0  0.00E+0  0.00E+0  0.00E+0  0.00E+0  0.00E+0  0.00E+0\n"
"  4  4.00E+4  4.00E+4  2.40E+4  2.40E+4  0.00E+0  0.00E+0  1.00E+0  0.00E+0  1.00E+0  1.00E+0 'Material number    2                    '\n"
"  1  0.00E+0  0.00E+0  1.00E+6  1.00E+6  0.00E+0  0.00E+0  3.60E+1  3.50E+1  0.00E+0\n"
"      \n"                                                         
"  0\n"
"* ------------------------------------------------------------------------------\n"
"* GRID SPECIFICATION LIST - 1 line per grid - end list with N=0\n"
"* ------------------------------------------------------------------------------\n"
"* N           - grid number\n"
"* 'GRID NAME' - maximum of 20 characters\n"
"* I1,I2,I3,I4 - coordinate numbers of corners of grid plane    \n"
"* I5,I6,I7,I8,TYPE,THICK - not used\n"
"* WIDTH - maximum width                                            [metres:feet]\n"
"* ------------------------------------------------------------------------------\n"
"*   N 'GRID NAME'              I1    I2    I3    I4    I5    I6    I7    I8 TYPE   THICK WIDTH\n"
"* ------------------------------------------------------------------------------\n"
"\n"                                                               
"\n"                                                               
"    0\n" 
"* ------------------------------------------------------------------------------\n"
"* MINING STEP SPECIFICATION LIST - 1 line per block - end list with N=0\n"
"* ------------------------------------------------------------------------------\n"
"* N  - block identification number (1 - 32000)\n"
"* MC - material code\n"
"*     MC= 0 for zero surface stresses (mined out)\n"
"*     MC=-M to set surface stresses of block N to stress state of material M\n"
"*     MC=+M to insert material number M into block number N\n"
"* ------------------------------------------------------------------------------\n"
"*   N    MC    ME                                              \n"
"* ------------------------------------------------------------------------------\n"<<endl;  

	myFile<<endl;

	if(!WithActivities)
	{
		myFile<<"   "<<1<<"    "<<0<<endl<<"   "<<0;
		return ;
	}


	for(map<int,vector<int>>::iterator it = activities.begin();
		it!= activities.end(); it++)
	{
		for(vector<int>::iterator it2 = it->second.begin(); 
			it2 != it->second.end(); it2++)
		{
			myFile<<setw(5)<<*it2<<"     "<<0<<endl;
		}
		myFile<<setw(5)<<0<<endl<<endl;
	}

	myFile.close();
	}
	

int vtkMap3dWriter::FillInputPortInformation(int, vtkInformation *info)
{
	info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData");
	return 1;
}

vtkPolyData* vtkMap3dWriter::GetInput()
{
	return vtkPolyData::SafeDownCast(this->Superclass::GetInput());
}

vtkPolyData* vtkMap3dWriter::GetInput(int port)
{
	return vtkPolyData::SafeDownCast(this->Superclass::GetInput(port));
}

//Called with this->PrintSelf(*fp, indent)
void vtkMap3dWriter::PrintSelf(ostream& fp, vtkIndent indent)
{
	//this->Superclass::PrintSelf(fp,indent);
}


void writeSegment(ofstream& myFile, map<int,vector<pair<int*,int>>>::iterator &segment,
									int Type, double Thickness, double Width )
{
	long pos;
	int *pts;
	pos = myFile.tellp();

	
	if(segment->second.size() == 6) // a segment with 6 cells is a block with
		// 6 faces and 8 points
	{
		writeBlock(myFile,segment,Type,Thickness,Width);
	}


	else if(segment->second.size() > 6) // this is a block formed by 
		// more than 8 points and may be more than 6 faces
	{
		for(vector<pair<int*,int>>::iterator cell = segment->second.begin(); 
			cell != segment->second.end(); cell++)
		{
			myFile<<setw(6)<<segment->first<<" '"<<setw(40)<<"stope '";
			if(cell->second == 4) // this is a face
			{
				writeFace(myFile, cell,Type,Thickness,Width);
			}
			else if(cell->second == 3)	// this is a triangle
			{
				writeTriangle(myFile, cell,Type,Thickness,Width);
			}
			else
			{
				//vtkErrorMacro("Error when writing segment"<<segment->first<<endl);
				myFile.seekp(pos);
				return ;
			}
			if(!myFile.good())
			{
				//vtkErrorMacro("Error when writing segment"<<segment->first<<endl);				
				myFile.clear();
				myFile.seekp(pos);
				return ;
			}
		}
	}

//	else
//		vtkErrorMacro("Error when writing segment"<<segment->first<<endl);	
	
}



 void writeBlock(ofstream& myFile, map<int,vector<pair<int*,int>>>::iterator &segment,
	 int Type, double Thickness, double Width)
{
		long pos;
		int *pts;
		pos = myFile.tellp();	// save the position in the file before writing,
		// to go back to this position if something fail when writing

		myFile<<setw(6)<<segment->first<<" '"<<setw(40)<<"stope '";
		vector<pair<int*,int>>::iterator cell = segment->second.begin();
		
		//while(cell < segment->second.end() - 
		if(cell->second == 4)		// a 6 faces block is always formed 
				// by 6 cells with 4 points each
		{
			pts = cell->first;
			myFile<<setw(7)<<pts[0]+1<<setw(7)<<pts[1]+1<<setw(7) // +1 for all id because map3d can't read an id=0
				<<pts[2]+1<<setw(7)<<pts[3]+1;
		}
		else
		{
			//vtkErrorMacro("Error when writing segment"<<segment->first<<endl);
			myFile.seekp(pos);
			return ;
		}
		if(!myFile.good())
		{
			//vtkErrorMacro("Error when writing segment"<<segment->first<<endl);				
			myFile.clear();
			myFile.seekp(pos);
			return ;
		}

		cell += 5;
		if(cell->second == 4)
		{
			pts = cell->first;
			myFile<<setw(7)<<pts[0]+1<<setw(7)<<pts[1]+1<<setw(7)
				<<pts[2]+1<<setw(7)<<pts[3]+1;
		}
		else
		{
			//vtkErrorMacro("Error when writing segment"<<segment->first<<endl);
			myFile.seekp(pos);
			return ;
		}
		if(!myFile.good())
		{
			//vtkErrorMacro("Error when writing segment"<<segment->first<<endl);				
			myFile.clear();
			myFile.seekp(pos);
			return ;
		}
		myFile<<"   "<<Type<<"   "<<Thickness<<"   "<<Width<<endl;
}


 void writeFace(ofstream &myFile, vector<pair<int*,int>>::iterator &cell,
	 int Type, double Thickness, double Width)
 {
	int* pts; 
	pts = cell->first;

	myFile<<setw(7)<<pts[0]+1<<setw(7)<<pts[1]+1<<setw(7)<<pts[2]+1<<
		setw(7)<<pts[3]+1<<"     "<<0<<"     "<<0<<"     "<<0<<"     "<<0<<"   "
		<<Type<<"   "<<Thickness<<"   "<<Width<<endl;
 }


 void writeTriangle(ofstream &myFile, vector<pair<int*,int>>::iterator &cell,
	 int Type, double Thickness, double Width)
 {
	int* pts; 
	pts = cell->first;

	myFile<<setw(7)<<pts[0]+1<<setw(7)<<pts[1]+1<<setw(7)<<pts[2]+1<<
		setw(7)<<pts[2]+1<<"     "<<0<<"     "<<0<<"     "<<0<<"     "<<0<<
		"   "<<Type<<"   "<<Thickness<<"   "<<Width<<endl;
 }


