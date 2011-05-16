// By: Eric Daoust && Matthew Livingstone
#include "vtkDXFBlock.h"
#include <vtksys/ios/sstream>
#include "vtkObjectFactory.h"
#include "vtkPolyData.h"
#include "vtkCollection.h"
#include "vtkTransform.h"
#include "vtkTransformFilter.h"

vtkCxxRevisionMacro(vtkDXFBlock, "$Revision: 1 $");
vtkStandardNewMacro(vtkDXFBlock);

vtkDXFBlock::vtkDXFBlock()
{
	this->blockScale = new double[3];
	this->blockScale[0] = this->blockScale[1] = this->blockScale[2] = 1.0;

	this->blockTransform = new double[3];
	this->blockTransform[0] = this->blockTransform[1] = this->blockTransform[2] = 0.0;

	this->parentLayer = "";
	this->drawBlock = false;

	this->blockPropertyValue = 0;
}

vtkDXFBlock::~vtkDXFBlock()
{
	if ( this->blockScale )
		{
		delete[] this->blockScale;
		}

	if ( this->blockTransform )
		{
		delete[] this->blockTransform;
		}	
}

void vtkDXFBlock::PrintSelf(ostream& os, vtkIndent indent)
{
	//TODO: complete this method
	this->Superclass::PrintSelf(os,indent);
}

void vtkDXFBlock::CopyFrom(vtkDXFBlock* block)
{
	this->name = block->getName();

	// Point/Cell Data
	this->pointPoints->DeepCopy(block->getPointPoints());
	this->pointCells->DeepCopy(block->getPointCells());
	this->linePoints->DeepCopy(block->getLinePoints());
	this->lineCells->DeepCopy(block->getLineCells());
	this->polyLinePoints->DeepCopy(block->getPolyLinePoints());
	this->polyLineCells->DeepCopy(block->getPolyLineCells());
	this->lwPolyLinePoints->DeepCopy(block->getLWPolyLinePoints());
	this->lwPolyLineCells->DeepCopy(block->getLWPolyLineCells());
	this->surfPoints->DeepCopy(block->getSurfPoints());
	this->surfCells->DeepCopy(block->getSurfCells());
	this->solidPoints->DeepCopy(block->getSolidPoints());
	this->solidCells->DeepCopy(block->getSolidCells());
	this->arcPoints->DeepCopy(block->getArcPoints());
	this->arcCells->DeepCopy(block->getArcCells());
	
	for(int textItem = 0; textItem < block->getText()->GetNumberOfItems(); textItem++)
	{
		this->textList->AddItem(vtkPolyData::SafeDownCast(block->getText()->GetItemAsObject(textItem)));
	}

	for(int circleItem = 0; circleItem < block->getCircles()->GetNumberOfItems(); circleItem++)
	{
		this->circleList->AddItem(vtkPolyData::SafeDownCast(block->getCircles()->GetItemAsObject(circleItem)));
	}

	// Properties
	this->pointProps->DeepCopy(block->getPointProps());
	this->lineProps->DeepCopy(block->getLineProps());
	this->polyLineProps->DeepCopy(block->getPolyLineProps());
	this->lwPolyLineProps->DeepCopy(block->getLWPolyLineProps());
	this->surfProps->DeepCopy(block->getSurfProps());
	this->solidProps->DeepCopy(block->getSolidProps());
	this->arcProps->DeepCopy(block->getArcProps());
	this->circleProps->DeepCopy( block->getCircleProps() );
	this->textProps->DeepCopy(block->getTextProps());
}