#include "vtkPVStereoNetFilter.h"

#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"

#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkMath.h"

#include <stack>

vtkCxxRevisionMacro(vtkPVStereoNetFilter, "$Revision: 0.1 $");
vtkStandardNewMacro(vtkPVStereoNetFilter);

//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
class vtkInternalPlane
{
public:    
	vtkInternalPlane( double first[3], double second[3], double third[3]);
	vtkInternalPlane();
	~vtkInternalPlane();
	void TransformPlane(double pt[31][3]);
	
	double a[3][3];

	//BTX
	enum
		{
		Wulff = 0,
		Schmidt = 1
		};
	//ETX
};
//----------------------------------------------------------------------------
vtkInternalPlane::vtkInternalPlane(){}

//----------------------------------------------------------------------------
vtkInternalPlane::vtkInternalPlane( double first[3], double second[3],double third[3])
{
	double temp;
	for(int i = 0; i < 3; i++)
		{
		this->a[0][i] = first[i];
		this->a[1][i] = second[i];
		this->a[2][i] = third[i];
		}
	//sort so that (a[0][2] >= a[1][2] >= a[2][2])
	for(int i = 0; i < 2; i++)
		{
		for(int j = i; j < 3; j++)
			{
			if(this->a[i][2] < this->a[j][2])
				{
				temp = this->a[i][0];
				this->a[i][0] = this->a[j][0];
				this->a[j][0] = temp;
				temp = this->a[i][1];
				this->a[i][1] = this->a[j][1];
				this->a[j][1] = temp;
				temp = this->a[i][2];
				this->a[i][2] = this->a[j][2];
				this->a[j][2] = temp;
				}
			}
		}
}

//----------------------------------------------------------------------------
vtkInternalPlane::~vtkInternalPlane(){}

//----------------------------------------------------------------------------
void vtkInternalPlane::TransformPlane(double pt[31][3])
{
	// 3 points define a plane
	// a[0][2] >= a[1][2] >= a[2][2]
	// a unit vector in the same plan, but with z=0
	double alpha = ( this->a[1][2] - this->a[2][2] )/( this->a[0][2] - this->a[2][2] );
	double vect1[3];
	vect1[0] = alpha * ( this->a[0][0] - this->a[1][0] ) + (1.0-alpha)*( this->a[2][0] - this->a[1][0] );
	vect1[1] = alpha * ( this->a[0][1] - this->a[1][1] ) + (1.0-alpha)*( this->a[2][1] - this->a[1][1] );
	vect1[2] = 0.0;
	double norme = sqrt( vect1[0]*vect1[0] + vect1[1]*vect1[1] );
	vect1[0] /= norme;
	vect1[1] /= norme;

	// another unit vector in the plan (oriented towards z<0)
	double vect2[3];
	vect2[0] = this->a[2][0] - this->a[0][0];
	vect2[1] = this->a[2][1] - this->a[0][1];
	vect2[2] = this->a[2][2] - this->a[0][2];
	norme = sqrt( vect2[0]*vect2[0] + vect2[1]*vect2[1] + vect2[2]*vect2[2] );
	vect2[0] /= norme;
	vect2[1] /= norme;
	vect2[2] /= norme;

	// Now discretize the arc through points vect1, vect2, -vect1
	const int npt = 31;
	//double pt[npt][3];
	double dt = 1.0/(npt-1);
	double c1,c2;
	for (int i = 0; i < npt; i++ )
	 {
			c1 = 2*i*dt-1.0;  // from -1 to 1
			c2 = 1.0 - fabs( c1 ); // from 0 to 1 to 0 again
			pt[i][0] = c1*vect1[0] + c2*vect2[0];
			pt[i][1] = c1*vect1[1] + c2*vect2[1];
			pt[i][2] = c1*vect1[2] + c2*vect2[2];
			norme = sqrt( pt[i][0]*pt[i][0] + pt[i][1]*pt[i][1] + pt[i][2]*pt[i][2] );
			pt[i][0] /= norme;
			pt[i][1] /= norme;
			pt[i][2] /= norme;
	 }
}
//End of vtkInternalPlane
// ---------------------------------------------------------------------------

//----------------------------------------------------------------------------
vtkPVStereoNetFilter::vtkPVStereoNetFilter()
{
	this->Mode = 1;
}

