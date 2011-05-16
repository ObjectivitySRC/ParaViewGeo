#include "vtkProximityToLines.h"

#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"

#include "vtkMath.h"
#include "vtkPlane.h"
#include "vtkLine.h"

#include "vtkMath.h"
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"
#include "vtkPointData.h"
#include "vtkPointLocator.h"
#include "vtkCylinder.h"
#include "vtkExtractGeometry.h"
#include "vtkImplicitBoolean.h"
#include "vtkPlanes.h"
#include "vtkCoordinateSystemMapper.h"
#include "vtkDoubleArray.h"
#include "vtkUnstructuredGrid.h"
#include "vtkDataSetSurfaceFilter.h"
#include "vtkGeneralTransform.h"
#include "vtkTransform.h"

#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <time.h>

using namespace std;

vtkCxxRevisionMacro(vtkProximityToLines, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkProximityToLines);

//--------------------------------------------------------------------------------------
vtkProximityToLines::vtkProximityToLines()
{
	this->SetNumberOfInputPorts(2);
}
//--------------------------------------------------------------------------------------
vtkProximityToLines:: ~vtkProximityToLines(){}


//----------------------------------------------------------------------------
// SetSourceConnection
//----------------------------------------------------------------------------
void vtkProximityToLines::SetSourceConnection( vtkAlgorithmOutput *algOutput )
	{
	this->SetInputConnection( 1, algOutput );
	}

//----------------------------------------------------------------------------
// Fill Input Port Information (Source and Input here)
//----------------------------------------------------------------------------
int vtkProximityToLines::FillInputPortInformation ( int , vtkInformation* info )
  {
  info->Set ( vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData", 0 );
  info->Set ( vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData", 1 );
  info->Set ( vtkAlgorithm::INPUT_IS_OPTIONAL(), 1);
  return 1;
  }


//--------------------------------------------------------------------------------------
int vtkProximityToLines::RequestData(vtkInformation *vtkNotUsed(request),
	vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{
  // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *sourceInfo = inputVector[1]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  output = vtkPolyData::SafeDownCast(
		outInfo->Get(vtkDataObject::DATA_OBJECT()));

  this->input = vtkPolyData::SafeDownCast (
		inInfo->Get(vtkDataObject::DATA_OBJECT()));
  if (!this->input)
    {
    vtkErrorMacro(<<"The Input is null.");
    return 0;
    }

	this->source = vtkPolyData::SafeDownCast (
			sourceInfo->Get(vtkDataObject::DATA_OBJECT()));
	if(!this->source)
	{
		vtkErrorMacro("The source is null");
		return 0;
	}

	vtkPoints* inPoints = this->input->GetPoints();
	vtkPoints* srcPoints = this->source->GetPoints();
	vtkCellArray* srcLines = this->source->GetLines();

	if(this->NClosestLines > srcLines->GetNumberOfCells())
	{
		vtkErrorMacro("NClosestLines must be smaller or equal to the number of lines in the source");
		return 0;
	}
	if(this->NClosestLines <=0)
	{
		vtkErrorMacro("NClosestLines must be greater than 0");
		return 0;
	}

	vector<vtkDoubleArray*> distanceArrays(this->NClosestLines);
	vector<vtkIntArray*> closestLineIdArrays(this->NClosestLines);
	for(int i=0; i<this->NClosestLines; ++i)
	{
		distanceArrays[i] = vtkDoubleArray::New();
		closestLineIdArrays[i] = vtkIntArray::New();

		std::ostringstream out;
		out << i;
		
		string name = "shortest distance " + out.str();
		distanceArrays[i]->SetName(name.c_str());
		
		name = "closest line " + out.str();
		closestLineIdArrays[i]->SetName(name.c_str());
	}

	for(vtkIdType i=0; i<inPoints->GetNumberOfPoints(); ++i)
	{
		double pt[3];
		inPoints->GetPoint(i, pt);
		
		// this multimap will contain the distances to the lines as keys and 
		// the id of the lines as values. In this way we will now what are the distance
		// of the closest lines and what are the id's of these lines.
		// note that a multimap is sorted, so the first element will be always the 
		// shortest distance and the closest line.
		multimap<double, vtkIdType> closestLines;
		srcLines->InitTraversal();
		for(vtkIdType j=0; j<srcLines->GetNumberOfCells(); ++j)
		{
			vtkIdType npts;
			vtkIdType* pts;
			srcLines->GetNextCell(npts, pts);
			if(npts < 2)
				continue;

			double pt1[3];
			double pt2[3];
			srcPoints->GetPoint(pts[0], pt1);
			srcPoints->GetPoint(pts[npts-1], pt2);
			double t;
			double closestPoint[3];
			double d = sqrt(vtkLine::DistanceToLine(pt, pt1, pt2, t, closestPoint)); 
			closestLines.insert(pair<double, vtkIdType>(d, j));
		}
		int pos = 0;
		for(multimap<double,vtkIdType>::iterator it = closestLines.begin();
			it != closestLines.end(); ++it)
		{
			if(pos == this->NClosestLines)
				break;
			distanceArrays[pos]->InsertNextValue(it->first);
			closestLineIdArrays[pos]->InsertNextValue(it->second);
			++pos;
		}
	}

	this->output->ShallowCopy(this->input);

	for(int i=0; i<this->NClosestLines; ++i)
	{
		this->output->GetPointData()->AddArray(distanceArrays[i]);
		this->output->GetPointData()->AddArray(closestLineIdArrays[i]);
		distanceArrays[i]->Delete();
		closestLineIdArrays[i]->Delete();
	}


	return 1;
}








