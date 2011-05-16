// .NAME vtkDXFObject.cxx
// By: Eric Daoust && Matthew Livingstone
// (Boolean statement above returns true)
// Read DXF file (.dxf) for single objects.

#include "vtkDXFObject.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkCellArray.h"
#include "vtkMultiBlockDataSet.h"
#include "vtkSmartPointer.h"
#include "vtkTriangle.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkCollection.h"
#include <vtksys/ios/sstream>
#include "vtkDXFLayer.h"
#include "vtkDXFParser.h"
#include "vtkCleanPolyData.h"
#include "vtkVectorText.h"
#include "vtkAppendPolyData.h"
#include "vtkTransform.h"
#include "vtkTransformFilter.h"

#define POINT_NAME "Points"
#define LINE_NAME "Lines"
#define POLYLINE_NAME "PolyLines"
#define LWPOLY_NAME "LWPolyLines"
#define SURFACE_NAME "Surfaces"
#define TEXT_NAME "Text"
#define CIRCLE_NAME "Circles"
#define ARC_NAME "Arcs"
#define SOLID_NAME "Solids"
#define PROPERTY_NAME "Layer"

vtkCxxRevisionMacro(vtkDXFObject, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkDXFObject);

// Constructor
vtkDXFObject::vtkDXFObject()
{
	this->SetNumberOfOutputPorts(1);
	this->FileName = 0;
	this->CommandLine = 0;
	this->layerExists = 0;
	this->SetNumberOfInputPorts(0);
};

// --------------------------------------
// Destructor
vtkDXFObject::~vtkDXFObject()
{
	this->SetFileName(0);
	this->CommandLine = 0;
	this->ValueLine = "";
}

// --------------------------------------
void vtkDXFObject::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent <<  "FileName: "
     << (this->FileName ? this->FileName : "(none)") << "\n";
}