//----------------------------------------------------------------------------
vtkPVStereoNetFilter::~vtkPVStereoNetFilter(){}

//----------------------------------------------------------------------------
int vtkPVStereoNetFilter::FillInputPortInformation(int port, vtkInformation *info)
{
	info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData");
	info->Set(vtkAlgorithm::INPUT_IS_REPEATABLE(), 1);
	return 1;
}

//----------------------------------------------------------------------------
int vtkPVStereoNetFilter::FillOutputPortInformation(int port, vtkInformation *info)
{
	info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkPolyData");
	return 1;
}

//----------------------------------------------------------------------------
int vtkPVStereoNetFilter::RequestInformation(vtkInformation *request,vtkInformationVector **inputVector,vtkInformationVector *outputVector)
{return 1;}

//----------------------------------------------------------------------------
int vtkPVStereoNetFilter::RequestData(vtkInformation *request,vtkInformationVector **inputVector,vtkInformationVector *outputVector)
{	
	//get the output 
	vtkInformation *outInfo = outputVector->GetInformationObject(0);
	vtkPolyData *output = vtkPolyData::SafeDownCast( outInfo->Get(vtkDataObject::DATA_OBJECT()));

	//find the number of inputs
	int num = inputVector[0]->GetNumberOfInformationObjects();
	vtkInformation *inInfo;
	vtkPolyData *input;
	
	vtkCellArray *polys = vtkCellArray::New();
	vtkCellArray *lines = vtkCellArray::New();
	vtkPoints *points = vtkPoints::New();
	for ( int i=0; i < num ; i++)
		{
		inInfo = inputVector[0]->GetInformationObject( i );
		input = vtkPolyData::SafeDownCast( inInfo->Get(vtkDataObject::DATA_OBJECT()) );			

		//list draw the points and lines on the stereo net.
		this->unitCircle(polys, points);
		this->TransformLines(input->GetLines(),input->GetPoints(), lines, points);
		this->TransformPlanes(input->GetPolys(), input->GetPoints(), polys, points );
		}
	
	output->SetLines( polys );
	output->SetVerts(lines);
	output->SetPoints( points );
	
	polys->Delete();
	lines->Delete();
	points->Delete();

	//now create an unique id to set as your scalar 
	/*vtkIntArray *cellId = vtkIntArray::New();
	cellId->SetName("uid");
	for ( int i=0; i < output->GetNumberOfCells(); i++)
		{
		cellId->InsertNextValue( i );
		}

	output->GetCellData()->SetScalars( cellId );
	cellId->Delete();*/
			
	return 1;
}

