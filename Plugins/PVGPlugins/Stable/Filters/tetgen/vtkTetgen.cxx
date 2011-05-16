//===================================================================================//
//				Mirarco Mining Innovation
//
// Filter:   Volumique Tetrahedralisation
// Class:    vtkTetgen
// Author:   Nehmeh Bilal
// Director: Marie-Grabrielle Valet 
// Date:     July 2008
// contact: nehmebilal@gmail.com 
//====================================================================================//

#include "vtkTetgen.h"
#include "vtkCellArray.h"
#include "vtkFloatArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkPointData.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkUnstructuredGrid.h"
#include "vtkCellData.h"
#include <math.h>
#include <map>
#include <algorithm>
#include <vector>
#include<iostream>
#include<string>
#include<string.h>
#include<sstream>
#include<fstream> 
#include "tetgen.h" // Defined tetgenio, tetrahedralize().
#include "vtkCleanPolyData.h"
#include "vtkTriangleFilter.h"
#include "vtkDataArray.h"
#include "vtkIntArray.h"
#include "vtkMath.h"



struct tetgen_Internal
{

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// tetgenio    Passing data into and out of the library of TetGen.           //
//                                                                           //
// The tetgenio data structure is actually a collection of arrays of points, //
// facets, tetrahedra, and so forth.  The library will read and write these  //
// arrays according to the options specified in tetgenbehavior structure.    //
//                                                                           //
// If you want to program with the library of TetGen, it's necessary for you //
// to understand this data type,while the other two structures can be hidden //
// through calling the global function "tetrahedralize()". Each array corre- //
// sponds to a list of data in the file formats of TetGen.  It is necessary  //
// to understand TetGen's input/output file formats (see user's manual).     //
//                                                                           //
// Once an object of tetgenio is declared,  no array is created. One has to  //
// allocate enough memory for them, e.g., use the "new" operator in C++. On  //
// deletion of the object, the memory occupied by these arrays needs to be   //
// freed.  Routine deinitialize() will be automatically called. It will de-  //
// allocate the memory for an array if it is not a NULL. However, it assumes //
// that the memory is allocated by the C++ "new" operator. If you use malloc //
// (), you should free() them and set the pointers to NULLs before reaching  //
// deinitialize().                                                           //
//                                                                           //
// In all cases, the first item in an array is stored starting at index [0]. //
// However, that item is item number `firstnumber' which may be '0' or '1'.  //
// Be sure to set the 'firstnumber' be '1' if your indices pointing into the //
// pointlist is starting from '1'. Default, it is initialized be '0'.        //
//                                                                           //
// Tetgenio also contains routines for reading and writing TetGen's files as //
// well.  Both the library of TetGen and TetView use these routines to parse //
// input files, i.e., .node, .poly, .smesh, .ele, .face, and .edge files.    //
// Other routines are provided mainly for debugging purpose.                 //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

	tetgenio in, out;		// in -> input tetgen object, the input polyData is passed to tetgen throught this object
											// out -> output tetgen object, the output unstructuredGrid data is passed from tetgen to
											// paraview throught this object.

	// The facet data structure.  A "facet" is a planar facet.  It is used
  //   to represent a planar straight line graph (PSLG) in two dimension.
  //   A PSLG contains a list of polygons. It also may conatin holes in it,
  //   indicated by a list of hole points (their coordinates).


	tetgenio::facet *f;  

	tetgenio::polygon *p; 

																												
};
	
																													//////////////
																																			//
void tetrahedralize(char *switches, tetgenio *in, tetgenio *out,			//	prototypes of tetgen functions wich will be used in vtkTetgen
  tetgenio *addin, tetgenio *bgmin);																	//	
void tetrahedralize(tetgenbehavior *b, tetgenio *in, tetgenio *out,		//
  tetgenio *addin, tetgenio *bgmin);																	//
																												////////////////	


using namespace std;


vtkCxxRevisionMacro(vtkTetgen, "$Revision: 0.1 $");
vtkStandardNewMacro(vtkTetgen);

vtkTetgen::vtkTetgen()
{

	this->numberOfCells = 0;
	this->numberOfPoints = 0;
	this->tetgenCommand = 0;
  this->RadiusEdgeRatio = 0;
  this->MaxTetrahedronVolume = 0;	
	this->MaxTetrahedronVolumeInBox = 1000000;
	this->WithRegions = 0;
	this->WithPredefinedRegions = 0;
	this->numberOfTetrahedron = 0;
	this->MaxTetrahedronVolumeInBox2 = 1000000;

	this->coordCounter = 0;

	this->WithBoundingBox = 0;

	this->WithBBX1 = 0;
	this->deltaX = 0;
	this->deltaY = 0;
	this->deltaZ = 0;

	this->WithBBX2 = 0;
	this->deltaX2 = 0;
	this->deltaY2 = 0;
	this->deltaZ2 = 0;

	this->PieceInvariant = 1;
	this->Tolerance = 0;
	this->AbsoluteTolerance = 0;
	this->ToleranceIsAbsolute = 0;
	this->ConvertLinesToPoints = 0;
	this->ConvertPolysToLines = 0;
	this->ConvertStripsToPolys = 1;
	this->PointMerging = 1;


	this->RegionArray = NULL;
	
}


vtkTetgen::~vtkTetgen()
{
	delete[] this->RegionArray;
}