// --------------------------------------
int vtkDXFObject::RequestData(vtkInformation* request,
                          vtkInformationVector** inputVector,
                          vtkInformationVector* outputVector)
{
	vtkMultiBlockDataSet *output;
	output = (vtkMultiBlockDataSet*)this->GetExecutive()->GetOutputData(0);
	vtkDXFParser *mainLayer = vtkDXFParser::New();
	vtkDXFObjectMap *layerList = mainLayer->ParseData(this->FileName, this->DrawHidden, this->AutoScale);
	
	//Delete mainLayer, and clean up toooons of memory
	mainLayer->Delete();
	
	if (layerList == NULL)
	  {
	  return 1;
	  }	
	int layerCount = -1;

	// Create our final MultiBlockDataSet.
	// Each item in the layerList is added as a block
	vtkDXFLayer *currLayer;
	 
	for(int i = 0; i < layerList->GetNumberOfItems(); i++)
	{
		// Create the name for the later
		std::string s;
		std::stringstream out;
		layerCount++;
		out << layerCount;
		s = out.str();
		// Grab layer that will be added from the list of layers
		currLayer = layerList->GetItemAsObject(i);		
		//currLayer->ApplyProperties();
		int blockCounter = 0;
		vtkMultiBlockDataSet *currBlock = vtkMultiBlockDataSet::New();
		if(currLayer->getPointPoints()->GetNumberOfPoints() > 0 && this->DrawPoints)
		{
			// Create a new PolyData, to hold our point data
			vtkPolyData *pointObj = vtkPolyData::New();
			pointObj->SetPoints(currLayer->getPointPoints());
			pointObj->SetVerts(currLayer->getPointCells());

			// Add our colour property
			if(currLayer->getPointProps()->GetNumberOfTuples() > 0)
			{
				currLayer->getPointProps()->SetName(PROPERTY_NAME);
				pointObj->GetPointData()->AddArray(currLayer->getPointProps());
			}

			// Add the point data to the MultiBlockDataSet
			currBlock->SetBlock(blockCounter, pointObj);
			currBlock->GetMetaData(blockCounter)->Set(vtkCompositeDataSet::NAME(), POINT_NAME);
			pointObj->Delete();

			blockCounter++;
		}
		if(currLayer->getLinePoints()->GetNumberOfPoints() > 0)
		{
			// Create a new PolyData, to hold our line data
			vtkPolyData *lineObj = vtkPolyData::New();
			lineObj->SetPoints(currLayer->getLinePoints());
			lineObj->SetLines(currLayer->getLineCells());

			// Add our colour property
			if(currLayer->getLineProps()->GetNumberOfTuples() > 0)
			{
				currLayer->getLineProps()->SetName(PROPERTY_NAME);
				lineObj->GetPointData()->AddArray(currLayer->getLineProps());
			}

			// Add the point data to the MultiBlockDataSet
			currBlock->SetBlock(blockCounter, lineObj);
			currBlock->GetMetaData(blockCounter)->Set(vtkCompositeDataSet::NAME(), LINE_NAME);
			lineObj->Delete();

			blockCounter++;
		}
		if(currLayer->getPolyLinePoints()->GetNumberOfPoints() > 0)
		{
			// Create a new PolyData, to hold our line data
			vtkPolyData *polyLineObj = vtkPolyData::New();
			polyLineObj->SetPoints(currLayer->getPolyLinePoints());
			polyLineObj->SetLines(currLayer->getPolyLineCells());

			// Add our colour property
			if(currLayer->getPolyLineProps()->GetNumberOfTuples() > 0)
			{
				currLayer->getPolyLineProps()->SetName(PROPERTY_NAME);
				polyLineObj->GetPointData()->AddArray(currLayer->getPolyLineProps());
			}

			// Add the point data to the MultiBlockDataSet
			currBlock->SetBlock(blockCounter, polyLineObj);
			currBlock->GetMetaData(blockCounter)->Set(vtkCompositeDataSet::NAME(), POLYLINE_NAME);
			polyLineObj->Delete();

			blockCounter++;
		}
		if(currLayer->getLWPolyLinePoints()->GetNumberOfPoints() > 0)
		{
			// Create a new PolyData, to hold our line data
			vtkPolyData *lwPolyLineObj = vtkPolyData::New();
			lwPolyLineObj->SetPoints(currLayer->getLWPolyLinePoints());
			lwPolyLineObj->SetLines(currLayer->getLWPolyLineCells());

			// Add our colour property
			if(currLayer->getLWPolyLineProps()->GetNumberOfTuples() > 0)
			{
				currLayer->getLWPolyLineProps()->SetName(PROPERTY_NAME);
				lwPolyLineObj->GetPointData()->AddArray(currLayer->getLWPolyLineProps());
			}

			//clean it
			vtkCleanPolyData *cleanData = vtkCleanPolyData::New();
			cleanData->SetInput( lwPolyLineObj );						
			cleanData->PointMergingOff();
			cleanData->Update();

			// Add the point data to the MultiBlockDataSet
			currBlock->SetBlock(blockCounter, cleanData->GetOutput());
			currBlock->GetMetaData(blockCounter)->Set(vtkCompositeDataSet::NAME(), LWPOLY_NAME);
			
			cleanData->Delete();
			lwPolyLineObj->Delete();
			
			blockCounter++;
		}
		if(currLayer->getSurfPoints()->GetNumberOfPoints() > 0)
		{
      //3dface
			// Create a new PolyData, to hold our surf data
			vtkPolyData *surfObj = vtkPolyData::New();
			surfObj->SetPoints(currLayer->getSurfPoints());
			surfObj->SetPolys(currLayer->getSurfCells());

			// Add our colour property
			if(currLayer->getSurfProps()->GetNumberOfTuples() > 0)
			{
				currLayer->getSurfProps()->SetName(PROPERTY_NAME);
				surfObj->GetPointData()->AddArray(currLayer->getSurfProps());
			}

			// Clean the object to remove duplicate points
			vtkCleanPolyData *cleanData = vtkCleanPolyData::New();
			cleanData->SetInput(surfObj);
			cleanData->SetAbsoluteTolerance(0);
			// Merging data will cause problems with MultiBlockDataSets
			cleanData->SetConvertLinesToPoints(0);
			cleanData->SetConvertPolysToLines(0);
			cleanData->SetConvertStripsToPolys(0);
			cleanData->SetPieceInvariant(1);
			cleanData->SetPointMerging(1);
			cleanData->Update();

			// Add the point data to the MultiBlockDataSet
      double bounds[6];
      surfObj->GetBounds(bounds);
      //vtkErrorMacro(<< cleanData->GetOutput()->GetBounds());
			currBlock->SetBlock(blockCounter, cleanData->GetOutput());
			currBlock->GetMetaData(blockCounter)->Set(vtkCompositeDataSet::NAME(), SURFACE_NAME);

			surfObj->Delete();
			cleanData->Delete();

			blockCounter++;
		}
		if(currLayer->getText()->GetNumberOfItems() > 0)
		{
			// Create a new AppendPolyData, to hold our text
			vtkAppendPolyData *textObj = vtkAppendPolyData::New();

			vtkCollection *textCollection = currLayer->getText();
			for(int i = 0; i < textCollection->GetNumberOfItems(); i++)
			{
				vtkPolyData *text = vtkPolyData::SafeDownCast(textCollection->GetItemAsObject(i));
				textObj->AddInput( text );//tf->GetPolyDataOutput() );
				if(currLayer->getTextProps()->GetNumberOfTuples() > 0)
				{
					currLayer->getTextProps()->SetName(PROPERTY_NAME);
					text->GetPointData()->AddArray(currLayer->getTextProps());
				}
				// Removing the polydata from the AppendData causes a crash
				//text->Delete();
			}


			textObj->Update();

			currBlock->SetBlock(blockCounter, textObj->GetOutput());
			currBlock->GetMetaData(blockCounter)->Set(vtkCompositeDataSet::NAME(), TEXT_NAME);

			textObj->Delete();

			blockCounter++;
		}
		if(currLayer->getCircles()->GetNumberOfItems() > 0)
		{
			// Create a new PolyData, to hold our circle data
			vtkAppendPolyData *circleObj = vtkAppendPolyData::New();

			vtkCollection *circleCollection = currLayer->getCircles();

			//add inputs
			for(int i = 0; i < circleCollection->GetNumberOfItems(); i++)
			{
				vtkPolyData *circle = vtkPolyData::SafeDownCast(circleCollection->GetItemAsObject(i));
				circleObj->AddInput( circle );
				// Add our colour property
				if(currLayer->getCircleProps()->GetNumberOfTuples() > 0)
				{
					currLayer->getCircleProps()->SetName(PROPERTY_NAME);
					circle->GetPointData()->AddArray(currLayer->getCircleProps());
				}
				// Removing the polydata from the AppendData causes a crash
			}


			circleObj->Update();

			// Add the circle data to the MultiBlockDataSet
			currBlock->SetBlock(blockCounter, circleObj->GetOutput());
			currBlock->GetMetaData(blockCounter)->Set(vtkCompositeDataSet::NAME(), CIRCLE_NAME);

			circleObj->Delete();

			blockCounter++;
		}
		if(currLayer->getArcPoints()->GetNumberOfPoints() > 0)
		{
			// Create a new PolyData, to hold our arc data
			vtkPolyData *arcObj = vtkPolyData::New();
			arcObj->SetPoints(currLayer->getArcPoints());
			arcObj->SetLines(currLayer->getArcCells());

			// Add our colour property
			if(currLayer->getArcProps()->GetNumberOfTuples() > 0)
			{
				currLayer->getArcProps()->SetName(PROPERTY_NAME);
				arcObj->GetPointData()->AddArray(currLayer->getArcProps());
			}

			// Add the arc data to the MultiBlockDataSet
			currBlock->SetBlock(blockCounter, arcObj);
			currBlock->GetMetaData(blockCounter)->Set(vtkCompositeDataSet::NAME(), ARC_NAME);

			arcObj->Delete();

			blockCounter++;
		}
		if(currLayer->getSolidPoints()->GetNumberOfPoints() > 0)
		{
			// Create a new PolyData, to hold our line data
			vtkPolyData *solidObj = vtkPolyData::New();
			solidObj->SetPoints(currLayer->getSolidPoints());
			solidObj->SetPolys(currLayer->getSolidCells());

			// Add our colour property
			if(currLayer->getSolidProps()->GetNumberOfTuples() > 0)
			{
				currLayer->getSolidProps()->SetName(PROPERTY_NAME);
				solidObj->GetPointData()->AddArray(currLayer->getSolidProps());
			}

			// Add the point data to the MultiBlockDataSet
			currBlock->SetBlock(blockCounter, solidObj);
			currBlock->GetMetaData(blockCounter)->Set(vtkCompositeDataSet::NAME(), SOLID_NAME);

			solidObj->Delete();

			blockCounter++;
		}
		// ---- PARSE BLOCK DATA ----
		// If there are blocks in the layer, parse them now
		if(currLayer->getBlockList()->GetNumberOfItems() > 0)
		{
			vtkCollection *blockCollection = currLayer->getBlockList();
			vtkStdString innerBlockName;
			vtkMultiBlockDataSet *innerBlockSet;
			int innerBlockCounter = 0;
			innerBlockSet = vtkMultiBlockDataSet::New();
			// Go through each block in the layer's block list 
			for(int i = 0; i < blockCollection->GetNumberOfItems(); i++)
			{
				innerBlockCounter = 0;
				innerBlockSet = vtkMultiBlockDataSet::New();
				vtkDXFBlock *innerBlock = vtkDXFBlock::SafeDownCast(blockCollection->GetItemAsObject(i));
				innerBlockName = innerBlock->getName();
				//innerBlock->ApplyProperties();

				if(innerBlock->getDrawBlock())
				{

				if(innerBlock->getPointPoints()->GetNumberOfPoints() > 0  && this->DrawPoints)
				{
					// Create a new PolyData, to hold our line data
					vtkPolyData *pointObj = vtkPolyData::New();
					pointObj->SetPoints(innerBlock->getPointPoints());
					pointObj->SetLines(innerBlock->getPointCells());

					// Add our colour property
					if(innerBlock->getPointProps()->GetNumberOfTuples() > 0)
					{
						innerBlock->getPointProps()->SetName(PROPERTY_NAME);
						pointObj->GetPointData()->AddArray(innerBlock->getPointProps());
					}
					vtkTransform *transform = vtkTransform::New();
					vtkTransformFilter *tf = vtkTransformFilter::New();
					transform->Translate(innerBlock->getBlockTransform());
					transform->Scale(innerBlock->getBlockScale());
					   
					tf->SetInput(pointObj);
					tf->SetTransform(transform);

					tf->Update();

					// Add the 3DFace data to the MultiBlockDataSet
					innerBlockSet->SetBlock(innerBlockCounter, tf->GetPolyDataOutput());
					innerBlockSet->GetMetaData(innerBlockCounter)->Set(vtkCompositeDataSet::NAME(), POINT_NAME);

					tf->Delete();
					transform->Delete();
					pointObj->Delete();

					innerBlockCounter++;
				}
				if(innerBlock->getLinePoints()->GetNumberOfPoints() > 0)
				{
					// Create a new PolyData, to hold our line data
					vtkPolyData *lineObj = vtkPolyData::New();
					lineObj->SetPoints(innerBlock->getLinePoints());
					lineObj->SetLines(innerBlock->getLineCells());
					// Add our colour property
					if(innerBlock->getLineProps()->GetNumberOfTuples() > 0)
					{
						innerBlock->getLineProps()->SetName(PROPERTY_NAME);
						lineObj->GetPointData()->AddArray(innerBlock->getLineProps());
					}

					vtkTransform *transform = vtkTransform::New();
					vtkTransformFilter *tf = vtkTransformFilter::New();
					transform->Translate(innerBlock->getBlockTransform());
					transform->Scale(innerBlock->getBlockScale());
					   
					tf->SetInput(lineObj);
					tf->SetTransform(transform);

					tf->Update();

					// Add the line data to the MultiBlockDataSet
					innerBlockSet->SetBlock(innerBlockCounter, tf->GetPolyDataOutput());
					innerBlockSet->GetMetaData(innerBlockCounter)->Set(vtkCompositeDataSet::NAME(), LINE_NAME);
					innerBlockCounter++;

					tf->Delete();
					transform->Delete();
					lineObj->Delete();
				}
				if(innerBlock->getPolyLinePoints()->GetNumberOfPoints() > 0)
				{
					// Create a new PolyData, to hold our line data
					vtkPolyData *polyLineObj = vtkPolyData::New();
					polyLineObj->SetPoints(innerBlock->getPolyLinePoints());
					polyLineObj->SetLines(innerBlock->getPolyLineCells());
					// Add our colour property
					if(innerBlock->getPolyLineProps()->GetNumberOfTuples() > 0)
					{
						innerBlock->getPolyLineProps()->SetName(PROPERTY_NAME);
						polyLineObj->GetPointData()->AddArray(innerBlock->getPolyLineProps());
					}
					vtkTransform *transform = vtkTransform::New();
					vtkTransformFilter *tf = vtkTransformFilter::New();
					transform->Translate(innerBlock->getBlockTransform());
					transform->Scale(innerBlock->getBlockScale());
					   
					tf->SetInput(polyLineObj);
					tf->SetTransform(transform);

					tf->Update();

					// Add the line data to the MultiBlockDataSet
					innerBlockSet->SetBlock(innerBlockCounter, tf->GetPolyDataOutput());
					innerBlockSet->GetMetaData(innerBlockCounter)->Set(vtkCompositeDataSet::NAME(), POLYLINE_NAME);
					innerBlockCounter++;

					tf->Delete();
					transform->Delete();
					polyLineObj->Delete();
				}
				if(innerBlock->getLWPolyLinePoints()->GetNumberOfPoints() > 0)
				{
					// Create a new PolyData, to hold our line data
					vtkPolyData *lwPolyLineObj = vtkPolyData::New();
					lwPolyLineObj->SetPoints(innerBlock->getLWPolyLinePoints());
					lwPolyLineObj->SetLines(innerBlock->getLWPolyLineCells());

					// Add our colour property
					if(innerBlock->getLWPolyLineProps()->GetNumberOfTuples() > 0)
					{
						innerBlock->getLWPolyLineProps()->SetName(PROPERTY_NAME);
						lwPolyLineObj->GetPointData()->AddArray(innerBlock->getLWPolyLineProps());
					}
					vtkTransform *transform = vtkTransform::New();
					vtkTransformFilter *tf = vtkTransformFilter::New();
					transform->Translate(innerBlock->getBlockTransform());
					transform->Scale(innerBlock->getBlockScale());
					   
					tf->SetInput(lwPolyLineObj);
					tf->SetTransform(transform);

					tf->Update();

					// Add the lwPolyLine data to the MultiBlockDataSet
					innerBlockSet->SetBlock(innerBlockCounter, tf->GetPolyDataOutput());
					innerBlockSet->GetMetaData(innerBlockCounter)->Set(vtkCompositeDataSet::NAME(), LWPOLY_NAME);
					innerBlockCounter++;

					tf->Delete();
					transform->Delete();
					lwPolyLineObj->Delete();
				}
				if(innerBlock->getSurfPoints()->GetNumberOfPoints() > 0)
				{
					// Create a new PolyData, to hold our line data
					vtkPolyData *surfObj = vtkPolyData::New();
					surfObj->SetPoints(innerBlock->getSurfPoints());
					surfObj->SetLines(innerBlock->getSurfCells());

					// Add our colour property
					if(innerBlock->getSurfProps()->GetNumberOfTuples() > 0)
					{
						innerBlock->getSurfProps()->SetName(PROPERTY_NAME);
						surfObj->GetPointData()->AddArray(innerBlock->getSurfProps());
					}
					vtkTransform *transform = vtkTransform::New();
					vtkTransformFilter *tf = vtkTransformFilter::New();
					transform->Translate(innerBlock->getBlockTransform());
					transform->Scale(innerBlock->getBlockScale());
					   
					tf->SetInput(surfObj);
					tf->SetTransform(transform);

					tf->Update();

					// Add the 3DFace data to the MultiBlockDataSet
					innerBlockSet->SetBlock(innerBlockCounter, tf->GetPolyDataOutput());
					innerBlockSet->GetMetaData(innerBlockCounter)->Set(vtkCompositeDataSet::NAME(), SURFACE_NAME);
					innerBlockCounter++;

					tf->Delete();
					transform->Delete();
					surfObj->Delete();
				}
				if(innerBlock->getText()->GetNumberOfItems() > 0)
				{
					// Create a new AppendPolyData, to hold our text
					vtkAppendPolyData *textObj = vtkAppendPolyData::New();

					vtkCollection *textCollection = innerBlock->getText();
					for(int i = 0; i < textCollection->GetNumberOfItems(); i++)
					{
						vtkPolyData *text = vtkPolyData::SafeDownCast(textCollection->GetItemAsObject(i));
						vtkTransform *transform = vtkTransform::New();
						vtkTransformFilter *tf = vtkTransformFilter::New();
						transform->Translate(innerBlock->getBlockTransform());
						transform->Scale(innerBlock->getBlockScale());
						   
						tf->SetInput(text);
						tf->SetTransform(transform);

						tf->Update();

						textObj->AddInput( tf->GetPolyDataOutput() );//tf->GetPolyDataOutput() );
						if(innerBlock->getTextProps()->GetNumberOfTuples() > 0)
						{
							innerBlock->getTextProps()->SetName(PROPERTY_NAME);
							tf->GetPolyDataOutput()->GetPointData()->AddArray(innerBlock->getTextProps());
						}
						
						tf->Delete();
						transform->Delete();
					}

					textObj->Update();

					// Add the text data to the MultiBlockDataSet
					innerBlockSet->SetBlock(innerBlockCounter, textObj->GetOutput());
					innerBlockSet->GetMetaData(innerBlockCounter)->Set(vtkCompositeDataSet::NAME(), TEXT_NAME);
					innerBlockCounter++;
					
					textObj->Delete();
				}
				if(innerBlock->getCircles()->GetNumberOfItems() > 0)
				{
					// Create a new PolyData, to hold our circle data
					vtkAppendPolyData *circleObj = vtkAppendPolyData::New();

					vtkCollection *circleCollection = innerBlock->getCircles();

					//add inputs
					for(int i = 0; i < circleCollection->GetNumberOfItems(); i++)
					{
						vtkPolyData *circle = vtkPolyData::SafeDownCast(circleCollection->GetItemAsObject(i));
						vtkTransform *transform = vtkTransform::New();
						vtkTransformFilter *tf = vtkTransformFilter::New();
						transform->Translate(innerBlock->getBlockTransform());
						transform->Scale(innerBlock->getBlockScale());
						   
						tf->SetInput(circle);
						tf->SetTransform(transform);

						tf->Update();

						circleObj->AddInput( tf->GetPolyDataOutput() );
						// Add our colour property
						if(innerBlock->getCircleProps()->GetNumberOfTuples() > 0)
						{
							innerBlock->getCircleProps()->SetName(PROPERTY_NAME);
							tf->GetPolyDataOutput()->GetPointData()->AddArray(innerBlock->getCircleProps());
						}
						
						//rmaynard
						tf->Delete();
					  transform->Delete();
					  circle->Delete();						
					}
					
					circleObj->Update();

					// Add the circle data to the MultiBlockDataSet
					innerBlockSet->SetBlock(innerBlockCounter, circleObj->GetOutput());
					innerBlockSet->GetMetaData(innerBlockCounter)->Set(vtkCompositeDataSet::NAME(), CIRCLE_NAME);

					innerBlockCounter++;
					circleObj->Delete();
				}
				if(innerBlock->getArcPoints()->GetNumberOfPoints() > 0)
				{
					// Create a new PolyData, to hold our arc data
					vtkPolyData *arcObj = vtkPolyData::New();
					arcObj->SetPoints(innerBlock->getArcPoints());
					arcObj->SetLines(innerBlock->getArcCells());

					// Add our colour property
					if(innerBlock->getArcProps()->GetNumberOfTuples() > 0)
					{
						innerBlock->getArcProps()->SetName(PROPERTY_NAME);
						arcObj->GetPointData()->AddArray(innerBlock->getArcProps());
					}

					vtkTransform *transform = vtkTransform::New();
					vtkTransformFilter *tf = vtkTransformFilter::New();
					transform->Translate(innerBlock->getBlockTransform());
					transform->Scale(innerBlock->getBlockScale());
					   
					tf->SetInput(arcObj);
					tf->SetTransform(transform);

					tf->Update();

					// Add the arc data to the MultiBlockDataSet
					innerBlockSet->SetBlock(innerBlockCounter, tf->GetPolyDataOutput());
					innerBlockSet->GetMetaData(innerBlockCounter)->Set(vtkCompositeDataSet::NAME(), ARC_NAME);
					innerBlockCounter++;

					tf->Delete();
					transform->Delete();
					arcObj->Delete();
				}
				if(innerBlock->getSolidPoints()->GetNumberOfPoints() > 0)
				{
					// Create a new PolyData, to hold our line data
					vtkPolyData *solidObj = vtkPolyData::New();
					solidObj->SetPoints(innerBlock->getSolidPoints());
					solidObj->SetPolys(innerBlock->getSolidCells());

					// Add our colour property
					if(innerBlock->getSolidProps()->GetNumberOfTuples() > 0)
					{
						innerBlock->getSolidProps()->SetName(PROPERTY_NAME);
						solidObj->GetPointData()->AddArray(innerBlock->getSolidProps());
					}

					vtkTransform *transform = vtkTransform::New();
					vtkTransformFilter *tf = vtkTransformFilter::New();
					transform->Translate(innerBlock->getBlockTransform());
					transform->Scale(innerBlock->getBlockScale());
					   
					tf->SetInput(solidObj);
					tf->SetTransform(transform);

					tf->Update();

					// Add the solid data to the MultiBlockDataSet
					innerBlockSet->SetBlock(innerBlockCounter, tf->GetPolyDataOutput());
					innerBlockSet->GetMetaData(innerBlockCounter)->Set(vtkCompositeDataSet::NAME(), SOLID_NAME);
					innerBlockCounter++;

					tf->Delete();
					transform->Delete();
					solidObj->Delete();
				}
				// Add the line data to the inner MultiBlockDataSet
				currBlock->SetBlock(blockCounter, innerBlockSet);
				currBlock->GetMetaData(blockCounter)->Set(vtkCompositeDataSet::NAME(), innerBlockName);
				blockCounter++;
        innerBlockSet->Delete();
			}
			}
		}
		//need to confirm that the block actually has something before we add it!
		if ( blockCounter != 0 )
			{
			vtkIdType size = output->GetNumberOfBlocks();
			output->SetBlock(size, currBlock);
			output->GetMetaData(size)->Set(vtkCompositeDataSet::NAME(), currLayer->getName());
			}

		currBlock->Delete();
		
	}
	
	// All done :)
	
	layerList->Delete();	

	return 1;
}

vtkStdString vtkDXFObject::GetObjectName()
{
	vtkStdString fName = this->FileName;

	//Find position final '\' that occurs just before the file name
	int slashPosition = fName.find_last_of('\\');
	//sometimes path contains the other slash ('/')
	if(slashPosition == -1)
		slashPosition = fName.find_last_of('/');
	//Add one to slashPosition so that the slash is not included
	slashPosition = slashPosition+1;

	//Find position of '.' that occurs before the file extension
	int dotPosition = fName.find_last_of('.');
	//Save the file extention into a stdString, so that the length that be found
	vtkStdString extension = fName.substr(dotPosition);
	int extLen = extension.length();

	//Save the file name AND the file extention into a stdString, so that the length that be found
	vtkStdString nameWithExt = fName.substr(slashPosition);
	int nameExtLen = nameWithExt.length();

	//Determine the length of the word, so that it can be taken from fName, which has the file name and extension
	int finalNameLength = nameExtLen - extLen;

	//Pull the file name out of the full path (fName)
	vtkStdString newName = fName.substr(slashPosition, finalNameLength);

	return newName;
}