//----------------------------------------------------------------------------
void vtkPVStereoNetFilter::unitCircle(vtkCellArray *outCells, vtkPoints *outPoints)
{
	double pts[6][3];
	//x*x + y*y = 1
	for(int i = 0; i < 6; i++)
		{
		pts[i][0] =	sqrt( i / 10.0 );
		pts[i][1] =	sqrt( (1.0 - (i/10.0)) );
		pts[i][2] =	0.0;
		}
	//using 8 way symmetry
	for(int i = 0; i < 4; i++)
		{
		//x,y
		outCells->InsertNextCell(3);
		outCells->InsertCellPoint(outPoints->InsertNextPoint( pts[i][0],  pts[i][1], pts[i][2]) );
		outCells->InsertCellPoint(outPoints->InsertNextPoint( pts[i+1][0],  pts[i+1][1], pts[i+1][2] ) );
		outCells->InsertCellPoint(outPoints->InsertNextPoint( pts[i+2][0],  pts[i+2][1], pts[i+2][2] ) );
		outCells->UpdateCellCount(3);
		//y,x
		outCells->InsertNextCell(3);
		outCells->InsertCellPoint(outPoints->InsertNextPoint( pts[i][1],  pts[i][0], pts[i][2]) );
		outCells->InsertCellPoint(outPoints->InsertNextPoint( pts[i+1][1],  pts[i+1][0], pts[i+1][2] ) );
		outCells->InsertCellPoint(outPoints->InsertNextPoint( pts[i+2][1],  pts[i+2][0], pts[i+2][2] ) );
		outCells->UpdateCellCount(3);
		//-x,y
		outCells->InsertNextCell(3);
		outCells->InsertCellPoint(outPoints->InsertNextPoint( -(pts[i][0]),  pts[i][1], pts[i][2]) );
		outCells->InsertCellPoint(outPoints->InsertNextPoint( -(pts[i+1][0]),  pts[i+1][1], pts[i+1][2] ) );
		outCells->InsertCellPoint(outPoints->InsertNextPoint( -(pts[i+2][0]),  pts[i+2][1], pts[i+2][2] ) );
		outCells->UpdateCellCount(3);
		//y,-x
		outCells->InsertNextCell(3);
		outCells->InsertCellPoint(outPoints->InsertNextPoint( pts[i][1],  -(pts[i][0]), pts[i][2]) );
		outCells->InsertCellPoint(outPoints->InsertNextPoint( pts[i+1][1],  -(pts[i+1][0]), pts[i+1][2] ) );
		outCells->InsertCellPoint(outPoints->InsertNextPoint( pts[i+2][1],  -(pts[i+2][0]), pts[i+2][2] ) );
		outCells->UpdateCellCount(3);
		//x,-y
		outCells->InsertNextCell(3);
		outCells->InsertCellPoint(outPoints->InsertNextPoint( pts[i][0],  -(pts[i][1]), pts[i][2]) );
		outCells->InsertCellPoint(outPoints->InsertNextPoint( pts[i+1][0], -(pts[i+1][1]), pts[i+1][2] ) );
		outCells->InsertCellPoint(outPoints->InsertNextPoint( pts[i+2][0], -(pts[i+2][1]), pts[i+2][2] ) );
		outCells->UpdateCellCount(3);
		//-y,x
		outCells->InsertNextCell(3);
		outCells->InsertCellPoint(outPoints->InsertNextPoint( -(pts[i][1]),  pts[i][0], pts[i][2]) );
		outCells->InsertCellPoint(outPoints->InsertNextPoint( -(pts[i+1][1]),  pts[i+1][0], pts[i+1][2] ) );
		outCells->InsertCellPoint(outPoints->InsertNextPoint( -(pts[i+2][1]),  pts[i+2][0], pts[i+2][2] ) );
		outCells->UpdateCellCount(3);
		//-x,-y
		outCells->InsertNextCell(3);
		outCells->InsertCellPoint(outPoints->InsertNextPoint( -(pts[i][0]),  -(pts[i][1]), pts[i][2]) );
		outCells->InsertCellPoint(outPoints->InsertNextPoint( -(pts[i+1][0]),  -(pts[i+1][1]), pts[i+1][2] ) );
		outCells->InsertCellPoint(outPoints->InsertNextPoint( -(pts[i+2][0]),  -(pts[i+2][1]), pts[i+2][2] ) );
		outCells->UpdateCellCount(3);
		//-y,-x
		outCells->InsertNextCell(3);
		outCells->InsertCellPoint(outPoints->InsertNextPoint( -(pts[i][1]),  -(pts[i][0]), pts[i][2]) );
		outCells->InsertCellPoint(outPoints->InsertNextPoint( -(pts[i+1][1]),  -(pts[i+1][0]), pts[i+1][2] ) );
		outCells->InsertCellPoint(outPoints->InsertNextPoint( -(pts[i+2][1]),  -(pts[i+2][0]), pts[i+2][2] ) );
		outCells->UpdateCellCount(3);
		}	
}

//----------------------------------------------------------------------------
void vtkPVStereoNetFilter::TransformPlanes( vtkCellArray *inCells, vtkPoints *inPoints, vtkCellArray *outCells, vtkPoints *outPoints )
{
	if (inCells)
		{
		inCells->InitTraversal();
		vtkIdType npts, *pts;
		double first[3],second[3],third[3];
		vtkInternalPlane *plane;
		
		double X[2] = {1,0};
		for(int i = 0; i < inCells->GetNumberOfCells(); i++)
			{
			inCells->GetNextCell(npts, pts);
			if ( npts == 3 || npts == 4 )
				{				
				//get three points on a plane				
				inPoints->GetPoint(pts[0],first);
				inPoints->GetPoint(pts[1],second);
				inPoints->GetPoint(pts[2],third);
				plane = new vtkInternalPlane( first, second, third );	

				double pt[31][3];
				plane->TransformPlane(pt);

				for(int i = 0; i < 31; i++)
					{
					this->TransformCoordinate(pt[i]);
					}

				//output cells
				for(int i = 0; i < 30; i+=2)
					{
					outCells->InsertNextCell(3);
					outCells->InsertCellPoint(outPoints->InsertNextPoint( pt[i] ) );
					outCells->InsertCellPoint(outPoints->InsertNextPoint( pt[i+1] ) );
					outCells->InsertCellPoint(outPoints->InsertNextPoint( pt[i+2] ) );
					outCells->UpdateCellCount(3);
					}
				}				
			}
		}
}

