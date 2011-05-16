#include "vtkRuler.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPolyDataAlgorithm.h"
#include "vtkPolyData.h"
#include "vtkMath.h"
#include <math.h>

vtkCxxRevisionMacro(vtkRuler, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkRuler);

vtkRuler::vtkRuler()
{
  this->SetNumberOfInputPorts(1);
  this->SetNumberOfOutputPorts(1);
  for (int i=0; i<3; i++)
    {
    this->Point1[i] = this->Point2[i] = 0.0;
    }
	this->distance = -1;
	this->xDistance = -1;
	this->yDistance = -1;
	this->zDistance = -1;
}

vtkRuler::~vtkRuler()
{
}


int vtkRuler::RequestData(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{
	vtkInformation *inputInfo = inputVector[0]->GetInformationObject ( 0 );
	vtkInformation *outInfo = outputVector->GetInformationObject ( 0 );

	vtkPolyData* input = vtkPolyData::SafeDownCast (
                         inputInfo->Get ( vtkDataObject::DATA_OBJECT() ) );

	vtkPolyData *output = vtkPolyData::SafeDownCast (
	outInfo->Get ( vtkDataObject::DATA_OBJECT() ) );
	output->ShallowCopy(input); 
	calculateDistances();
	this->Modified();
  return 1;
}

int vtkRuler::RequestInformation(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{
  return 1;
}

void vtkRuler::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

void vtkRuler::calculateDistances()
{
  this->distance = sqrt(pow((Point1[0]-Point2[0]),2)+pow((Point1[1]-Point2[1]),2)+pow((Point1[2]-Point2[2]),2));
	this->xDistance = abs(Point1[0]-Point2[0]);
	this->yDistance = abs(Point1[1]-Point2[1]);
	this->zDistance = abs(Point1[2]-Point2[2]);
}

