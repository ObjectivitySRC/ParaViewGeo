// .NAME vtkPolyLineAddPoints - Add points to an existing PolyLine
// .SECTION Description
// Author: Matthew Livingstone -- February 2009

#include "vtkPolyLineAddPoints.h"
#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkObjectFactory.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"

vtkCxxRevisionMacro(vtkPolyLineAddPoints, "$Revision: 1.00 $");
vtkStandardNewMacro(vtkPolyLineAddPoints);

vtkPolyLineAddPoints::vtkPolyLineAddPoints()
{
	this->AddFactor = 0;
}

vtkPolyLineAddPoints::~vtkPolyLineAddPoints()
{
}

int vtkPolyLineAddPoints::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
	vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

	vtkPolyData *input = vtkPolyData::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
	vtkPolyData *output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

  vtkIdType *cellPts;
	vtkIdType numCellPts;

	vtkPoints *outPoints = vtkPoints::New();
	vtkCellArray *outCells = vtkCellArray::New();

	// Keep track of point number
	int pointCounter = 0;
	// Used to determine if the points are to be added in the positive or negative direction
	// 0 -> X, 1 -> Y, 2 -> Z
	bool* dirPositive = new bool[3];
	dirPositive[0] = dirPositive[1] = dirPositive[2] = true;

	//Create the cell for the line
	int pointInterNum = 0;

	// Iterate over all but the last point
	vtkCellArray *lines = input->GetLines();
	lines->InitTraversal();
	for(int cellNum = 0; cellNum < lines->GetNumberOfCells(); cellNum++)
		{
		lines->GetNextCell(numCellPts,cellPts);

		int size = numCellPts + ( (numCellPts - 1) * (this->AddFactor-1) );
		outCells->InsertNextCell( size );
		
		double tempPoint[3];
		vtkIdType realPointId;
		for(int pointNum = 0; pointNum < numCellPts -1; pointNum++)
			{
			// Make sure we reset which direction the points are moving in
			// Otherwise the points will be added in the wrong direction
			//  if the previous points were added in the reverse direction.
			dirPositive[0] = dirPositive[1] = dirPositive[2] = true;
			realPointId = cellPts[pointNum];
			input->GetPoint(realPointId,tempPoint);
			
			// Keep the original points
			pointCounter = outPoints->InsertNextPoint(tempPoint);
			outCells->InsertCellPoint(pointCounter);

			// Calculate the distance between two of the original points
			double* dirDifference = new double[3];
			double* origPos = new double[3];
			dirDifference[0] = origPos[0] = tempPoint[0];
			dirDifference[1] = origPos[1] = tempPoint[1];
			dirDifference[2] = origPos[2] = tempPoint[2];

			// Grab the next ORIGINAL point
			realPointId = cellPts[pointNum+1];
			input->GetPoint(realPointId,tempPoint);

			// Determine which direction the points are to be added in
			dirPositive[0] = (origPos[0] > tempPoint[0]) ? false : true;
			dirPositive[1] = (origPos[1] > tempPoint[1]) ? false : true;
			dirPositive[2] = (origPos[2] > tempPoint[2]) ? false : true;
			// Make sure there are no tricky negatives left over
			dirDifference[0] = abs(dirDifference[0] - tempPoint[0]);
			dirDifference[1] = abs(dirDifference[1] - tempPoint[1]);
			dirDifference[2] = abs(dirDifference[2] - tempPoint[2]);

			// Calculate how far away the next point(s) should be
			double* addDist = new double[3];
			addDist[0] = dirDifference[0]/(this->AddFactor);
			addDist[1] = dirDifference[1]/(this->AddFactor);
			addDist[2] = dirDifference[2]/(this->AddFactor);

			// Add n (n=addFactor) new points
			for(int addPointCount = 1; addPointCount < this->AddFactor; addPointCount++)
				{
				// Stores location of next new point
				double* newLoc = new double[3];
				if (dirPositive[0])
					{
					newLoc[0] = origPos[0]+(addDist[0]*addPointCount);
					} else
					{
					newLoc[0] = origPos[0]-(addDist[0]*addPointCount);
					}
				if (dirPositive[1])
					{
					newLoc[1] = origPos[1]+(addDist[1]*addPointCount);
					} else
					{
					newLoc[1] = origPos[1]-(addDist[1]*addPointCount);
					}
				if (dirPositive[2])
					{
					newLoc[2] = origPos[2]+(addDist[2]*addPointCount);
					} else
					{
					newLoc[2] = origPos[2]-(addDist[2]*addPointCount);
					}

				pointCounter = outPoints->InsertNextPoint( newLoc );
				outCells->InsertCellPoint( pointCounter );

				// Housekeeping
				delete[] newLoc;
				}

			// Housekeeping
			delete[] dirDifference;
			delete[] addDist;
			delete[] origPos;
			}
		
			// Add the last point in the cell
			realPointId = cellPts[numCellPts -1];
			input->GetPoint(realPointId,tempPoint);
				
			pointCounter = outPoints->InsertNextPoint(tempPoint);
			outCells->InsertCellPoint(pointCounter);
		}

	// Housekeeping
	output->SetPoints(outPoints);
	output->SetLines(outCells);
	outPoints->Delete();
	outCells->Delete();
	delete[] dirPositive;

	return 1;

}

int vtkPolyLineAddPoints::FillInputPortInformation ( int vtkNotUsed(port), vtkInformation* info )
  {
  info->Set ( vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData", 0 );
  return 1;
  }

void vtkPolyLineAddPoints::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "Add Factor: " << this->AddFactor << "\n";
}