//----------------------------------------------------------------------------
void vtkPVStereoNetFilter::TransformLines( vtkCellArray *inCells, vtkPoints *inPoints, vtkCellArray *outCells, vtkPoints *outPoints )
{
	//make sure we have inCells
	if (inCells)
		{
		inCells->InitTraversal();
		vtkIdType npts, *pts;
		double firstPoint[3],secondPoint[3],vector[3];
		vtkIdType transformId;
		for(int i = 0; i < inCells->GetNumberOfCells(); i++)
			{
			inCells->GetNextCell(npts, pts);
			outCells->InsertNextCell( npts-1 );
			for ( int j = 0; j < (npts-1); j++)
				{
				//get the first point
				inPoints->GetPoint(pts[j],firstPoint);				

				//get the second point
				inPoints->GetPoint(pts[j+1],secondPoint);				

				//now get the vector of the line between the two points
				vector[0] = secondPoint[0] - firstPoint[0];
				vector[1] = secondPoint[1] - firstPoint[1];
				vector[2] = secondPoint[2] - firstPoint[2];

				//now normalize the vector UNIT VECTOR
				vtkMath::Normalize(vector);

				//get intersection
				this->LineIntersect(vector);
				
				//transform the vector to the proper coordinates
				this->TransformCoordinate(vector);

				//add to points
				transformId = outPoints->InsertNextPoint( vector );
				outCells->InsertCellPoint( transformId );
				}			
			}
		}
}

//----------------------------------------------------------------------------
void vtkPVStereoNetFilter::LineIntersect( double *vector )
{
	double firstPoint[3],secondPoint[3];

	double F = (vector[0] * vector[0]) +  (vector[1] * vector[1]) + (vector[2] * vector[2]);
				
	double K1 =  ( - sqrt( -F * (-1) )) / F;
	double K2 =  ( + sqrt( -F * (-1) )) / F;

	firstPoint[0] = vector[0] * K1;
	firstPoint[1] = vector[1] * K1;
	firstPoint[2] = vector[2] * K1;

	secondPoint[0] = vector[0] * K2;
	secondPoint[1] = vector[1] * K2;
	secondPoint[2] = vector[2] * K2;

	if(firstPoint[2] < 0)
		{
		vector[0] = firstPoint[0];
		vector[1] = firstPoint[1];
		vector[2] = firstPoint[2];
		}
	else
		{
		vector[0] = secondPoint[0];
		vector[1] = secondPoint[1];
		vector[2] = secondPoint[2];
		}
}

//----------------------------------------------------------------------------
bool vtkPVStereoNetFilter::TransformCoordinate( double *xyz )
{
	//value = 1 - Z;
	double value = 1.0 - xyz[2];

	if(value == 0) //no division by 0
		{
		return false;
		}

	//2d Transformation to Schmidt or Wulff Projection
	if ( this->Mode == vtkInternalPlane::Schmidt)
		{//equal area
		//http://en.wikipedia.org/wiki/Lambert_azimuthal_equal-area_projection	
		//(X,Y) = (sqrt(2/1-z) * X , sqrt(2/1-z) * Y)
		//xyz[0] *= sqrt( 2 / value);
		//xyz[1] *= sqrt( 2 / value); 
		//no transformation needed because on the sphere is already
		///schmidt
		xyz[2] = 0.0;		
		}
	else if (this->Mode == vtkInternalPlane::Wulff)
		{//equal angle
		//http://en.wikipedia.org/wiki/Stereographic_projection
		//(X,Y) = ( x/1-z , y/1-z) 
		xyz[0] /= value ;
		xyz[1] /= value ;
		xyz[2] = 0.0;
		}
	else
		{
		return false;
		}

	return true;//Valid output
}

//----------------------------------------------------------------------------
void vtkPVStereoNetFilter::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os,indent);

		os << indent << "Mode" << this->GetMode() << endl;
}