/*****************************************************************************
 Name:            RequestData
 Types of variables: input and output
 Description: 
"****************************************************************************/
int vtkTetgen::RequestData(vtkInformation *vtkNotUsed(request), 
	vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{

	 // get the info objects
	 vtkInformation *inInfo = inputVector[0]->GetInformationObject ( 0 );
	 vtkInformation *outInfo = outputVector->GetInformationObject ( 0 );

	 // get the input and ouptut
	 vtkPolyData *realInput = vtkPolyData::SafeDownCast (
						  inInfo->Get ( vtkDataObject::DATA_OBJECT() ) );

	 vtkUnstructuredGrid *output = vtkUnstructuredGrid::SafeDownCast (		// the output is unstructuredGrid because
							 outInfo->Get ( vtkDataObject::DATA_OBJECT() ) );         // we output a volumique tetrahedra 
																																				

	vtkIdType id;		// used to hold a temporary id  

	vtkCell *cell1;

	realInput->GetBounds(bounds);		// bounds = [Xmin, Xmax, Ymin, Ymax, Zmin, Zmax]

  //you cannot modify data that comes in from the pipeline, 
  //instead you have to modify a local copy only. So made the pipeline
  //input realinput, and the local version just called input
  vtkPolyData *input = vtkPolyData::New();

//---------------------------------------- Clean Filter ----------------------------------------------//
//																																																		//
	 vtkCleanPolyData *connect = vtkCleanPolyData::New();
	 connect->SetInput(realInput);
	 connect->SetPieceInvariant(this->PieceInvariant);
	 connect->SetTolerance(this->Tolerance);
	 connect->SetAbsoluteTolerance(this->AbsoluteTolerance);
	 connect->SetToleranceIsAbsolute(this->ToleranceIsAbsolute);
	 connect->SetConvertLinesToPoints(this->ConvertLinesToPoints);
	 connect->SetConvertPolysToLines(this->ConvertPolysToLines);
	 connect->SetConvertStripsToPolys(this->ConvertStripsToPolys);
	 connect->SetPointMerging(this->PointMerging);
	 connect->Update();
	 
	 input->ShallowCopy(connect->GetOutput());
	 connect->Delete();
//																																																		//	
//----------------------------------------------------------------------------------------------------//


	 
//---------------------------------------- Clean Faces --------------------------------------------------//
// this code clean duplicated faces. we didn't use the filter cleanCells because we want to use a part	 //	
// of this filter and because we did modify it. that's why we copied only the part we want and we modified it.		 //
// for documentation on the folowing code, look at Clean Faces filter.																	 //
//																																																			 //

	map<vector<vtkIdType>, vector<vtkIdType> > cellMap;
	vector<vtkIdType> cellVect;
	vector<vtkIdType> cellVect2;
	this->inCells = input->GetPolys();

	(this->inCells)->InitTraversal();
	// will store the number of points in current cell
	vtkIdType *npts = new vtkIdType(); 
	// will store a array of points id's in current cell
	vtkIdType *pts; 

	for(vtkIdType currentCell = 0; currentCell < inCells->GetNumberOfCells(); currentCell++)
	{
		(this->inCells)->GetNextCell(*npts, *&pts);
		for(int currentPoint=0 ; currentPoint<*npts; currentPoint++)
		{
			cellVect.push_back(pts[currentPoint]);
			cellVect2.push_back(pts[currentPoint]);
		}
		std::sort(cellVect2.begin(),cellVect2.end());
		cellMap[cellVect2] = cellVect;
		cellVect.erase(cellVect.begin(), cellVect.end());
		cellVect2.erase(cellVect2.begin(), cellVect2.end());
	}

	//this->inCells->Delete();

	this->Cells = vtkCellArray::New();
	//this->Cells->Allocate(cellMap.size());

	for(map<vector<vtkIdType>,vector<vtkIdType> >::iterator iter=cellMap.begin(); iter!=cellMap.end(); iter++)
	{
		this->Cells->InsertNextCell(iter->second.size());

		for(unsigned int currentPoint = 0; currentPoint < iter->second.size(); currentPoint++)
		{
			this->Cells->InsertCellPoint(iter->second[currentPoint]);
		}
	}
	input->SetPolys(this->Cells);

//																																																	//
//																																																	//
//--------------------------------------------------------------------------------------------------//


//-------------------------------- Triangulate ------------------------------------------//
//																																											 //	
	vtkTriangleFilter *connect2 = vtkTriangleFilter::New();
	 connect2->SetInput(input);
	 connect2->Update();
	 input->ShallowCopy(connect2->GetOutput());
	 connect2->Delete();
//																																											 //	
//---------------------------------------------------------------------------------------//

	 this->Internal = new tetgen_Internal;

	this->coordCounter = 0;

	this->inPoints = input->GetPoints();
	vtkIntArray* BlkNum = vtkIntArray::New();	// used to set regions attributs on the output cells
	vtkDataArray *regionNumbers;	// used to load map3d regions attributs from the input cells 

	regionNumbers = NULL;
	if(WithPredefinedRegions)
	{
		regionNumbers = realInput->GetCellData()->GetArray(this->RegionArray);	// reading the input map3d attributs if they exist  
		if(regionNumbers == NULL)
		{
			vtkErrorMacro("The Predefined Region Array is invalid");
			delete this->Internal;
			return 1;
		}
	}

	if(WithRegions || (WithPredefinedRegions)) // if (find regions is selected) or (with MAP3D regions is selected and
																																			// the input data are MAP3D data)	
	{
		BlkNum->SetName("Region Numbers");	// create a data array called "Region Numbers"
	}

	

	// tetgenio object input
	this->Internal->in.numberoffacets = input->GetPolys()->GetNumberOfCells() + input->GetStrips()->GetNumberOfCells();

	if(WithBBX1)					// if we want to add a bounding box on the input data, we must allocate 
		this->Internal->in.numberoffacets += 6;		// memory for each faces of the bounding box 
	if(WithBBX2)					// if we want to add a bounding box on the input data, we must allocate 
		this->Internal->in.numberoffacets += 6;		// memory for each faces of the bounding box

	this->Internal->in.facetlist = new tetgenio::facet[this->Internal->in.numberoffacets];
	this->Internal->in.facetmarkerlist = new int[this->Internal->in.numberoffacets];

	map<int,vector<int> > stopes;	// the keys of this map are the regions numbers, and for each 
																// key ( region number) the value is a vector of the cells id
																// having the same region number.
	double *currentTuple;
	
	// MLivingstone
	// Make sure we delete the CellArray that was created to form the initial input
	this->Cells->Delete();

	// the folowing loop is used to read polygonal cells and strips cells in the same time //	
	for (int cellType = 0; cellType < 2; cellType++)																												
	{
		if(cellType == 0)
			{
			this->Cells = input->GetPolys();
			}
		else
			{
			this->Cells = input->GetStrips();
			}
		this->numberOfCells = (this->Cells)->GetNumberOfCells();

		if(this->numberOfCells == 0)
			continue;

		(this->Cells)->InitTraversal();

		vtkIdType realNumOfCells = realInput->GetPolys()->GetNumberOfCells();	// realNumOfCells is the numbers of cells before cleaning faces 
																																		// and before triangulation. this variable is used to know how many
																																		// cells have a MAP3D region attribut when the input is a MAP3D data	

		// for loop on all cells
		for(vtkIdType currentCell = 0; currentCell < this->numberOfCells; currentCell++)
		{
			if(WithPredefinedRegions && (currentCell < realNumOfCells)) // if the current cell is a part of the original
																																														 // cell, and the real input is a MAP3D data	
			{
				stopes[regionNumbers->GetComponent(currentCell,0)].push_back(currentCell); // append the current cell id to stopes[current region number]  
			}

			(this->Cells)->GetNextCell(*npts, *&pts);
			// Facet 
			this->Internal->f = &this->Internal->in.facetlist[currentCell];		
			this->Internal->f->numberofpolygons = 1;		// each face will be formed by one polygone
			this->Internal->f->polygonlist = new tetgenio::polygon[this->Internal->f->numberofpolygons];
			this->Internal->f->numberofholes = 0;		// the volumique tetrahedralisation filter dont support predefined holes
			this->Internal->f->holelist = NULL;
			this->Internal->p = &this->Internal->f->polygonlist[0];
			this->Internal->p->numberofvertices = *npts;	// the number of vertices of the current face, is equal to the number of points on the current cell
			this->Internal->p->vertexlist = new int[this->Internal->p->numberofvertices];
			

			for(int currentPoint = 0; currentPoint < *npts; currentPoint++)
			{
				this->Internal->p->vertexlist[currentPoint] = pts[currentPoint];
			}

			this->Internal->in.facetmarkerlist[currentCell] = 1;	// Set 'in.facetmarkerlist': 1-> boundary face
		}

		
		this->Internal->in.firstnumber = 0;		// All indices start from 0.
		this->numberOfPoints = this->inPoints->GetNumberOfPoints();
		this->Internal->in.numberofpoints = this->numberOfPoints;

		if(WithBBX1)  // if with bounding box, add 8 more points
			this->Internal->in.numberofpoints += 8;
		if(WithBBX2)  // if with bounding box, add 8 more points
			this->Internal->in.numberofpoints += 8;

		this->Internal->in.pointlist = new REAL[this->Internal->in.numberofpoints * 3]; // each points is a array of 3 reals: [x, y, z]

		for(int currentPoint = 0; currentPoint<this->numberOfPoints; currentPoint++)
		{	
			currentTuple = NULL; 
			currentTuple = this->inPoints->GetPoint(currentPoint);

			//'pointlist':  An array of point coordinates.  The first point's x
			// coordinate is at index [0] and its y coordinate at index [1], its
			// z coordinate is at index [2], followed by the coordinates of the
			// remaining points.  Each point occupies three REALs.
			this->Internal->in.pointlist[coordCounter]  = currentTuple[0];		
			this->Internal->in.pointlist[coordCounter + 1] = currentTuple[1];
			this->Internal->in.pointlist[coordCounter + 2] = currentTuple[2];

			coordCounter += 3;
		}


		// if WithBBX1 option is selected, we create the desired boundingBox
		if(WithBBX1)
		{
			this->addBBX(1);
		}
		// if WithBBX2 option is selected, we create the desired boundingBox
		if(WithBBX2)
		{
			this->addBBX(2);
		}

	}

//----------------------------------------------------------------------------------------------------------------------------//
// when the option with predefined regions ( or with Map3d regions) is selected, we must read the regions attributs for				//
// each stope from the input map3d data and tell tetgen where each region or stope is. to do this we must find a point				//
// inside each stope and give this point with the corresponding region attribut to tetgen. when we use predefined regions			//
// in tetgen, we can specify the desired max tetrahedron volume for each region(stope) and this is the goal from using				//
// predefined regions. to tell tetgen this information, we set the folowing tetgenio classe attribut:													//
//																																																														//	
// `regionlist': An array of regional attributes and volume constraints.																											//
// The first constraint's x, y and z coordinates are at indices [0], [1] and [2], followed by the regional attribute					//
// at index [3], followed by the maximum volume at index [4]. Five REALs per constraint.																			//
// Note that each regional attribute is used only if you select the `A' switch, and each volume constraint is used only				//
// if you select the `a' switch (with no number following).																																		//
//																																																														//
// to find a point inside a stope, we first calculate the center of one cell on the boundary of this stope(all cells are on		//
// the boundary before the tetrahedralisation, so we chose one random cell on the stope). After that we calculate the normal	//
// to this cell. With the center and the normal of the cell, we can find the equation of the line perpendicular to this cell	//
// and passing through it's center:																																														//
// x = Xcenter + a*t ; y = Ycenter + b*t ; z = Zcenter + c*t, where (a,b,c) are the coordinates of the normal to the cell.		//
// because the cell is in the boundary of the stope, when we calculate the normal using the rule of the right hand, we found	//	
// a normal oriented to the outside of the stope. so if we normalise the normal and we choose t=0.0001, the (x,y,z) found,		//
// will be a coordinates of a point very close to the center of the cell but inside the stope, and that all what we need !		//
// 																																																														//	
// Remark: see class tetgenio in tetgen.cxx if u need more information																												//

	vtkPoints *realPoints = realInput->GetPoints();		// it's more safe to use the original input points					

	double XcellCenter = 0;
	double YcellCenter = 0;
	double ZcellCenter = 0;

	double point1[3];
	double point2[3];
	double point3[3];
	double vect1[3];
	double vect2[3];
	double normal[3];
	int currentRegion = 0;
	int regionID = 1;
	int x = 0;

	if(WithPredefinedRegions)
	{
		if((regionNumbers->GetComponent(0,0)) == 0)
			this->Internal->in.numberofregions = regionNumbers->GetMaxNorm() + 1;	// if regions attributs start from 0, the numbers of regions
																														// attribut will be the maximum region attribut + 1
		else
			// else : number of region = (max region attribut) - (min region attribut) + 1
			this->Internal->in.numberofregions = (regionNumbers->GetMaxNorm()) - (regionNumbers->GetComponent(0,0)) + 1; 
																																																	 	

		if(WithBBX1)
		{
			this->Internal->in.numberofregions += 1;	// if we use bounding box, we have one additional region 
		}
		if(WithBBX2)
		{
			this->Internal->in.numberofregions += 1;	// if we use bounding box, we have one additional region 
		}

		this->Internal->in.regionlist = new REAL[this->Internal->in.numberofregions*5];

		for(map<int, vector<int> >::iterator iter=stopes.begin(); iter!=stopes.end(); iter++)
		{
			XcellCenter = 0;
			YcellCenter = 0;
			ZcellCenter = 0;
			
			cell1 = realInput->GetCell(iter->second[(iter->second.size())/2]);  // we can choose any other cell in the stope
			*npts = cell1->GetNumberOfPoints();

			for(int currentPoint = 0; currentPoint < *npts; currentPoint++)
			{
				id = cell1->GetPointId(currentPoint);
				currentTuple = NULL; 
				currentTuple = realPoints->GetPoint(id);
				XcellCenter += currentTuple[0];
				YcellCenter += currentTuple[1];
				ZcellCenter += currentTuple[2];
			}
			XcellCenter = XcellCenter/(*npts);	// XcellCenter = (Xpoint1 + Xpoint2 + ...)/number of points 
			YcellCenter = YcellCenter/(*npts);	// YcellCenter = (Ypoint1 + Ypoint2 + ...)/number of points
			ZcellCenter = ZcellCenter/(*npts);	// ZcellCenter = (Zpoint1 + Zpoint2 + ...)/number of points

			currentTuple = NULL; 
			
			id = cell1->GetPointId(0);
			currentTuple = (realPoints->GetPoint(id));
			point1[0] = currentTuple[0];
			point1[1] = currentTuple[1];
			point1[2] = currentTuple[2];

			id = cell1->GetPointId(1);
			currentTuple = realPoints->GetPoint(id);
			point2[0] = currentTuple[0];
			point2[1] = currentTuple[1];
			point2[2] = currentTuple[2];

			id = cell1->GetPointId(2);
			currentTuple = realPoints->GetPoint(id);
			point3[0] = currentTuple[0];
			point3[1] = currentTuple[1];
			point3[2] = currentTuple[2];

			vect1[0] = point1[0] - point2[0];
			vect1[1] = point1[1] - point2[1];
			vect1[2] = point1[2] - point2[2];

			vect2[0] = point3[0] - point2[0];
			vect2[1] = point3[1] - point2[1];
			vect2[2] = point3[2] - point2[2];

			vtkMath::Cross(vect2,vect1,normal);
			vtkMath::Normalize(normal);


			this->Internal->in.regionlist[currentRegion] = XcellCenter - (normal[0]/1000);
			this->Internal->in.regionlist[currentRegion + 1] = YcellCenter - (normal[1]/1000);
			this->Internal->in.regionlist[currentRegion + 2] = ZcellCenter - (normal[2]/1000);
			this->Internal->in.regionlist[currentRegion + 3] = regionID ;
			this->Internal->in.regionlist[currentRegion + 4] = MaxTetrahedronVolume;	// the maximum tetrahedron volume for all stope is defined by the user

			currentRegion += 5;
			regionID++;

		}
		
		
		if(WithBBX1)
		{
			this->Internal->in.regionlist[currentRegion] = bounds1[1] - 0.0001;
			this->Internal->in.regionlist[currentRegion + 1] = bounds1[3] - 0.0001;
			this->Internal->in.regionlist[currentRegion + 2] = bounds1[5] - 0.0001;
			this->Internal->in.regionlist[currentRegion + 3] = regionID ;
			this->Internal->in.regionlist[currentRegion + 4] = MaxTetrahedronVolumeInBox;
			regionID++;
		}

		if(WithBBX2)
		{
			this->Internal->in.regionlist[currentRegion] = bounds2[1] - 0.0001;
			this->Internal->in.regionlist[currentRegion + 1] = bounds2[3] - 0.0001;
			this->Internal->in.regionlist[currentRegion + 2] = bounds2[5] - 0.0001;
			this->Internal->in.regionlist[currentRegion + 3] = regionID ;
			this->Internal->in.regionlist[currentRegion + 4] = MaxTetrahedronVolumeInBox2;
			regionID++;
		}
		
	}



	// Output the PLC to files 'barin.node' and 'barin.poly'.
	//this->Internal->in.save_nodes("C:\\tetgen");
	//this->Internal->in.save_poly("C:\\tetgen");

									////////////////
	std::string command="";				//
	std::string q = "";						// this code is used to convert the input value of
	std::string v = "";						// RadiusEdgeRatio and MaxTetrahedronVolume from 
	std::stringstream out2;				// int to string. 
	out2<<RadiusEdgeRatio;				//
	q = out2.str();								//
	std::stringstream out1 ;			//
	out1<<MaxTetrahedronVolume;		//
	v = out1.str();								//
									////////////////


//----------------------------------------------------------------------------------------------------------------//
// The folowing list define all commandes included in tetgen. We use only a part of this switch in this filter.
//
// -p Tetrahedralizes a piecewise linear complex (.poly or .smesh file).
// -q Quality mesh generation. A minimum radius-edge ratio may be specified (default 2.0).
// -a Applies a maximum tetrahedron volume constraint.
// -A Assigns attributes to identify tetrahedra in certain regions.
// -r Reconstructs and Refines a previously generated mesh.
// -Y Suppresses boundary facets/segments splitting.
// -i Inserts a list of additional points into mesh.
// -M Does not merge coplanar facets.
// -T Set a tolerance for coplanar test (default 1e-8).
// -d Detect intersections of PLC facets.
// -z Numbers all output items starting from zero.
// -o2 Generates second-order subparametric elements.
// -f Outputs faces (including non-boundary faces) to .face file.
// -e Outputs subsegments to .edge file.
// -n Outputs tetrahedra neighbors to .neigh file.
// -g Outputs mesh to .mesh file for viewing by Medit.
// -G Outputs mesh to .msh file for viewing by Gid.
// -O Outputs mesh to .off file for viewing by Geomview.
// -J No jettison of unused vertices from output .node file.
// -B Suppresses output of boundary information.
// -N Suppresses output of .node file.
// -E Suppresses output of .ele file.
// -F Suppresses output of .face file.
// -I Suppresses mesh iteration numbers.
// -C Checks the consistency of the final mesh.
// -Q Quiet: No terminal output except errors.
// -V Verbose: Detailed information, more terminal output.
// -v Prints the version information.
// -h Help: A brief instruction for using TetGen.

	switch(tetgenCommand)
	{
	case 1:
		command = "p" ;
		if(WithPredefinedRegions)
			command = "pA";
		break;
	case 2:
		command = "pY" ;
		if(WithPredefinedRegions)
			command = "pA";
		break;
	case 3:
		command = "pq" + q ;
		if(WithPredefinedRegions)
			command = "pqA";
		break;
	case 4:
		command = "pa" + v;
		if(WithPredefinedRegions)
			command = "paA";
		break;
	case 5:
		command = "pq" + q + "a" + v;
		if(WithPredefinedRegions)
			command = "pq" + q + "aA";
		break;
	default:
		vtkErrorMacro(" please select a tetrahedralization mode");
		delete this->Internal;
		input->Delete();
		delete[] npts;
		BlkNum->Delete();
		return 1;
	}

	if(WithRegions)
	{
		command = command +"AA";
	}
	char *command2;
	command2 = new char[command.size()];

	for(unsigned int i = 0; i<command.size(); i++)
		command2[i]=command[i];

// command2 define the switch used to excecute tetgen 

  try
    {
	  tetrahedralize(command2, &this->Internal->in, &this->Internal->out);
	  }
	catch ( ... )
	  {
	  vtkErrorMacro(" You need to try a different option, tetgen crashed " );
		delete this->Internal;
		input->Delete();
		delete[] npts;
		BlkNum->Delete();
	  return 1;
	  }

	// Output mesh to files 'barout.node', 'barout.ele' and 'barout.face'.
	//out.save_nodes("C:\\Transfer\\Tetgen\\barout");
	//out.save_elements("C:\\Transfer\\Tetgen\\barout");
	//out.save_faces("C:\\Transfer\\Tetgen\\barout");	

	vtkPoints *newpoints; 
	newpoints= vtkPoints::New();


// this is used only for debuging. if showPoints is defined, we will see the points inside each stope.
#ifdef showPoints
	int currentVert = 0;
	output->Allocate(9);


	for (int i = 0; i < in.numberofregions; i++)
	{
		pts = NULL;
		pts = new vtkIdType[1];
		pts[0] = i;
		newpoints->InsertNextPoint(in.regionlist[currentVert],in.regionlist[currentVert+1],in.regionlist[currentVert+2]);
		currentVert += 5;
		output->InsertNextCell(VTK_VERTEX,1,pts); 
	}
	output->SetPoints(newpoints);
	newpoints -> Delete();

	return 1;
#endif


// After the tetrahedralisation, tetgen store the boundary faces in out.trifacelist, the generated tetrahedra 
// in out.tetrahedronlist and all points in out.pointlist

// `trifacelist':  An array of triangular face endpoints.  The first
//   face's endpoints are at indices [0], [1] and [2], followed by the

// pointlist An array of point coordinates. The first point�s x coordinate is
// at index [0], its y coordinate at index [1], and its z coordinate at index [2],
// followed by the coordinates of the remaining points. Each point occupies
// three REALs.

// tetrahedronlist An array of tetrahedron corners. The first tetrahedron�s
// first corner is at index [0], followed by its other three corners, followed by
// any other nodes if the �-o2� switch is used. Each tetrahedron occupies �numberofcorners�
// (4 or 10) ints.

// tetrahedronattributelist An array of tetrahedron attributes. Each tetrahedron�s
// attributes occupy �numberoftetrahedronattributes� REALs.

// edgelist An array of segment endpoints. The first segment�s endpoints are
// at indices [0] and [1], followed by the remaining segments. Two ints per
// segment.

	output->Allocate(this->Internal->out.numberoftrifaces + this->Internal->out.numberoftetrahedra);


	coordCounter = 0;
	for (int i = 0 ; i < (this->Internal->out.numberofpoints); i++)
	{

		newpoints->InsertNextPoint(this->Internal->out.pointlist[coordCounter],this->Internal->out.pointlist[coordCounter+1],this->Internal->out.pointlist[coordCounter+2]);
		coordCounter += 3;
	}

	int cornersCounter = 0;

	int j = 0;
/*
	pts = NULL;
	pts = new vtkIdType[3];
	
	for (int i=0 ; i < out.numberoftrifaces; i++)
	{
		pts[0] = out.trifacelist[cornersCounter];
		pts[1] = out.trifacelist[cornersCounter + 1];
		pts[2] = out.trifacelist[cornersCounter + 2];
		output->InsertNextCell(VTK_TRIANGLE, 3, pts);
		cornersCounter += 3;
		if(WithRegions || (regionNumbers != NULL && WithPredefinedRegions))
			BlkNum->InsertValue(j,1);		// all boundary faces in tetgen have region attribut equal to 1. 
		j++ ;
	} */

	pts = NULL;
	pts = new vtkIdType[4];
	cornersCounter = 0;

	//vtkErrorMacro(""<<out.numberoftetrahedronattributes);
	for (int i=0 ; i < this->Internal->out.numberoftetrahedra; i++)
	{	
		pts[0] = this->Internal->out.tetrahedronlist[cornersCounter];
		pts[1] = this->Internal->out.tetrahedronlist[cornersCounter+1];
		pts[2] = this->Internal->out.tetrahedronlist[cornersCounter+2];
		pts[3] = this->Internal->out.tetrahedronlist[cornersCounter+3];
		output->InsertNextCell(VTK_TETRA,4,pts);

		if(WithRegions || (WithPredefinedRegions))
		BlkNum->InsertValue(j,this->Internal->out.tetrahedronattributelist[i]);		// all tetrahedron in the same region have the same region attribut
			j++;
		cornersCounter += 4;
	}


	this->numberOfTetrahedron = this->Internal->out.numberoftetrahedra;

	output->SetPoints(newpoints);
	if(WithRegions || WithPredefinedRegions)
		output->GetCellData()->AddArray(BlkNum);


	delete[] command2;

	input->Delete();
	delete[] npts;
	delete[] pts;
	newpoints -> Delete();
	BlkNum->Delete();

	delete this->Internal;
	
return 1;
}

/***************************************************************************
  Name:            PrintSelf
 Types of variables:
 Description:
"****************************************************************************/
void vtkTetgen::PrintSelf(ostream& os, vtkIndent indent)
{
}

/********************************************************************************
  Name:            FillInputPortInformation
 Types of variables:
 Description:
"*********************************************************************************/
int vtkTetgen::FillInputPortInformation ( int vtkNotUsed(port), vtkInformation* info )
  {
  info->Set ( vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData", 0 );
  return 1;
  }









void vtkTetgen::addBBX(int i)
{

	double dx;
	double dy;
	double dz;

	if(i==1)
	{
		dx = this->deltaX;
		dy = this->deltaY;
		dz = this->deltaZ;
	}
	else
	{
		dx = this->deltaX2;
		dy = this->deltaY2;
		dz = this->deltaZ2;
	}

	double newBounds[6];

								/////////
	double Xcentre = 0;	 //	
	double Ycentre = 0;	 // coordinates of the boundingBox centre
	double Zcentre = 0;	 //
								/////////	

	double Xlength = (bounds[1]-bounds[0])/2;  // length of X compement of the original bounding box diveded by 2
	double Ylength = (bounds[3]-bounds[2])/2;	 // length of Y compement of the original bounding box diveded by 2	
	double Zlength = (bounds[5]-bounds[4])/2;	 // length of Z compement of the original bounding box diveded by 2	

	Xcentre = bounds[0] + Xlength;	// Xcentre = Xmin + (Xmax - Xmin)/2
	Ycentre = bounds[2] + Ylength;	// Ycentre = Ymin + (Ymax - Ymin)/2
	Zcentre = bounds[4] + Zlength;	// Zcentre = Zmin + (Zmax - Zmin)/2

	newBounds[0] = Xcentre - (dx/100)*Xlength;		// new Xmin = Xcentre - [(deltaX/100)*( Xmax - Xmin)/2] 
	newBounds[1] = Xcentre + (dx/100)*Xlength;		// new Xmax = Xcentre + [(deltaX/100)*( Xmax - Xmin)/2]
	newBounds[2] = Ycentre - (dy/100)*Ylength;		// new Ymin = Ycentre - [(deltaY/100)*( Ymax - Ymin)/2]
	newBounds[3] = Ycentre + (dy/100)*Ylength;		// new Ymax = Ycentre + [(deltaY/100)*( Ymax - Ymin)/2]
	newBounds[4] = Zcentre - (dz/100)*Zlength;		// new Zmin = Zcentre + [(deltaZ/100)*( Zmax - Zmin)/2]
	newBounds[5] = Zcentre + (dz/100)*Zlength;		// new Zmax = Zcentre + [(deltaZ/100)*( Zmax - Zmin)/2]


//--------------------------------------------------------------------------------------------------------------------//
// the folowing code is used to add the bounding Box faces and points in tetgen input(tetgenio::in). the bounding box	//
// has 6 faces and 8 points. the vertex list for each face must be given with the correct order so the faces orientations  //  
// are corrects.																																																				//
//																																																										//

	//----------------------------------------- Facet 1 --------------------------------------------------
	this->Internal->f = &this->Internal->in.facetlist[this->numberOfCells];		
	this->Internal->f->numberofpolygons = 1;
	this->Internal->f->polygonlist = new tetgenio::polygon[this->Internal->f->numberofpolygons];
	this->Internal->f->numberofholes = 0;
	this->Internal->f->holelist = NULL;
	this->Internal->p = &this->Internal->f->polygonlist[0];
	this->Internal->p->numberofvertices = 4;
	this->Internal->p->vertexlist = new int[this->Internal->p->numberofvertices];

	this->Internal->p->vertexlist[0] = this->numberOfPoints ;				
	this->Internal->p->vertexlist[1] = this->numberOfPoints + 3 ;
	this->Internal->p->vertexlist[2] = this->numberOfPoints + 2;
	this->Internal->p->vertexlist[3] = this->numberOfPoints + 1;

	this->Internal->in.facetmarkerlist[this->numberOfCells] = 1;	// Set 'in.facetmarkerlist': 1-> boundary face
	//-----------------------------------------------------------------------------------------------------

	//--------------------------------------------- Facet 2 ------------------------------------------------
	this->Internal->f = &this->Internal->in.facetlist[this->numberOfCells + 1];
	this->Internal->f->numberofpolygons = 1;
	this->Internal->f->polygonlist = new tetgenio::polygon[this->Internal->f->numberofpolygons];
	this->Internal->f->numberofholes = 0;
	this->Internal->f->holelist = NULL;
	this->Internal->p = &this->Internal->f->polygonlist[0];
	this->Internal->p->numberofvertices = 4;
	this->Internal->p->vertexlist = new int[this->Internal->p->numberofvertices];

	this->Internal->p->vertexlist[0] = this->numberOfPoints + 4 ;
	this->Internal->p->vertexlist[1] = this->numberOfPoints + 5;
	this->Internal->p->vertexlist[2] = this->numberOfPoints + 6;
	this->Internal->p->vertexlist[3] = this->numberOfPoints + 7;

	this->Internal->in.facetmarkerlist[this->numberOfCells + 1] = 1;	// Set 'in.facetmarkerlist': 1-> boundary face
	//-------------------------------------------------------------------------------------------------------

	//----------------------------------------------- Facet 3 -----------------------------------------------
	this->Internal->f = &this->Internal->in.facetlist[this->numberOfCells + 2];
	this->Internal->f->numberofpolygons = 1;
	this->Internal->f->polygonlist = new tetgenio::polygon[this->Internal->f->numberofpolygons];
	this->Internal->f->numberofholes = 0;
	this->Internal->f->holelist = NULL;
	this->Internal->p = &this->Internal->f->polygonlist[0];
	this->Internal->p->numberofvertices = 4;
	this->Internal->p->vertexlist = new int[this->Internal->p->numberofvertices];

	this->Internal->p->vertexlist[0] = this->numberOfPoints ;
	this->Internal->p->vertexlist[1] = this->numberOfPoints + 1;
	this->Internal->p->vertexlist[2] = this->numberOfPoints + 5;
	this->Internal->p->vertexlist[3] = this->numberOfPoints + 4;

	this->Internal->in.facetmarkerlist[this->numberOfCells + 2] = 1;	// Set 'in.facetmarkerlist': 1-> boundary face
	//---------------------------------------------------------------------------------------------------------

	//----------------------------------------------- Facet 4 -------------------------------------------------
	this->Internal->f = &this->Internal->in.facetlist[this->numberOfCells + 3];
	this->Internal->f->numberofpolygons = 1;
	this->Internal->f->polygonlist = new tetgenio::polygon[this->Internal->f->numberofpolygons];
	this->Internal->f->numberofholes = 0;
	this->Internal->f->holelist = NULL;
	this->Internal->p = &this->Internal->f->polygonlist[0];
	this->Internal->p->numberofvertices = 4;
	this->Internal->p->vertexlist = new int[this->Internal->p->numberofvertices];

	this->Internal->p->vertexlist[0] = this->numberOfPoints + 3;
	this->Internal->p->vertexlist[1] = this->numberOfPoints + 7;
	this->Internal->p->vertexlist[2] = this->numberOfPoints + 6;
	this->Internal->p->vertexlist[3] = this->numberOfPoints + 2;

	this->Internal->in.facetmarkerlist[this->numberOfCells + 3] = 1;	// Set 'in.facetmarkerlist': 1-> boundary face
	//----------------------------------------------------------------------------------------------------------

	//----------------------------------------------- Facet 5 --------------------------------------------------
	this->Internal->f = &this->Internal->in.facetlist[this->numberOfCells + 4];
	this->Internal->f->numberofpolygons = 1;
	this->Internal->f->polygonlist = new tetgenio::polygon[this->Internal->f->numberofpolygons];
	this->Internal->f->numberofholes = 0;
	this->Internal->f->holelist = NULL;
	this->Internal->p = &this->Internal->f->polygonlist[0];
	this->Internal->p->numberofvertices = 4;
	this->Internal->p->vertexlist = new int[this->Internal->p->numberofvertices];

	this->Internal->p->vertexlist[0] = this->numberOfPoints + 1;
	this->Internal->p->vertexlist[1] = this->numberOfPoints + 2;
	this->Internal->p->vertexlist[2] = this->numberOfPoints + 6;
	this->Internal->p->vertexlist[3] = this->numberOfPoints + 5;

	this->Internal->in.facetmarkerlist[this->numberOfCells + 4] = 1;	// Set 'in.facetmarkerlist': 1-> boundary face
	//----------------------------------------------------------------------------------------------------------

	//----------------------------------------------- Facet 6 -------------------------------------------------
	this->Internal->f = &this->Internal->in.facetlist[this->numberOfCells + 5];
	this->Internal->f->numberofpolygons = 1;
	this->Internal->f->polygonlist = new tetgenio::polygon[this->Internal->f->numberofpolygons];
	this->Internal->f->numberofholes = 0;
	this->Internal->f->holelist = NULL;
	this->Internal->p = &this->Internal->f->polygonlist[0];
	this->Internal->p->numberofvertices = 4;
	this->Internal->p->vertexlist = new int[this->Internal->p->numberofvertices];

	this->Internal->p->vertexlist[0] = this->numberOfPoints ;
	this->Internal->p->vertexlist[1] = this->numberOfPoints + 4;
	this->Internal->p->vertexlist[2] = this->numberOfPoints + 7;
	this->Internal->p->vertexlist[3] = this->numberOfPoints + 3;

	this->Internal->in.facetmarkerlist[this->numberOfCells + 5] = 1;	// Set 'in.facetmarkerlist': 1-> boundary face
	//-----------------------------------------------------------------------------------------------------------

	this->Internal->in.pointlist[coordCounter]  = newBounds[0];				
	this->Internal->in.pointlist[coordCounter + 1] = newBounds[2];
	this->Internal->in.pointlist[coordCounter + 2] = newBounds[4];

	this->Internal->in.pointlist[coordCounter + 3] = newBounds[1];
	this->Internal->in.pointlist[coordCounter + 4] = newBounds[2];
	this->Internal->in.pointlist[coordCounter + 5] = newBounds[4];

	this->Internal->in.pointlist[coordCounter + 6] = newBounds[1];
	this->Internal->in.pointlist[coordCounter + 7] = newBounds[3];
	this->Internal->in.pointlist[coordCounter + 8] = newBounds[4];

	this->Internal->in.pointlist[coordCounter + 9] = newBounds[0];
	this->Internal->in.pointlist[coordCounter + 10] = newBounds[3];
	this->Internal->in.pointlist[coordCounter + 11] = newBounds[4];

	this->Internal->in.pointlist[coordCounter + 12] = newBounds[0];
	this->Internal->in.pointlist[coordCounter + 13] = newBounds[2];
	this->Internal->in.pointlist[coordCounter + 14] = newBounds[5];

	this->Internal->in.pointlist[coordCounter + 15] = newBounds[1];
	this->Internal->in.pointlist[coordCounter + 16] = newBounds[2];
	this->Internal->in.pointlist[coordCounter + 17] = newBounds[5];

	this->Internal->in.pointlist[coordCounter + 18] = newBounds[1];
	this->Internal->in.pointlist[coordCounter + 19] = newBounds[3];
	this->Internal->in.pointlist[coordCounter + 20] = newBounds[5];

	this->Internal->in.pointlist[coordCounter + 21] = newBounds[0];
	this->Internal->in.pointlist[coordCounter + 22] = newBounds[3];
	this->Internal->in.pointlist[coordCounter + 23] = newBounds[5];

	this->coordCounter += 24;
	this->numberOfPoints += 8;
	this->numberOfCells += 6;

	if( i == 1)
		for(int j=0; j<6; j++)
		{
			this->bounds1[j] = newBounds[j];
		}
	else
		for(int j=0; j<6; j++)
		{
			this->bounds2[j] = newBounds[j];
		}

}

