// By: Eric Daoust && Matthew Livingstone
#include "vtkDXFLayer.h"
#include <vtksys/ios/sstream>
#include "vtkObjectFactory.h"
#include "vtkPolyData.h"
#include "vtkTriangle.h"
#include "vtkCollection.h"
#include "vtkVectorText.h"
#include "vtkTransform.h"
#include "vtkTransformFilter.h"
#include "vtkDiskSource.h"
#include "vtkTransform.h"
#include "vtkTransformFilter.h"
#include <math.h>

#define PI 3.1415926

vtkCxxRevisionMacro(vtkDXFLayer, "$Revision: 1 $");
vtkStandardNewMacro(vtkDXFLayer);

vtkDXFLayer::vtkDXFLayer()
{
	this->pointPoints = vtkPoints::New();
	this->pointCells = vtkCellArray::New();

	this->arcPoints = vtkPoints::New();
	this->arcCells = vtkCellArray::New();

	this->linePoints = vtkPoints::New();
  this->lineCells = vtkCellArray::New();

	this->polyLinePoints = vtkPoints::New();
  this->polyLineCells = vtkCellArray::New();

	this->lwPolyLinePoints = vtkPoints::New();
  this->lwPolyLineCells = vtkCellArray::New();

	this->surfPoints = vtkPoints::New();
	this->surfCells = vtkCellArray::New();

	this->solidPoints = vtkPoints::New();
	this->solidCells = vtkCellArray::New();

	//Deep copy fails unless we manually state how many components

	this->pointProps = vtkDoubleArray::New();
	this->pointProps->SetNumberOfComponents( 1 );

	this->polyLineProps = vtkDoubleArray::New();
	this->polyLineProps->SetNumberOfComponents( 1 );

	this->lwPolyLineProps = vtkDoubleArray::New();
	this->lwPolyLineProps->SetNumberOfComponents( 1 );

	this->lineProps = vtkDoubleArray::New();
	this->lineProps->SetNumberOfComponents( 1 );

	this->surfProps = vtkDoubleArray::New();
	this->surfProps->SetNumberOfComponents( 1 );

	this->circleProps = vtkDoubleArray::New();
	this->circleProps->SetNumberOfComponents( 1 );

	this->textProps = vtkDoubleArray::New();
	this->textProps->SetNumberOfComponents( 1 );

	this->arcProps = vtkDoubleArray::New();
	this->arcProps->SetNumberOfComponents( 1 );

	this->solidProps = vtkDoubleArray::New();
	this->solidProps->SetNumberOfComponents( 1 );

	this->textList = vtkCollection::New();
	this->circleList = vtkCollection::New();
	this->blockList = vtkCollection::New();

	this->scale = 1.0;
	this->currElevation = 0.0;
	this->largeUnits = 1;
	this->textRotation = 0.0;
	this->layerPropertyValue = 0.0;
	this->freezeValue = 0.0;

}

vtkDXFLayer::~vtkDXFLayer()
{
	this->pointPoints->Delete();
	this->pointCells->Delete();

	this->arcPoints->Delete();
	this->arcCells->Delete();

	this->linePoints->Delete();
	this->lineCells->Delete();

	this->polyLinePoints->Delete();
  this->polyLineCells->Delete();

	this->lwPolyLinePoints->Delete();
  this->lwPolyLineCells->Delete();

	this->surfPoints->Delete();
	this->surfCells->Delete();

	this->solidPoints->Delete();
	this->solidCells->Delete();

	this->pointProps->Delete();
	this->polyLineProps->Delete();
	this->lwPolyLineProps->Delete();
	this->lineProps->Delete();
	this->surfProps->Delete();
	this->circleProps->Delete();
	this->textProps->Delete();
	this->arcProps->Delete();
	this->solidProps->Delete();
  

	this->textList->Delete();
	this->circleList->Delete();
	this->blockList->Delete();

}

#define UnRegisterAndCopy( x, y )\
	if ( x )\
		{\
		x->UnRegister(this);\
		}\
	x = y;\
  if (x)\
    {\
    x->Register(this);\
    }


void vtkDXFLayer::ShallowCopy(vtkDXFLayer *object)
{
	if ( !object )
		{
		return; //we failed!
		}
	
	UnRegisterAndCopy(this->pointPoints, object->pointPoints );
	
	UnRegisterAndCopy(this->pointCells, object->pointCells);

	UnRegisterAndCopy(this->arcPoints, object->arcPoints);
  
	UnRegisterAndCopy(this->arcCells, object->arcCells);
  
	UnRegisterAndCopy(this->linePoints, object->linePoints);
  
	UnRegisterAndCopy(this->lineCells, object->lineCells);
  
	UnRegisterAndCopy(this->polyLinePoints, object->polyLinePoints);
  
	UnRegisterAndCopy(this->polyLineCells, object->polyLineCells);
  
	UnRegisterAndCopy(this->lwPolyLinePoints, object->lwPolyLinePoints);
  
	UnRegisterAndCopy(this->lwPolyLineCells, object->lwPolyLineCells);
  
	UnRegisterAndCopy(this->surfPoints, object->surfPoints);
  
	UnRegisterAndCopy(this->surfCells, object->surfCells);
  
	UnRegisterAndCopy(this->solidPoints, object->solidPoints);
  
	UnRegisterAndCopy(this->solidCells, object->solidCells);
  
	UnRegisterAndCopy(this->pointProps, object->pointProps);
  
	UnRegisterAndCopy(this->polyLineProps, object->polyLineProps);
  
	UnRegisterAndCopy(this->lwPolyLineProps, object->lwPolyLineProps);
  
	UnRegisterAndCopy(this->lineProps, object->lineProps);
  
	UnRegisterAndCopy(this->surfProps, object->surfProps);
  
	UnRegisterAndCopy(this->circleProps, object->circleProps);
  
	UnRegisterAndCopy(this->textProps, object->textProps);
 
	UnRegisterAndCopy(this->arcProps, object->arcProps);
  
	UnRegisterAndCopy(this->solidProps, object->solidProps);
  
	UnRegisterAndCopy(this->textList, object->textList);
 
	UnRegisterAndCopy(this->circleList, object->circleList);  

	UnRegisterAndCopy(this->blockList, object->blockList);
    
	this->scale = object->scale;
	this->currElevation = object->currElevation;
	this->largeUnits = object->largeUnits;
	this->textRotation = object->textRotation;

	this->freezeValue = object->freezeValue;

}

void vtkDXFLayer::PrintSelf(ostream& os, vtkIndent indent)
{
	//TODO: complete this method
	this->Superclass::PrintSelf(os,indent);
}