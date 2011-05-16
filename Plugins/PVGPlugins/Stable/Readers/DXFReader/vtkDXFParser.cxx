// By: Eric Daoust && Matthew Livingstone
#include "vtkDXFParser.h"
#include <vtksys/ios/sstream>
#include "vtkObjectFactory.h"
#include "vtkPolyData.h"
#include "vtkTriangle.h"
#include "vtkCollection.h"
#include "vtkVectorText.h"
#include "vtkTransform.h"
#include "vtkTransformFilter.h"
#include "vtkDiskSource.h"
#include "vtkDXFLayer.h"
#include "vtkDXFBlock.h"
#include "vtkPointData.h"
#include "vtkDXFObjectMap.h"
#include <math.h>

#define PI 3.141592653

vtkCxxRevisionMacro(vtkDXFParser, "$Revision: 1 $");
vtkStandardNewMacro(vtkDXFParser);

vtkDXFParser::vtkDXFParser()
{

  this->layerExists = 0; //CCAMERON
	
	// If 0.0, text that does not set a scale will disappear
	this->scale = 1.0;
	this->currElevation = 0.0;
	// If 0.0, objects that do not set their own scale will disappear
	this->LineTypeScale = 1.0;
  // CCAMERON: THESE MUST BE INITIALIZED HERE
  this->XScale = 1.0;
  this->YScale = 1.0;
  this->ZScale = 1.0;
  // CCAMERON

	this->ExtMinX = 0.0;
	this->ExtMaxX = 0.0;
	this->PExtMinX = 0.0;
	this->PExtMaxX = 0.0;
	this->ExtMinY = 0.0;
	this->ExtMaxY = 0.0;
	this->PExtMinY = 0.0;
	this->PExtMaxY = 0.0;
	this->ExtMinZ = 0.0;
	this->ExtMaxZ = 0.0;
	this->PExtMinZ = 0.0;
	this->PExtMaxZ = 0.0;
	this->XAdj = 0.0;
	this->YAdj = 0.0;
  // Used for viewport information
	this->ViewX = 0.0;
	this->ViewY = 0.0;
	this->ViewZoom = 0.0;

  this->blockRecordExists = false;

  this->CommandLine = 0;

	this->blockList = vtkDXFObjectMap::New();

	

  
}

vtkDXFParser::~vtkDXFParser()
{
  this->blockList->Delete();
}

char* vtkDXFParser::TrimWhiteSpace(vtkStdString input)
{
	unsigned int i = 0;
	int size = 0;
	while(i < input.size())
	{	
		if(input.at(i) >= '0' && input.at(i) <= '9') //digit
			size++;
		i++;
	}
	//make room for null termination by incrementing "i" and "size"
	size++; 
	i++;

	char* output = new char[size];
	int outputIndex = 0;

	for(unsigned int x = 0; x < i; x++)
	{
		if(input.at(x) >= '0' && input.at(x) <= '9') //digit
		{
			while( x < input.size() && ((input.at(x) >= '0' && input.at(x) <= '9') || input.at(x) == '.') ) //account for possible decimal character
			{
				output[outputIndex] = input.at(x);
				outputIndex++;
				x++;
			}
			output[outputIndex] = '\0';
		}
	}
	return output;
}

int vtkDXFParser::getIntFromLine(const char* line)
{
	char* trimLine = this->TrimWhiteSpace(line);
	vtksys_ios::istringstream iss(trimLine);
	int outputInt;
	iss >> outputInt;
	delete[] trimLine;
	return outputInt;
	
}

void vtkDXFParser::Read(vtksys_ios::ifstream* file)
{
	// The file is read two lines at a time
	// command line is used to determine current mode (create new, add, etc.)
	// value line is corresponding data (Coordinate, etc.)
	vtkStdString text1;
	std::getline(*file, text1);
	this->CommandLine = this->getIntFromLine(text1);
	std::getline(*file, this->ValueLine);	 
}

void vtkDXFParser::ParseHeader(ifstream* file)
{
	this->Read(file);
	while(this->ValueLine != "ENDSEC")
	{
		if(this->CommandLine == 9 && this->ValueLine == "$LTSCALE")
		{
			this->Read(file);
			this->LineTypeScale = this->GetLine();
		}
		else if(this->CommandLine == 9 && this->ValueLine == "$LIMMIN")
		{
			this->Read(file);
			this->ExtMinX = this->GetLine();
			this->Read(file);
			this->ExtMinY = this->GetLine();
			//this->Read(file);
			//this->ExtMinZ = this->GetLine();

			if(this->ExtMinX < 0)
			{
				this->XAdj = abs(this->ExtMinX);
			}
			if(this->ExtMinY < 0)
			{
				this->YAdj = abs(this->ExtMinY);
			}
		}
		else if(this->CommandLine == 9 && this->ValueLine == "$LIMMAX")
		{
			this->Read(file);
			this->ExtMaxX = this->GetLine();
			this->Read(file);
			this->ExtMaxY = this->GetLine();
			//this->Read(file);
			//this->ExtMaxZ = this->GetLine();
		}
		else if(this->CommandLine == 9 && this->ValueLine == "$PLIMMIN")
		{
			this->Read(file);
			this->PExtMinX = this->GetLine();
			this->Read(file);
			this->PExtMinY = this->GetLine();
			//this->Read(file);
			//this->PExtMinZ = this->GetLine();
		}
		else if(this->CommandLine == 9 && this->ValueLine == "$PLIMMAX")
		{
			this->Read(file);
			this->PExtMaxX = this->GetLine();
			this->Read(file);
			this->PExtMaxY = this->GetLine();
			//this->Read(file);
			//this->PExtMaxZ = this->GetLine();
		}
		else if(this->CommandLine == 9 && this->ValueLine == "$EXTMIN")
		{
			this->Read(file);
			this->ViewX = this->GetLine();
			this->Read(file);
			this->ViewY = this->GetLine();

			if(this->ViewX < 0.0)
			{
				this->ViewX = abs(this->ViewX);
			}
			else
			{
				this->ViewX = 0.0;
			}
			if(this->ViewY < 0.0)
			{
				this->ViewY = abs(this->ViewY);
			}
			else
			{
				this->ViewY = 0.0;
			}
		}
		this->Read(file);
	}
	/*double PExtX = this->PExtMaxX - this->PExtMinX;
	double PExtY = this->PExtMaxY - this->PExtMinY;
	//double PExtZ = this->PExtMaxZ - this->PExtMinZ;

	double ExtX = this->ExtMaxX - this->ExtMinX;
	double ExtY = this->ExtMaxY - this->ExtMinY;
	//double ExtZ = this->ExtMaxZ - this->ExtMinZ;*/

	double PExtX = this->PExtMaxX - 0;
	double PExtY = this->PExtMaxY - 0;
	double PExtZ = this->PExtMaxZ - 0;

	double ExtX = this->ExtMaxX - 0;
	double ExtY = this->ExtMaxY - 0;
	double ExtZ = this->ExtMaxZ - 0;

	this->XScale = (PExtX/ExtX);
	this->YScale = (PExtY/ExtY);

	this->XAdj = this->ViewX;
	this->YAdj = this->ViewY;
	
	this->ZScale = 1.0;
	// Ensure that the scale is atleast 1
	if(this->XScale >= 10000 || this->XScale <= -10000 || ExtX == 0.0)
	{
		this->XScale = 1.0;
	}
	if(this->YScale >= 10000 || this->YScale <= -10000 || ExtY == 0.0)
	{
		this->YScale = 1.0;
	}
	if(this->ZScale == 0.0)
	{
		this->ZScale = 1.0;
	}
}

void vtkDXFParser::ParseViewPort(ifstream* file)
{
	bool activeView = false;
	// May need to place a check for the active viewport (AutoCAD can have multiple
	// viewports)
	// Active viewport has CommandLine 2 and ValueLine "*ACTIVE"
	this->Read(file);
	while(this->CommandLine != 0)
	{
		if(this->CommandLine == 2 && this->ValueLine == "*ACTIVE")
		{
			activeView = true;
		}
		else if(this->CommandLine == 12 && activeView)
		{
			this->ViewX = this->GetLine();
		}
		else if(this->CommandLine == 22 && activeView)
		{
			this->ViewY = this->GetLine();
		}
		else if(this->CommandLine == 45 && activeView)
		{
			this->ViewZoom = this->GetLine();
		}
		this->Read(file);
	}
}

vtkDXFObjectMap* vtkDXFParser::ParseData(const char* name, bool DrawHidden, bool AutoScale)
{
	ifstream file;
	file.open(name, ios::in);
	vtkDXFObjectMap *layerList = vtkDXFObjectMap::New();
		
	// Used for dealing with BLOCK data in ParseData
	vtkStdString blockName;
	vtkStdString layerName;
	bool foundBlock = false;

	vtkStdString currentLayerName;

	this->layerExists = 0;
	bool firstPoint = true;

	// Parse to get VIEWPORT data. Used for initial positions and
	// zoom information.
	this->Read(&file);

	while(this->ValueLine != "EOF")
	{
		if(this->CommandLine == 0 && this->ValueLine == "VIEWPORT")
		{
			this->ParseViewPort(&file);
		}
		else if(this->CommandLine == 0 && this->ValueLine == "VPORT")
		{
			this->ParseViewPort(&file);
		}
		else
		{
			this->Read(&file);
		}
	}

	file.close();
	file.clear();

	// Second read for other data
	file.open(name, ios::in);

	this->Read(&file);

	while(this->ValueLine != "EOF")
	{
		if(this->CommandLine == 2 && this->ValueLine == "HEADER")
		{
			this->ParseHeader(&file);
		}
		if(this->CommandLine == 2 && this->ValueLine == "LAYER")
		{
			//we are now in the section where we define layers (a.k.a color table)
			this->layerExists = 1;
			this->ParseColorTable(&file, layerList, DrawHidden);			
		}
		else if(this->CommandLine == 2 && this->ValueLine == "BLOCK_RECORD")
		{
			// Block Records are used to create the initial block list, which all block methods
			// will search for to find specific blocks to add data to.
			this->ParseBlockRecords(&file);
		}
		else if( this->CommandLine == 2 && this->ValueLine == "ENTITIES")
		{
			//these are the entities that will be assigned to the created layers (if any exist)
			if(this->layerExists == 0)
			{	//sometimes a DXF file does not have layers, so in this case we create a default layer with value 0
				vtkDXFBlock *layer = vtkDXFBlock::New();
				layer->setName("DEFAULT");
				layer->setLayerPropertyValue(0);
				layer->setDrawHidden(DrawHidden);
				layerList->AddItem(layer->getName(), layer);
				layer->Delete();
				this->layerExists = 1;
			}
			while(this->ValueLine != "ENDSEC")
			{
				if(this->ValueLine == "LINE")
				{
					//this creates a line cell
					this->ParseLine(&file, &currentLayerName, layerList, AutoScale);
				}
				else if(this->ValueLine == "POINT")
				{
					//this creates many point cells
					this->ParsePoints(&file, &currentLayerName, layerList, AutoScale);
				}
				else if(this->ValueLine == "POLYLINE")
				{
					//this creates many poly lines (with an additional line if it is a closed poly line)
					this->ParsePolyLine(&file, &currentLayerName, layerList, firstPoint, AutoScale);
				}
				else if(this->ValueLine == "LWPOLYLINE")
				{
					//this creates many lines having common Z coordinates, and all lines are closed lines
					this->ParseLWPolyLine(&file, &currentLayerName, layerList, AutoScale);
				}
				else if(this->ValueLine == "3DFACE")
				{
					//this creates many triangle cells
					this->ParseFace(&file, &currentLayerName, layerList, AutoScale);
				}
				else if(this->ValueLine == "TEXT" || this->ValueLine == "ATTRIB")
				{				  
					// Creates a collection of vector text objects
					this->ParseText(&file, &currentLayerName, layerList, false, AutoScale);
				}
				else if(this->ValueLine == "MTEXT")
				{
					// Same as TEXT, with different scaling/transformations
					this->ParseText(&file, &currentLayerName, layerList, true, AutoScale);
				}
				else if(this->ValueLine == "CIRCLE")
				{
					//this creates a circle
					this->ParseCircle(&file, &currentLayerName, layerList, AutoScale);
				}
				else if(this->ValueLine == "ARC")
				{
					//this creates an arc
					this->ParseArc(&file, &currentLayerName, layerList, AutoScale);
				}
				else if(this->ValueLine == "SOLID")
				{
					//this creates a polygon (3 or 4 verts)
					this->ParseSolid(&file, &currentLayerName, layerList, AutoScale);
				}
				else if(this->CommandLine == 0 && this->ValueLine == "INSERT")
				{
					// INSERT commands insert blocks at given positions, with given scales
					this->ParseInsert(&file, &currentLayerName, layerList, AutoScale);
				}
				else if(this->CommandLine == 0 && this->ValueLine == "DIMENSION")
				{
					// I'm treating it like INSERT, but with NO transform
					this->ParseDimension(&file, &currentLayerName, layerList, AutoScale);
				}
				else
				{
					this->Read(&file);
				}
			}
		}
		else if(this->CommandLine == 0 && this->ValueLine == "BLOCK" && this->blockRecordExists == true)
		{
		  vtkDXFBlock *block;
			foundBlock = false;
			while(this->ValueLine != "ENDBLK")
			{
				if(this->CommandLine == 8)
				{
					layerName = this->ValueLine;
					vtkDXFLayer* layer = this->GetLayer(layerName, layerList);
					//if layer not found, then we have a file with no layers defined, so use the default one
					if(layer == NULL)
					{
						vtkErrorMacro("BLOCK found without LAYER reference. You cannot have a BLOCK outside a layer.");
						// vtkDXFObject will handle the NULL returned
						return NULL;
					}
					if( (layer->getLayerPropertyValue() < 0 || layer->getFreezeValue() == 1) && !DrawHidden)
					{
						// Negative colour means hidden block
						this->CommandLine = 0;
						this->ValueLine = "";
					}
					this->Read(&file);
				}
				else if(this->CommandLine == 2 && foundBlock == false && blockName != this->ValueLine)
				{
					// Ensure that we do not try to look for another block,
					// there are other CommandLine = 2 entries inside of the block
					foundBlock = true;
					blockName = this->ValueLine;
					block = this->GetBlock(blockName, blockList);

					if(block == NULL)
					{
						vtkErrorMacro("BLOCK name does not match any entries in BLOCK listing.");
						// vtkDXFObject will handle the NULL returned				
						return NULL;
					}					
				}
				else if(this->ValueLine == "LINE")
				{
					//this creates a line cell
					this->ParseLineBlock(&file, &currentLayerName, layerList, block, AutoScale);
				}
				else if(this->ValueLine == "POINT")
				{
					//this creates many point cells
					this->ParsePointsBlock(&file, &currentLayerName, layerList, block, AutoScale);
				}
				else if(this->ValueLine == "POLYLINE")
				{
					//this creates many poly lines (with an additional line if it is a closed poly line)
					this->ParsePolyLineBlock(&file, &currentLayerName, layerList, firstPoint, block, AutoScale);
				}
				else if(this->ValueLine == "LWPOLYLINE")
				{
					//this creates many lines having common Z coordinates, and all lines are closed lines
					this->ParseLWPolyLineBlock(&file, &currentLayerName, layerList, block, AutoScale);
				}
				else if(this->ValueLine == "3DFACE")
				{
					//this creates many triangle cells
					this->ParseFaceBlock(&file, &currentLayerName, layerList, block, AutoScale);
				}
				else if(this->ValueLine == "TEXT" || this->ValueLine == "ATTRIB")
				{
					// Creates a collection of vector text objects
					this->ParseTextBlock(&file, &currentLayerName, layerList, false, block, AutoScale);
				}
				else if(this->ValueLine == "MTEXT")
				{
					// Same as TEXT, with different scaling/transformations
					this->ParseTextBlock(&file, &currentLayerName, layerList, true, block, AutoScale);
				}
				else if(this->ValueLine == "CIRCLE")
				{
					//this creates a circle
					this->ParseCircleBlock(&file, &currentLayerName, layerList, block, AutoScale);
				}
				else if(this->ValueLine == "ARC")
				{
					//this creates an arc
					this->ParseArcBlock(&file, &currentLayerName, layerList, block, AutoScale);
				}
				else if(this->ValueLine == "SOLID")
				{
					//this creates a polygon (3 or 4 verts)
					this->ParseSolidBlock(&file, &currentLayerName, layerList, block, AutoScale);
				}
				else
				{
					this->Read(&file);
				}
			}			
		}
		else
		{
			this->Read(&file);
		}
	}
	file.close();

	return layerList;
}

// ParseColorTable occurs near the beginning of the DXF file.
// It's purpose is to find the names and properties from all the layer
// that are declared, and place them into a layer list (collection) for
// use/modification later
void vtkDXFParser::ParseColorTable(ifstream* file, vtkDXFObjectMap* layerList, bool DrawHidden)
{
	vtkDXFBlock* currentLayer;
	vtkStdString currentName = "";
	// Used to ensure that we only check for frozen layers once we have found one
	bool checkFrozen = false;

	this->Read(file);

	while(this->ValueLine != "ENDTAB")
	{
		if(this->CommandLine == 2)
		{
			//define new layer to be added to list
			currentName = this->ValueLine;
			currentLayer = vtkDXFBlock::New();
			currentLayer->setName(currentName);
			currentLayer->setDrawHidden(DrawHidden);
			layerList->AddItem(currentLayer->getName(), currentLayer);
			currentLayer->Delete();
			// Ensure that we only check for frozen layers once we have found one
			checkFrozen = true;
		}
		else if(this->CommandLine == 62)
		{
			//define associated color value to go with layer previously defined
			double value = this->GetLine();
			currentLayer->setLayerPropertyValue(value);
		}
		else if(this->CommandLine == 70 && checkFrozen == true)
		{
			double value = this->GetLine();
			if(value < 3 && value > -1)
			{
				currentLayer->setFreezeValue(value);
			}
		}
		this->Read(file);
	}
}

// ParseBlockRecords occurs near the beginning of the DXF file as well.
// It's purpose is to find the names and properties from all the blocks
// that are declared, and place them into a block list (collection) for
// use/modification later
void vtkDXFParser::ParseBlockRecords(ifstream* file)
{
	this->blockRecordExists = true;
	vtkDXFBlock* currentBlock;
	vtkStdString currentName = "";
	this->Read(file);
	while(this->ValueLine != "ENDTAB")
	{
		if(this->CommandLine == 2)
		{
			//define new block to be added to list
			currentName = this->ValueLine;
			currentBlock = vtkDXFBlock::New();
			currentBlock->setName(currentName);
			this->blockList->AddItem(currentName, currentBlock);
			currentBlock->Delete();
		}
		this->Read(file);
	}
}

// Grab the layer with the given name from the layer list.
vtkDXFLayer* vtkDXFParser::GetLayer(vtkStdString name, vtkDXFObjectMap* layerList)
{	
	int size = layerList->GetNumberOfItems();
	//if there is only one then the first one is returned immeadiatly
	if ( 1 ==  size )
		{
		return (vtkDXFLayer*)layerList->GetItemAsObject(0);
		}
	//find the layer with the name as the key
	if(layerList->GetItemAsObject(name))
		{
		return (vtkDXFLayer*)layerList->GetItemAsObject(name);
		}
	//not found
	return NULL;
}

// Grab the block from the given name from the given block list
// (each layer has it's own block list)
vtkDXFBlock* vtkDXFParser::GetBlock(vtkStdString name, vtkDXFObjectMap* bl)
{	
	int size = bl->GetNumberOfItems();
	//if there is only one then the first one is returned immeadiatly
	if ( 1 ==  size )
		{
		return bl->GetItemAsObject(0);
		}
	//find the block with the name as the key
	if(bl->GetItemAsObject(name))
		{
		return bl->GetItemAsObject(name);
		}
	//not found
	return NULL;
}

// Used to push the string value from the file into a double
double vtkDXFParser::GetLine()
{
	double value;
	this->textString << this->ValueLine;
	this->textString >> value;

	this->textString.clear();
	return value;
}

void vtkDXFParser::ParsePolyLine(ifstream* file, vtkStdString* name, vtkDXFObjectMap* layerList, bool firstPoint, bool AutoScale)
{
	this->Read(file);
	bool closedPoly = false;
	this->currElevation = 0.0;
	double* xyz = new double[3];
	xyz[0] = xyz[1] = xyz[2] = 0.0;
	bool layerFound = false;
	bool inModelSpace = true;
	double layerProp = 0;
	// Variables used to keep track of polyFace data
	bool skipVertex = false;
	bool polyFace = false;
	int vertCount = 6;
	int currVertNum = 0;
	bool vertSet = false;
	
	vtkDXFLayer* layer;
	int startPointId;
	int pointId;
	while(this->ValueLine != "SEQEND")
	{
		// Add colour properties to the property array
		if(this->CommandLine == 8 && !layerFound)
		{
			*name = this->ValueLine;
			layer = this->GetLayer(*name, layerList);
			//if layer not found, then we have a file with no layers defined, so use the default one
			if(layer == NULL)
			{
				layer = this->GetLayer("DEFAULT", layerList);				
			}
			
			layerProp = layer->getLayerPropertyValue();
			startPointId = layer->getPolyLinePoints()->GetNumberOfPoints();
			pointId = layer->getPolyLinePoints()->GetNumberOfPoints();

			//only define the layer once, as it is shared by all the objects in this section
			layerFound = true;

			// If the colour value is negative, the layer is invisible
			if( (layer->getLayerPropertyValue() < 0 || layer->getFreezeValue() == 1) && !layer->getDrawHidden())
			{
				return;
			}
		}
		else if(this->CommandLine == 67)
		{
			inModelSpace = false;
		}
		else if(this->CommandLine == 70)
		{
			vtkStdString polyVal = this->TrimWhiteSpace(this->ValueLine);
			if(polyVal == "1")
			{
				//this defines a closed poly line, the first point will have to be duplicated at the end of the list
				closedPoly = true;
			}
			else if(polyVal == "64")
			{
				polyFace = true;
				skipVertex = true;
				firstPoint = false;
			}
			else if(polyVal == "128")
			{
				skipVertex = true;
				firstPoint = false;
			}
		}
		else if(this->CommandLine == 71)
		{
			vertCount = (int)this->GetLine();
			// Added one extra end (6 points) as this count seems to leave them out.
			vertCount += 6;
		}
		// X Coordinate
		else if(this->CommandLine == 10 || this->CommandLine == 11)
		{
			xyz[0] = this->GetLine();
			currVertNum++;
			vertSet = true;
		}
		// Y Coordinate
		else if(this->CommandLine == 20 || this->CommandLine == 21)
		{
			xyz[1] = this->GetLine();
		}
		// Z Coordinate
		else if(this->CommandLine == 30 || this->CommandLine == 31)
		{
			if(firstPoint == false)
			{
				xyz[2] = this->GetLine();
				// Check to see if the point needs the elevation applied to it
				if(xyz[2] != this->currElevation)
				{
					xyz[2] += this->currElevation;
				}
			}
			else
			{
				//firstPoint = false;
				this->currElevation = this->GetLine();
			}
		}
		// Colour
		else if(this->CommandLine == 62)
		{
			layerProp = this->GetLine();
		}
		else if(this->CommandLine == 0)
		{
			if((currVertNum <= vertCount || vertCount == 6) && !skipVertex)
			{
				if(vertSet && firstPoint == false)
				{
				// Check to see if the object is in Model Space and needs to be scaled
				if(inModelSpace && AutoScale)
				{
					xyz[0] += this->XAdj;
					xyz[1] += this->YAdj;

					xyz[0] *= this->XScale;
					xyz[1] *= this->YScale;
					xyz[2] *= this->ZScale;
				}

				layer->getPolyLinePoints()->InsertPoint(pointId, xyz);
				layer->getPolyLineProps()->InsertNextTuple1(layerProp);
				pointId++;
				vertSet = false;
				}
				else
				{
					if(firstPoint)
					{
						firstPoint = false;
					}
				}
			}
			else
			{
				if(skipVertex)
				{
					skipVertex = false;
				}
			}
		}
		this->Read(file);
	}
	if((currVertNum <= vertCount || vertCount == 6) && !skipVertex)
	{
		if(vertSet)
		{
		// Check to see if the object is in Model Space and needs to be scaled
		if(inModelSpace && AutoScale)
		{
			xyz[0] += this->XAdj;
			xyz[1] += this->YAdj;

			xyz[0] *= this->XScale;
			xyz[1] *= this->YScale;
			xyz[2] *= this->ZScale;
		}

		layer->getPolyLinePoints()->InsertPoint(pointId, xyz);
		layer->getPolyLineProps()->InsertNextTuple1(layerProp);
		pointId++;
		}
	}

	// Add the point IDs into the newly created cell
	int numPoints = layer->getPolyLinePoints()->GetNumberOfPoints() - startPointId;

	// Is the POLYLINE a PolyFace object?
	if(polyFace)
	{
		layer->getPolyLineCells()->InsertNextCell(7);
		// Create end "boxes" of PolyFace
		layer->getPolyLineCells()->InsertCellPoint(startPointId);
		layer->getPolyLineCells()->InsertCellPoint(startPointId+1);
		layer->getPolyLineCells()->InsertCellPoint(startPointId+2);
		layer->getPolyLineCells()->InsertCellPoint(startPointId+3);
		layer->getPolyLineCells()->InsertCellPoint(startPointId+4);
		layer->getPolyLineCells()->InsertCellPoint(startPointId+5);
		layer->getPolyLineCells()->InsertCellPoint(startPointId);
		for(int i = 6; i < numPoints-5; i+=6)
		{
			layer->getPolyLineCells()->InsertNextCell(7);
			// Create end "boxes" of PolyFace
			layer->getPolyLineCells()->InsertCellPoint(i+startPointId);
			layer->getPolyLineCells()->InsertCellPoint(i+startPointId+1);
			layer->getPolyLineCells()->InsertCellPoint(i+startPointId+2);
			layer->getPolyLineCells()->InsertCellPoint(i+startPointId+3);
			layer->getPolyLineCells()->InsertCellPoint(i+startPointId+4);
			layer->getPolyLineCells()->InsertCellPoint(i+startPointId+5);
			layer->getPolyLineCells()->InsertCellPoint(i+startPointId);
			// Create connecting side lines
			layer->getPolyLineCells()->InsertNextCell(2);
			layer->getPolyLineCells()->InsertCellPoint(i+startPointId-6);
			layer->getPolyLineCells()->InsertCellPoint(i+startPointId);
			layer->getPolyLineCells()->InsertNextCell(2);
			layer->getPolyLineCells()->InsertCellPoint(i+startPointId-1);
			layer->getPolyLineCells()->InsertCellPoint(i+startPointId+5);
			layer->getPolyLineCells()->InsertNextCell(2);
			layer->getPolyLineCells()->InsertCellPoint(i+startPointId-5);
			layer->getPolyLineCells()->InsertCellPoint(i+startPointId+1);
			layer->getPolyLineCells()->InsertNextCell(2);
			layer->getPolyLineCells()->InsertCellPoint(i+startPointId-2);
			layer->getPolyLineCells()->InsertCellPoint(i+startPointId+4);
		}
	}
	else
	{
		// If it is a closed polyline, then the first point must be added at the
		// end in order to "close" the line
		if(closedPoly == true)
		{
			layer->getPolyLineCells()->InsertNextCell(numPoints+1);
		}
		else
		{
			layer->getPolyLineCells()->InsertNextCell(numPoints);
		}
		for(int i = 0; i < numPoints; i++)
		{
			layer->getPolyLineCells()->InsertCellPoint(i+startPointId);
		}
		if(closedPoly == true)
		{
			layer->getPolyLineCells()->InsertCellPoint(startPointId);
		}
	}
}

void vtkDXFParser::PrintSelf(ostream& os, vtkIndent indent)
{
	//TODO: complete this method
	this->Superclass::PrintSelf(os,indent);
}

void vtkDXFParser::ParseFace(ifstream* file, vtkStdString* name, vtkDXFObjectMap* layerList, bool AutoScale)
{
	this->Read(file);
	double* xyz = new double[3];
	xyz[0] = xyz[1] = xyz[2] = 0.0;
	bool layerFound = false;
	bool inModelSpace = true;
	double layerProp = 0;
	vtkTriangle* cell;
	
	vtkDXFLayer* layer;
	int startPointId;
	int pointId;
	while(this->CommandLine != 0)
	{
		// Add colour properties to the property array
		if(this->CommandLine == 8 && !layerFound)
		{
			*name = this->ValueLine;
			layer = this->GetLayer(*name, layerList);
			//if layer not found, then we have a file with no layers defined, so use the default one
			if(layer == NULL)
			{
				layer = this->GetLayer("DEFAULT", layerList);
			}
			
			layerProp = layer->getLayerPropertyValue();
			startPointId = layer->getSurfPoints()->GetNumberOfPoints();
			pointId = layer->getSurfPoints()->GetNumberOfPoints();
			layerFound = true;

			// If the colour value is negative, the layer is invisible
			if( (layer->getLayerPropertyValue() < 0 || layer->getFreezeValue() == 1) && !layer->getDrawHidden())
			{
				return;
			}
		}
		else if(this->CommandLine == 67)
		{
			inModelSpace = false;
		}
		// X Coordinates
		else if(this->CommandLine >= 10 && this->CommandLine <= 14)
		{
			xyz[0] = this->GetLine();
		}
		// Y Coordinats
		else if(this->CommandLine >= 20 && this->CommandLine <= 24)
		{
			xyz[1] = this->GetLine();
		}
		// Z Coordinates
		else if(this->CommandLine >= 30 && this->CommandLine <= 34)
		{
			xyz[2] = this->GetLine();

			// Check to see if the object is in Model Space and needs to be scaled
			if(inModelSpace && AutoScale)
			{
				xyz[0] += this->XAdj;
				xyz[1] += this->YAdj;

				xyz[0] *= this->XScale;
				xyz[1] *= this->YScale;
				xyz[2] *= this->ZScale;
			}

			layer->getSurfPoints()->InsertPoint(pointId, xyz);
			layer->getSurfProps()->InsertNextTuple1(layerProp);
			pointId++;

			// Last point in the set of 3. Create the cell to hold our triangle data
			if(this->CommandLine == 32)
			{
				cell = vtkTriangle::New();
				layer->getSurfCells()->InsertNextCell(cell);
				cell->Delete();
			}
		}
		// Colour
		else if(this->CommandLine == 62)
		{
			layerProp = this->GetLine();
		}
		this->Read(file);
	}
	// Add the point IDs into the newly created cell
	int numPoints = layer->getSurfPoints()->GetNumberOfPoints() - startPointId;
	layer->getSurfCells()->InsertNextCell(numPoints);
	for(int i = 0; i < numPoints; i++)
	{
		layer->getSurfCells()->InsertCellPoint(i+startPointId);
	}
}

void vtkDXFParser::ParseInsert(ifstream* file, vtkStdString* name, vtkDXFObjectMap* layerList, bool AutoScale)
{
	vtkStdString layerName, blockName;
	this->Read(file);
	double* transforms = new double[3];
	transforms[0] = transforms[1] = transforms[2] = 0.0;
	double* scale = new double[3];
	scale[0] = scale[1] = scale[2] = 1.0;
	bool inModelSpace = true;
	while(this->CommandLine != 0)
	{
		// Layer Name
		if(this->CommandLine == 8)
		{
			layerName = this->ValueLine;
		}
		// Block name
		else if(this->CommandLine == 2)
		{
			blockName = this->ValueLine;
		}
		// X Shift
		else if(this->CommandLine == 10)
		{
			transforms[0] = this->GetLine();
		}
		// Y Shift
		else if(this->CommandLine == 20)
		{
			transforms[1] = this->GetLine();
		}
		// Z Shift
		else if(this->CommandLine == 30)
		{
			transforms[2] = this->GetLine();
		}
		// X Scale
		else if(this->CommandLine == 41)
		{
			scale[0] = this->GetLine();
		}
		// Y Scale
		else if(this->CommandLine == 42)
		{
			scale[1] = this->GetLine();
		}
		// Z Scale
		else if(this->CommandLine == 43)
		{
			scale[2] = this->GetLine();
		}
		else if(this->CommandLine == 67)
		{
			inModelSpace = false;
		}
		this->Read(file);
	}
	// If the block is set in the Paper Space, then undo the scaling
	// that was done before.
	if(!inModelSpace && AutoScale)
	{
		//transforms[0] *= this->LineTypeScale;
		//transforms[1] *= this->LineTypeScale;
		//transforms[2] *= this->LineTypeScale;
		transforms[0] -= this->XAdj;
		transforms[1] -= this->YAdj;
		scale[0] /= this->XScale;
		scale[1] /= this->YScale;
		scale[2] /= this->ZScale;
	}
	// Otherwise, scale the coordinates like every other object
	else if(inModelSpace && AutoScale)
	{
		transforms[0] -= this->XAdj;
		transforms[1] -= this->YAdj;
		transforms[0] *= this->XScale;
		transforms[1] *= this->YScale;
		transforms[2] *= this->ZScale;
	}
	vtkDXFLayer *layer;
	vtkDXFBlock *block;

	layer = this->GetLayer(layerName, layerList);
	
	// If colour is negative, the layer is hidden
	if(layer->getLayerPropertyValue() >= 0 && layer->getFreezeValue() != 1)
	{
		block = this->GetBlock(blockName, this->blockList);
		if(block != NULL)
		{
			vtkDXFBlock* newBlock = vtkDXFBlock::New();
			// Copy the block into the layer's block list
			newBlock->CopyFrom(block);
			newBlock->setBlockTransform(transforms);
			newBlock->setBlockScale(scale);
			newBlock->setDrawBlock(true);
			newBlock->setBlockPropertyValue(layer->getLayerPropertyValue());

			layer->getBlockList()->AddItem(newBlock);
			newBlock->Delete();
		}
	}
}

void vtkDXFParser::ParseDimension(ifstream* file, vtkStdString* name, vtkDXFObjectMap* layerList, bool AutoScale)
{
	vtkStdString layerName, blockName;
	this->Read(file);
	double* transforms = new double[3];
	transforms[0] = transforms[1] = transforms[2] = 0.0;
	double* scale = new double[3];
	scale[0] = scale[1] = scale[2] = 1.0;
	bool inModelSpace = true;

	while(this->CommandLine != 0)
	{
		// Layer Name
		if(this->CommandLine == 8)
		{
			layerName = this->ValueLine;
		}
		// Block name
		else if(this->CommandLine == 2)
		{
			blockName = this->ValueLine;
		}
		// X Shift
		else if(this->CommandLine == 10)
		{
			transforms[0] = this->GetLine();
		}
		// Y Shift
		else if(this->CommandLine == 20)
		{
			transforms[1] = this->GetLine();
		}
		// Z Shift
		else if(this->CommandLine == 30)
		{
			transforms[2] = this->GetLine();
		}
		// X Scale
		//else if(this->CommandLine == 41)
		//{
			//scale[0] = this->GetLine();
		//}
		// Y Scale
		//else if(this->CommandLine == 42)
		//{
			//scale[1] = this->GetLine();
		//}
		// Z Scale
		//else if(this->CommandLine == 43)
		//{
			//scale[2] = this->GetLine();
		//}
		else if(this->CommandLine == 67)
		{
			inModelSpace = false;
		}
		this->Read(file);
	}

	// If the block is set in the Paper Space, then undo the scaling
	// that was done before.
	if(!inModelSpace && AutoScale)
	{
		//transforms[0] *= this->LineTypeScale;
		//transforms[1] *= this->LineTypeScale;
		//transforms[2] *= this->LineTypeScale;
		transforms[0] -= this->XAdj;
		transforms[1] -= this->YAdj;
		scale[0] /= this->XScale;
		scale[1] /= this->YScale;
		scale[2] /= this->ZScale;
	}
	// Otherwise, scale the coordinates like every other object
	else if(inModelSpace && AutoScale)
	{
		transforms[0] -= this->XAdj;
		transforms[1] -= this->YAdj;
		transforms[0] *= this->XScale;
		transforms[1] *= this->YScale;
		transforms[2] *= this->ZScale;
	}

	vtkDXFLayer *layer;
	vtkDXFBlock *block;

	layer = this->GetLayer(layerName, layerList);	
	block = this->GetBlock(blockName, this->blockList);
	if(block != NULL)
	{
		vtkDXFBlock* newBlock = vtkDXFBlock::New();
		// Copy the block into the layer's block list
		newBlock->CopyFrom(block);
		// DIMENSIONS don't seem to need any adjusting...
		transforms[0] = transforms[1] = transforms[2] = 0.0;
		newBlock->setBlockTransform(transforms);
		newBlock->setBlockScale(scale);
		newBlock->setDrawBlock(true);

		layer->getBlockList()->AddItem(newBlock);
		newBlock->Delete();
	}
}

void vtkDXFParser::ParseLine(ifstream* file, vtkStdString* name, vtkDXFObjectMap* layerList, bool AutoScale)
{
	this->Read(file);
	double* xyz = new double[3];
	xyz[0] = xyz[1] = xyz[2] = 0.0;
	bool layerFound = false;
	bool inModelSpace = true;
	double layerProp = 0;

	vtkDXFLayer* layer;
	int startPointId = 0;
	int pointId = 0;
	vtkIdList *pointList = vtkIdList::New();

	while(this->CommandLine != 0)
	{
		// Add colour properties to the property array
		if(this->CommandLine == 8 && !layerFound)
		{
			*name = this->ValueLine;
			layer = this->GetLayer(*name, layerList);
			//if layer not found, then we have a file with no layers defined, so use the default one
			if(layer == NULL)
			{
				layer = this->GetLayer("DEFAULT", layerList);
			}
			layerProp = layer->getLayerPropertyValue();
			startPointId = layer->getLinePoints()->GetNumberOfPoints();
			pointId = layer->getLinePoints()->GetNumberOfPoints();

			//only define the layer once, as it is shared by all the objects in this section
			layerFound = true;

			// If the colour value is negative, the layer is invisible
			if( (layer->getLayerPropertyValue() < 0 || layer->getFreezeValue() == 1) && !layer->getDrawHidden())
			{
				return;
			}
		}
		else if(this->CommandLine == 67)
		{
			inModelSpace = false;
		}
		// X Coordinates
		else if(this->CommandLine == 10 || this->CommandLine == 11)
		{
			xyz[0] = this->GetLine();
		}
		// Y Coordinates
		else if(this->CommandLine == 20 || this->CommandLine == 21)
		{
			xyz[1] = this->GetLine();
		}
		// Z Coordinates
		else if(this->CommandLine == 30 || this->CommandLine == 31)
		{
			xyz[2] = this->GetLine();
			if(inModelSpace && AutoScale)
			{
				xyz[0] += this->XAdj;
				xyz[1] += this->YAdj;

				xyz[0] *= this->XScale;
				xyz[1] *= this->YScale;
				xyz[2] *= this->ZScale;	
			}

			layer->getLinePoints()->InsertPoint(pointId, xyz);
			layer->getLineProps()->InsertNextTuple1(layerProp);
			pointList->InsertNextId( pointId ); //need to create the cell
			pointId++;
		}
		// Colour
		else if(this->CommandLine == 62)
		{
			layerProp = this->GetLine();
		}
		this->Read(file);
	}
	// Add the point IDs into the newly created cell	
	layer->getLineCells()->InsertNextCell(pointList);
	
	//cleanup
	pointList->Delete();
	delete[] xyz;
}

void vtkDXFParser::ParsePoints(ifstream* file, vtkStdString* name, vtkDXFObjectMap* layerList, bool AutoScale)
{
	this->Read(file);
	double* xyz = new double[3];
	xyz[0] = xyz[1] = xyz[2] = 0.0;
	bool layerFound = false;
	bool inModelSpace = true;
	double layerProp = 0;
	
	vtkDXFLayer* layer;
	int startPointId;
	int pointId;
	while(this->CommandLine != 0)
	{
		// Add colour properties to the property array
		if(this->CommandLine == 8 && !layerFound)
		{
			*name = this->ValueLine;
			layer = this->GetLayer(*name, layerList);
			//if layer not found, then we have a file with no layers defined, so use the default one
			if(layer == NULL)
			{
				layer = this->GetLayer("DEFAULT", layerList);
			}
			layerProp = layer->getLayerPropertyValue();
			startPointId = layer->getPointPoints()->GetNumberOfPoints();
			pointId = layer->getPointPoints()->GetNumberOfPoints();

			//only define the layer once, as it is shared by all the objects in this section
			layerFound = true;

			// If the colour value is negative, the layer is invisible
			if( (layer->getLayerPropertyValue() < 0 || layer->getFreezeValue() == 1) && !layer->getDrawHidden())
			{
				return;
			}
		}
		else if(this->CommandLine == 67)
		{
			inModelSpace = false;
		}
		// X Coordinate
		else if(this->CommandLine == 10)
		{
			xyz[0] = this->GetLine();
		}
		// Y Coordinate
		else if(this->CommandLine == 20)
		{
			xyz[1] = this->GetLine();
		}
		// Z Coordinate
		else if(this->CommandLine == 30)
		{
			xyz[2] = this->GetLine();
			if(inModelSpace && AutoScale)
			{
				xyz[0] += this->XAdj;
				xyz[1] += this->YAdj;

				xyz[0] *= this->XScale;
				xyz[1] *= this->YScale;
				xyz[2] *= this->ZScale;
			}

			layer->getPointPoints()->InsertPoint(pointId, xyz);
			layer->getPointProps()->InsertNextTuple1(layerProp);
			layer->getPointCells()->InsertNextCell(1);
			layer->getPointCells()->InsertCellPoint(pointId);
			pointId++;
		}
		// Colour
		else if(this->CommandLine == 62)
		{
			layerProp = this->GetLine();
		}
		this->Read(file);
	}
  delete[] xyz;
}

void vtkDXFParser::ParseLWPolyLine(ifstream* file, vtkStdString* name, vtkDXFObjectMap* layerList, bool AutoScale)
{
	this->Read(file);
	double* xyz = new double[3];
	xyz[0] = xyz[1] = xyz[2] = 0.0;
	bool layerFound = false;
	bool inModelSpace = true;
	// Used to determine if the lwpolyline is closed or not.
	double closedPoly = 0.0;
	double layerProp = 0;
	
	vtkDXFLayer* layer;
	int startPointId = 0;
	int pointId = 0;
	vtkIdList *pointList = vtkIdList::New();
	while(this->CommandLine != 0)
	{
		// Add colour properties to the property array
		if(this->CommandLine == 8 && !layerFound)
		{
			// Grab the layer we are adding data to
			*name = this->ValueLine;
			layer = this->GetLayer(*name, layerList);
			//if layer not found, then we have a file with no layers defined, so use the default one
			if(layer == NULL)
			{
				layer = this->GetLayer("DEFAULT", layerList);
			}
			layerProp = layer->getLayerPropertyValue();
			startPointId = layer->getLWPolyLinePoints()->GetNumberOfPoints();
			pointId = layer->getLWPolyLinePoints()->GetNumberOfPoints();

			//only define the layer once, as it is shared by all the objects in this section
			layerFound = true;

			// If the colour value is negative, the layer is invisible
			if( (layer->getLayerPropertyValue() < 0 || layer->getFreezeValue() == 1) && !layer->getDrawHidden())
			{
				return;
			}
		}
		else if(this->CommandLine == 67)
		{
			inModelSpace = false;
		}
		// Z Coordinate (Static)
		if(this->CommandLine == 38)
		{
			xyz[2] = this->GetLine();
		}
		else if(this->CommandLine == 70)
		{
			closedPoly = this->GetLine();
		}
		// X Coordinate
		else if(this->CommandLine == 10 || this->CommandLine == 11)
		{
			xyz[0] = this->GetLine();
		}
		// Y Coordinate
		else if(this->CommandLine == 20 || this->CommandLine == 21)
		{
			xyz[1] = this->GetLine();
			// Check to see if the object is in Model Space and needs to be scaled
			if(inModelSpace && AutoScale)
			{
				xyz[0] += this->XAdj;
				xyz[1] += this->YAdj;

				xyz[0] *= this->XScale;
				xyz[1] *= this->YScale;
				xyz[2] *= this->ZScale;
			}

			layer->getLWPolyLinePoints()->InsertPoint(pointId, xyz);
			layer->getLWPolyLineProps()->InsertNextTuple1(layerProp);
			pointList->InsertNextId( pointId );
			pointId++;
		}
		// Colour
		else if(this->CommandLine == 62)
		{
			layerProp = this->GetLine();
		}
		this->Read(file);
	}
	// Add the point IDs into the newly created cell
	if(closedPoly == 1.0)
	{
		pointList->InsertNextId( startPointId );		
	}
	layer->getLWPolyLineCells()->InsertNextCell( pointList );
	
  delete[] xyz;
	pointList->Delete();
}

vtkStdString vtkDXFParser::CleanString(vtkStdString &line, bool mText, int &numLine)
{
	// DXF files use %%, \, and | commands to manipulate the text
	// (%%uText%%u would underline the text)
	// This method will remove all instances of %% commands
	// as well as all slash (\) commands and | commands
	std::string::size_type idx = -1;

	idx = -1;
	// Replace MTEXT starting line, (A1;)
	vtkStdString replaceMe = "\\A1;";
	vtkStdString replaceWith = "";
	while((idx = line.find(replaceMe, idx+1)) != std::string::npos)
	line.replace(idx, strlen(replaceMe.c_str()), replaceWith);

	idx = -1;
	// Replace new paragraph marks "\P" with "\n"
	replaceMe = "\\P";
	replaceWith = "\n";
	while((idx = line.find(replaceMe, idx+1)) != std::string::npos)
	{
		line.replace(idx, strlen(replaceMe.c_str()), replaceWith);
		// Keep track of the number of new lines, will be used in the
		// jusification calculations
		numLine++;
	}

	idx = -1;
	// Replace new paragraph marks (P)
	replaceMe = "\\H";
	replaceWith = "\n";
	while((idx = line.find(replaceMe, idx+1)) != std::string::npos)
	line.replace(idx, strlen(replaceMe.c_str())+4, replaceWith);

	// Replace any %% command
	//vtkStdString replaceMe = "%%";
	//vtkStdString replaceWith = "";
  replaceMe = "%%";
	replaceWith = "";
	while((idx = line.find(replaceMe, idx+1)) != std::string::npos)
	line.replace(idx, strlen(replaceMe.c_str())+1, replaceWith);

	// Replace FROMI and Fromi
	idx = -1;
	replaceMe = "\\FROMI";
	replaceWith = "";
	while((idx = line.find(replaceMe, idx+1)) != std::string::npos)
	line.replace(idx, strlen(replaceMe.c_str()), replaceWith);

	idx = -1;
	replaceMe = "\\Fromi";
	replaceWith = "";
	while((idx = line.find(replaceMe, idx+1)) != std::string::npos)
	line.replace(idx, strlen(replaceMe.c_str()), replaceWith);

	// Replace the \F command for setting the font
	idx = -1;
	replaceMe = "\\F";
	replaceWith = "";
	while((idx = line.find(replaceMe, idx+1)) != std::string::npos)
	{
		int semiLocation = line.find(";", idx);
		line.replace(idx, ((semiLocation+1)-idx), replaceWith);//strlen(replaceMe.c_str())
		idx = -1;
	}

	// Replace the \f command for setting the font
	idx = -1;
	replaceMe = "\\f";
	replaceWith = "";
	while((idx = line.find(replaceMe, idx+1)) != std::string::npos)
	{
		int semiLocation = line.find(";", idx);
		line.replace(idx, ((semiLocation+1)-idx), replaceWith);//strlen(replaceMe.c_str())
		idx = -1;
	}

	// Replace the \C# commands
	idx = -1;
	replaceMe = "\\C";
	replaceWith = "";
	while((idx = line.find(replaceMe, idx+1)) != std::string::npos)
	line.replace(idx, strlen(replaceMe.c_str())+1, replaceWith);

	// Replace ;
	idx = -1;
	replaceMe = ";";
	replaceWith = "";
	while((idx = line.find(replaceMe, idx+1)) != std::string::npos)
  line.replace(idx, strlen(replaceMe.c_str()), replaceWith);

	// Replace any "\" command
	idx = -1;
	replaceMe = "\\";
	replaceWith = "";
	while((idx = line.find(replaceMe, idx+1)) != std::string::npos)
	line.replace(idx, strlen(replaceMe.c_str())+1, replaceWith);

	// Replace "+2205" command
	// Ignoring special characters in text.
	idx = -1;
	replaceMe = "+2205";
	replaceWith = "";
	while((idx = line.find(replaceMe, idx+1)) != std::string::npos)
	line.replace(idx, strlen(replaceMe.c_str()), replaceWith);

	// Replace Arial Black
	idx = -1;
	replaceMe = ";";
	replaceWith = "";
	while((idx = line.find(replaceMe, idx+1)) != std::string::npos)
  line.replace(idx, strlen(replaceMe.c_str()), replaceWith);

	idx = -1;
	// Replace { and }
	replaceMe = "{";
	replaceWith = "";
	while((idx = line.find(replaceMe, idx+1)) != std::string::npos)
  line.replace(idx, strlen(replaceMe.c_str()), replaceWith);
	idx = -1;
	replaceMe = "}";
	replaceWith = "";
	while((idx = line.find(replaceMe, idx+1)) != std::string::npos)
  line.replace(idx, strlen(replaceMe.c_str()), replaceWith);

	return line;
}

void vtkDXFParser::ParseText(ifstream* file, vtkStdString* name, vtkDXFObjectMap* layerList, bool mText, bool AutoScale)
{
	this->Read(file);
	double* xyz = new double[3];
	xyz[0] = xyz[1] = xyz[2] = 0.0;
	double* offset = new double[3];
	// Initialize the offsets to 0.0, this eliminates problems that
	// will happen later on if offset is nto initialized.
	offset[0] = offset[1] = offset[2] = 0.0;
	// Set default text scale to 1.0 (0 makes things disappear)
	double scale = 1.0;
	// Set default text rotation
	double textRotation = 0.0;
	// Justification:
	// 0 = left, 1 = center, 2 = right
	vtkStdString justification = "0";
	// Length of the word/number to be justified
	int justLen = 0;
	// How many lines are in the text? (used for justification calculation)
	int numLine = 1;
	// Used to hold text data from multiple lines(CommandLine 3 and CommandLine 1)
	vtkStdString line = "";
	bool layerFound = false;
	bool inModelSpace = true;
	double layerProp = 0;

	vtkDXFLayer* layer;
	vtkVectorText *text = vtkVectorText::New();
	// The transform and transform filter are used to manipulate
	// the text object after it is created.
	vtkTransform *transform = vtkTransform::New();
	vtkTransformFilter *tf = vtkTransformFilter::New();
	while(this->CommandLine != 0)
	{
		// Grab the current layer that we are adding data to
		if(this->CommandLine == 8 && !layerFound)
		{
			*name = this->ValueLine;			
			layerFound = true;

			layer = this->GetLayer(*name, layerList);
			
			
			layerProp = layer->getLayerPropertyValue();
			// If the colour value is negative, the layer is invisible
			if( (layer->getLayerPropertyValue() < 0 || layer->getFreezeValue() == 1) && !layer->getDrawHidden())
			{
				return;
			}
		}
		else if(this->CommandLine == 67)
		{
			inModelSpace = false;
		}
		// X Coordinate
		else if(this->CommandLine == 10)
		{
			xyz[0] = this->GetLine();
		}
		// X Alignment Offset
		else if(this->CommandLine == 11)
		{
			offset[0] = this->GetLine();
		}
		// Y Coordinate
		else if(this->CommandLine == 20)
		{
			xyz[1] = this->GetLine();
		}
		// Y Alignment Offset
		else if(this->CommandLine == 21)
		{
			offset[1] = this->GetLine();
		}
		// Z Coordinate
		else if(this->CommandLine == 30)
		{
			xyz[2] = this->GetLine();
		}
		// Z Alignment Offset
		else if(this->CommandLine == 31)
		{
			offset[2] = this->GetLine();
		}
		else if(this->CommandLine == 3)
		{
			line.append(this->ValueLine);
		}
		// Text data line
		else if(this->CommandLine == 1)
		{
			line.append(this->ValueLine);
			// Clean the AutoCAD junk from our text
			vtkStdString cleanline = this->CleanString(line, mText, numLine);
			// Use our nice, clean text
			text->SetText(cleanline);
			// Save the length of the text added, will be used later for jusification
			justLen = line.length();
		}
		// The 40 command line is used to set the scale
		else if(this->CommandLine == 40)
		{
			scale = this->GetLine();
			// Adjust scale
			// Check to see if the object is in Model Space and needs to be scaled
			if(inModelSpace && AutoScale)
			{
				scale *= this->XScale;
			}

			// Autodesk and Paraview do not agree on scale, this fixes that
			scale *= 0.85;
		}
		// Text justification
		else if(this->CommandLine == 72)
		{
			justification = this->TrimWhiteSpace(this->ValueLine);
		}
		// Text rotation
		else if(this->CommandLine == 50)
		{
			textRotation = this->GetLine();
		}
		// Colour
		else if(this->CommandLine == 62)
		{
			layerProp = this->GetLine();
		}
		this->Read(file);
	}

	// Set text justification
	if(justification == "1")
	{ //Center justification
		// MTEXT and TEXT elements need to aligned differently
		if(mText)
		{
			xyz[0] = xyz[0] - ((0.4*scale) * justLen/numLine);
		}
		else
		{
			xyz[0] = xyz[0] - ((0.1*scale) * (justLen/2));
		}
	}
	else if(justification == "2")
	{ // Right justification
		if(mText)
		{
			xyz[0] = xyz[0] - ((0.8*scale) * justLen/numLine);
		}
		else
		{
			xyz[0] = xyz[0] - ((0.1*scale) * (justLen));
		}
	}

	// If there are offsets available, then use them
	// These offsets allow the text to be "aligned" properly
	// There is still an issue with the text not being in 
	// the proper location. This is due to the text being 
	// shifted for the alignment.
	// To have the text in the proper position, the text 
	// must be grouped properly, and then the groups must
	// be shifted as a whole into the proper positions.
	// This is not possible with the way we are currently 
	// handling the text data.
	// *NOT USING* Causes TEXT to be offset improperly
	// and MTEXT to go all over the place
	/*if(mText == true)
	{
	if(offset[0] != 0.0)
	{
		xyz[0] = xyz[0] - (offset[0] - xyz[0]);
	}
	if(offset[1] != 0.0)
	{
		xyz[1] = xyz[1] - (offset[1] - xyz[1]);
	}
	if(offset[2] != 0.0)
	{
		xyz[2] = xyz[2] - (offset[2] - xyz[2]);
	}
	}*/

	// Check to see if the object is in Model Space and needs to be scaled
	if(inModelSpace && AutoScale)
	{
		xyz[0] += this->XAdj;
		xyz[1] += this->YAdj;

		xyz[0] *= this->XScale;
		xyz[1] *= this->YScale;
		xyz[2] *= this->ZScale;
	}

	// Do not change the order of Translate and Scale, doing
	// so will cause the text to be placed in the wrong position
	transform->Translate(xyz);
	transform->Scale(scale, scale, scale);
	// Objects are drawn on the Z plane, so we can only scale on the Z axis.
	transform->RotateZ(textRotation);
	   
	tf->SetInput(text->GetOutput());
	tf->SetTransform(transform);

	tf->Update();
	for(int i = 0; i < tf->GetPolyDataOutput()->GetPoints()->GetNumberOfPoints(); i++)
	{
		layer->getTextProps()->InsertNextTuple1(layerProp);
	}
	layer->getText()->AddItem(tf->GetPolyDataOutput());
	
	tf->Delete();
	transform->Delete();	
	text->Delete();
	delete[] xyz;
	delete[] offset;
}

void vtkDXFParser::ParseCircle(ifstream* file, vtkStdString* name, vtkDXFObjectMap* layerList, bool AutoScale)
{
	this->Read(file);
	double* xyz = new double[3];
	xyz[0] = xyz[1] = xyz[2] = 0.0;
	double radius = 0.0;
	bool layerFound = false;
	bool inModelSpace = true;
	double layerProp = 0;
	
	vtkDXFLayer* layer;
	vtkDiskSource *disk = vtkDiskSource::New();
	// The transform and transform filter are used to manipulate
	// the text object after it is created.
	vtkTransform *transform = vtkTransform::New();
	vtkTransformFilter *tf = vtkTransformFilter::New();
	while(this->CommandLine != 0)
	{
		// Grab the current layer that we are adding data to
		if(this->CommandLine == 8 && !layerFound) //&& this->layerExists == 1
		{
			*name = this->ValueLine;
			layer = this->GetLayer(*name, layerList);
			layerFound = true;
			layerProp = layer->getLayerPropertyValue();

			// If the colour value is negative, the layer is invisible
			if( (layer->getLayerPropertyValue() < 0 || layer->getFreezeValue() == 1) && !layer->getDrawHidden())
			{
				return;
			}
		}
		else if(this->CommandLine == 67)
		{
			inModelSpace = false;
		}
		// X Coordinate
		else if(this->CommandLine == 10)
		{
			xyz[0] = this->GetLine();
		}
		// Y Coordinate
		else if(this->CommandLine == 20)
		{
			xyz[1] = this->GetLine();
		}
		// Z Coordinate
		else if(this->CommandLine == 30)
		{
			xyz[2] = this->GetLine();
		}
		// Colour
		else if(this->CommandLine == 62)
		{
			layerProp = this->GetLine();
		}
		// The 40 command line is used to set the scale
		else if(this->CommandLine == 40)
		{
			radius = this->GetLine();
			// Check to see if the object is in Model Space and needs to be scaled
			if(inModelSpace && AutoScale)
			{
				radius *= this->XScale;
			}

			double outerRadius = radius - (0.030*radius);
			disk->SetOuterRadius(radius);
			if(radius < 1.5)
			{
				disk->SetInnerRadius(radius - 0.035);
			}
			else
			{
				disk->SetInnerRadius(outerRadius);
			}
		}
		this->Read(file);
	}
	disk->SetCircumferentialResolution(10);

	// Check to see if the object is in Model Space and needs to be scaled
	if(inModelSpace && AutoScale)
	{	
		xyz[0] += this->XAdj;
		xyz[1] += this->YAdj;

		xyz[0] *= this->XScale;
		xyz[1] *= this->YScale;
		xyz[2] *= this->ZScale;
	}

	// Do not change the order of Translate and Scale, doing
	// so will cause the text to be placed in the wrong position
	transform->Translate(xyz);
	   
	tf->SetInput(disk->GetOutput());
	tf->SetTransform(transform);

	tf->Update();
	for(int i = 0; i < tf->GetPolyDataOutput()->GetPoints()->GetNumberOfPoints(); i++)
	{
		layer->getTextProps()->InsertNextTuple1(layerProp);
	}
	layer->getCircles()->AddItem(tf->GetPolyDataOutput());
	
	tf->Delete();
	transform->Delete();
	disk->Delete();
	delete[] xyz;
}

void vtkDXFParser::ParseArc(ifstream* file, vtkStdString* name, vtkDXFObjectMap* layerList, bool AutoScale)
{
	this->Read(file);
	double* centerPoint = new double[3];
	centerPoint[0] = centerPoint[1] = centerPoint[2] = 0.0;
	double radius = 0;
	double beginAngle = 0;
	double endAngle = 0;
	bool layerFound = false;
	bool inModelSpace = true;
	double layerProp = 0;
	
	vtkDXFLayer* layer;
	int startPointId;
	int pointId;
	while(this->CommandLine != 0)
	{
		// Add colour properties to the property array
		if(this->CommandLine == 8 && !layerFound)
		{
			*name = this->ValueLine;
			layer = this->GetLayer(*name, layerList);
			//if layer not found, then we have a file with no layers defined, so use the default one
			if(layer == NULL)
			{
				layer = this->GetLayer("DEFAULT", layerList);
			}
			layerProp = layer->getLayerPropertyValue();
			startPointId = layer->getArcPoints()->GetNumberOfPoints();
			pointId = layer->getArcPoints()->GetNumberOfPoints();

			//only define the layer once, as it is shared by all the objects in this section
			layerFound = true;

			// If the colour value is negative, the layer is invisible
			if( (layer->getLayerPropertyValue() < 0 || layer->getFreezeValue() == 1) && !layer->getDrawHidden())
			{
				return;
			}
		}
		else if(this->CommandLine == 67)
		{
			inModelSpace = false;
		}
		// X Coordinate
		else if(this->CommandLine == 10)
		{
			centerPoint[0] = this->GetLine();
		}
		// Y Coordinate
		else if(this->CommandLine == 20)
		{
			centerPoint[1] = this->GetLine();
		}
		// Z Coordinate
		else if(this->CommandLine == 30)
		{
			centerPoint[2] = this->GetLine();
		}
		// Radius
		else if(this->CommandLine == 40)
		{
			radius = this->GetLine();
		}
		// Begin Angle
		else if(this->CommandLine == 50)
		{
			beginAngle = this->GetLine();
		}
		// End Angle
		else if(this->CommandLine == 51)
		{
			endAngle = this->GetLine();
		}
		// Colour
		else if(this->CommandLine == 62)
		{
			layerProp = this->GetLine();
		}
		this->Read(file);
	}
	double angleLength = endAngle - beginAngle;
	double x;
	double y;
	double z = centerPoint[2]; //always constant
	if(inModelSpace && AutoScale)
	{
		z *= this->ZScale;
	}
	
	double increment = 10.0; //default angular distance between pts, may be reduced depending on angleLength
	int numPts = angleLength / increment + 1;
	//for visibility purposes, we want at least 5 points.  this helps with arcs of short radian length.
	if(numPts < 5)
	{
		numPts = 5;
	}
	increment = angleLength / (numPts-1); //new increment value so that points are uniformily distributed
	int count = 0;
	double currentAngle;
	if(beginAngle < endAngle)
	{
		for(currentAngle = beginAngle; (currentAngle - endAngle) <= 0.000001; currentAngle += increment)
		{
			double cosResult = radius * cos(currentAngle * (PI / 180.0));
			double sinResult = radius * sin(currentAngle * (PI / 180.0));
			x = centerPoint[0] + cosResult;
			y = centerPoint[1] + sinResult;

			// Check to see if the object is in Model Space and needs to be scaled
			if(inModelSpace && AutoScale)
			{
				x += this->XAdj;
				y += this->YAdj;

				x *= this->XScale;
				y *= this->YScale;
			}

			layer->getArcPoints()->InsertPoint(pointId, x, y, z);
			layer->getArcProps()->InsertNextTuple1(layerProp);
			pointId++;
			count++;
		}
		layer->getArcCells()->InsertNextCell(numPts);
	}
	else //arc crosses 360 degrees and therefore endAngle < beginAngle
	{
		double oldBegin = beginAngle;
		double oldEnd = endAngle;
		beginAngle = 180 - oldEnd;
		endAngle = 540 - oldBegin;
		angleLength = endAngle - beginAngle;
		increment = 10.0;
		numPts = angleLength / increment + 1;
		if(numPts < 5)
		{
			numPts = 5;
		}
		increment = angleLength / (numPts-1);
		for(currentAngle = beginAngle; (currentAngle - endAngle) <= 0.000001; currentAngle += increment)
		{
			x = centerPoint[0] - radius * cos(currentAngle * (PI / 180.0));
			y = centerPoint[1] + radius * sin(currentAngle * (PI / 180.0));

			// Check to see if the object is in Model Space and needs to be scaled
			if(inModelSpace && AutoScale)
			{
				x += this->XAdj;
				y += this->YAdj;

				x *= this->XScale;
				y *= this->YScale;
			}

			layer->getArcPoints()->InsertPoint(pointId, x, y, z);
			layer->getArcProps()->InsertNextTuple1(layerProp);
			pointId++;
			count++;
		}
		layer->getArcCells()->InsertNextCell(numPts);
	}
	for(int i = 0; i < count; i++)
	{
		layer->getArcCells()->InsertCellPoint(i+startPointId);
	}
delete[] centerPoint;
}

void vtkDXFParser::ParseSolid(ifstream* file, vtkStdString* name, vtkDXFObjectMap* layerList, bool AutoScale)
{
	this->Read(file);
	double* xyz = new double[3];
	xyz[0] = xyz[1] = xyz[2] = 0.0;
	// Used to keep track of our vertices
	double xyzList[4][3];
	xyzList[0][0] = xyzList[0][1] = xyzList[0][2] =
		xyzList[1][0] = xyzList[1][1] = xyzList[1][2] =
		xyzList[2][0] = xyzList[2][1] = xyzList[2][2] = 
		xyzList[3][0] = xyzList[3][1] = xyzList[3][2] = 0.0;
	int vertCount = 0;

	bool layerFound = false;
	bool inModelSpace = true;
	double layerProp = 0;

	vtkDXFLayer* layer;
	int startPointId;
	int pointId;
	while(this->CommandLine != 0)
	{
		// Add colour properties to the property array
		if(this->CommandLine == 8 && !layerFound)
		{
			*name = this->ValueLine;
			layer = this->GetLayer(*name, layerList);
			//if layer not found, then we have a file with no layers defined, so use the default one
			if(layer == NULL)
			{
				layer = this->GetLayer("DEFAULT", layerList);
			}
			layerProp = layer->getLayerPropertyValue();
			startPointId = layer->getSolidPoints()->GetNumberOfPoints();
			pointId = layer->getSolidPoints()->GetNumberOfPoints();

			//only define the layer once, as it is shared by all the objects in this section
			layerFound = true;

			// If the colour value is negative, the layer is invisible
			if( (layer->getLayerPropertyValue() < 0 || layer->getFreezeValue() == 1) && !layer->getDrawHidden())
			{
				return;
			}
		}
		else if(this->CommandLine == 67)
		{
			inModelSpace = false;
		}
		// X Coordinates
		else if(this->CommandLine >= 10 && this->CommandLine <= 13)
		{
			xyz[0] = this->GetLine();
		}
		// Y Coordinates
		else if(this->CommandLine >= 20 && this->CommandLine <= 23)
		{
			xyz[1] = this->GetLine();
		}
		// Z Coordinates
		else if(this->CommandLine >= 30 && this->CommandLine <= 33)
		{
			xyz[2] = this->GetLine();
			if(inModelSpace && AutoScale)
			{
				xyz[0] += this->XAdj;
				xyz[1] += this->YAdj;

				xyz[0] *= this->XScale;
				xyz[1] *= this->YScale;
				xyz[2] *= this->ZScale;
			}
			xyzList[vertCount][0] = xyz[0];
			xyzList[vertCount][1] = xyz[1];
			xyzList[vertCount][2] = xyz[2];
			vertCount++;
		}
		// Colour
		else if(this->CommandLine == 62)
		{
			layerProp = this->GetLine();
		}
		this->Read(file);
	}
	if(xyzList[2][0] == xyzList[3][0] && xyzList[2][1] == xyzList[3][1] && xyzList[2][2] == xyzList[3][2])
	{
		vertCount--;
	}

	if(layerFound)
	{
		for(int pt = 0; pt < vertCount; pt++)
		{
			layer->getSolidPoints()->InsertPoint(pointId, xyzList[pt][0], xyzList[pt][1], xyzList[pt][2]);
			layer->getSolidProps()->InsertNextTuple1(layerProp);
			pointId++;
		}
		// Add the point IDs into the newly created cell
		int numPoints = layer->getSolidPoints()->GetNumberOfPoints() - startPointId;
		for(int i = 0; i < numPoints-2; i++)
		{
			layer->getSolidCells()->InsertNextCell(3);
			layer->getSolidCells()->InsertCellPoint(i+startPointId);
			layer->getSolidCells()->InsertCellPoint(i+1+startPointId);
			layer->getSolidCells()->InsertCellPoint(i+2+startPointId);
		}
	}
delete[] xyz;
}

// BLOCK PARSING METHODS
void vtkDXFParser::ParseLineBlock(ifstream* file, vtkStdString* name, vtkDXFObjectMap* layerList, vtkDXFBlock* block, bool AutoScale)//vtkCollection* blockList, vtkStdString blockName)
{
	this->Read(file);
	double* xyz = new double[3];
	xyz[0] = xyz[1] = xyz[2] = 0.0;
	bool layerFound = false;
	bool inModelSpace = true;
	double layerProp = 0;
	
	vtkDXFLayer* layer;
	int startPointId;
	int pointId;
	while(this->CommandLine != 0)
	{
		// Add colour properties to the property array
		if(this->CommandLine == 8 && !layerFound)
		{
			*name = this->ValueLine;
			layer = this->GetLayer(*name, layerList);
			//if layer not found, then we have a file with no layers defined, so use the default one
			if(layer == NULL)
			{
				vtkErrorMacro("BLOCK found without LAYER reference.");
			}

			//only define the layer once, as it is shared by all the objects in this section
			layerFound = true;

			layerProp = layer->getLayerPropertyValue();
			startPointId = block->getLinePoints()->GetNumberOfPoints();
			pointId = block->getLinePoints()->GetNumberOfPoints();

			// If the colour value is negative, the layer is invisible
			if( (layer->getLayerPropertyValue() < 0 || layer->getFreezeValue() == 1) && !layer->getDrawHidden())
			{
				return;
			}
		}
		else if(this->CommandLine == 67)
		{
			inModelSpace = false;
		}
		// X Coordinates
		else if(this->CommandLine == 10 || this->CommandLine == 11)
		{
			xyz[0] = this->GetLine();
		}
		// Y Coordinates
		else if(this->CommandLine == 20 || this->CommandLine == 21)
		{
			xyz[1] = this->GetLine();
		}
		// Z Coordinates
		else if(this->CommandLine == 30 || this->CommandLine == 31)
		{
			xyz[2] = this->GetLine();
			// Check to see if the object is in Model Space and needs to be scaled
			if(inModelSpace && AutoScale)
			{	
				xyz[0] += this->XAdj;
				xyz[1] += this->YAdj;

				xyz[0] *= this->XScale;
				xyz[1] *= this->YScale;
				xyz[2] *= this->ZScale;
			}

			block->getLinePoints()->InsertPoint(pointId, xyz);
			block->getLineProps()->InsertNextTuple1(layerProp);
			pointId++;
		}
		// Colour
		else if(this->CommandLine == 62)
		{
			layerProp = this->GetLine();
		}
		this->Read(file);
	}
	// Add the point IDs into the newly created cell
	int numPoints = block->getLinePoints()->GetNumberOfPoints() - startPointId;
	block->getLineCells()->InsertNextCell(numPoints);
	for(int i = 0; i < numPoints; i++)
	{
		block->getLineCells()->InsertCellPoint(i+startPointId);
	}
	
delete[] xyz;
}

void vtkDXFParser::ParsePolyLineBlock(ifstream* file, vtkStdString* name, vtkDXFObjectMap* layerList, bool firstPoint, vtkDXFBlock* block, bool AutoScale)
{
	this->Read(file);
	bool closedPoly = false;
	this->currElevation = 0.0;
	double* xyz = new double[3];
	xyz[0] = xyz[1] = xyz[2] = 0.0;
	bool layerFound = false;
	bool inModelSpace = true;
	double layerProp = 0;
	// Variables used to keep track of polyFace data
	bool skipVertex = false;
	bool polyFace = false;
	int vertCount = 6;
	int currVertNum = 0;
	bool vertSet = false;
	
	vtkDXFLayer* layer;
	int startPointId;
	int pointId;
	while(this->ValueLine != "SEQEND")
	{
		// Add colour properties to the property array
		if(this->CommandLine == 8 && !layerFound)
		{
			*name = this->ValueLine;
			layer = this->GetLayer(*name, layerList);
			//if layer not found, then we have a file with no layers defined, so use the default one
			if(layer == NULL)
			{
				vtkErrorMacro("BLOCK found without LAYER reference. You cannot have a BLOCK outside a layer.");
			}
			layerProp = layer->getLayerPropertyValue();
			startPointId = block->getPolyLinePoints()->GetNumberOfPoints();
			pointId = block->getPolyLinePoints()->GetNumberOfPoints();

			//only define the layer once, as it is shared by all the objects in this section
			layerFound = true;

			// If the colour value is negative, the layer is invisible
			if( (layer->getLayerPropertyValue() < 0 || layer->getFreezeValue() == 1) && !layer->getDrawHidden())
			{
				return;
			}
		}
		else if(this->CommandLine == 67)
		{
			inModelSpace = false;
		}
		else if(this->CommandLine == 70)
		{
			vtkStdString polyVal = this->TrimWhiteSpace(this->ValueLine);
			if(polyVal == "1")
			{
				//this defines a closed poly line, the first point will have to be duplicated at the end of the list
				closedPoly = true;
			}
			else if(polyVal == "64")
			{
				polyFace = true;
				skipVertex = true;
				firstPoint = false;
			}
			else if(polyVal == "128")
			{
				skipVertex = true;
				firstPoint = false;
			}
		}
		else if(this->CommandLine == 71)
		{
			vertCount = (int)this->GetLine();
			// Added one extra end (6 points) as this count seems to leave them out.
			vertCount += 6;
		}
		// X Coordinate
		else if(this->CommandLine == 10 || this->CommandLine == 11)
		{
			xyz[0] = this->GetLine();
			currVertNum++;
			vertSet = true;
		}
		// Y Coordinate
		else if(this->CommandLine == 20 || this->CommandLine == 21)
		{
			xyz[1] = this->GetLine();
		}
		// Z Coordinate
		else if(this->CommandLine == 30 || this->CommandLine == 31)
		{
			if(firstPoint == false)
			{
				xyz[2] = this->GetLine();
				// Check to see if the point needs the elevation applied to it
				if(xyz[2] != this->currElevation)
				{
					xyz[2] += this->currElevation;
				}
			}
			else
			{
				//firstPoint = false;
				this->currElevation = this->GetLine();
			}
		}
		// Colour
		else if(this->CommandLine == 62)
		{
			layerProp = this->GetLine();
		}
		else if(this->CommandLine == 0)
		{
			if((currVertNum <= vertCount || vertCount == 6) && !skipVertex)
			{
				if(vertSet && firstPoint == false)
				{
					// Check to see if the object is in Model Space and needs to be scaled
					if(inModelSpace && AutoScale)
					{
						xyz[0] += this->XAdj;
						xyz[1] += this->YAdj;

						xyz[0] *= this->XScale;
						xyz[1] *= this->YScale;
						xyz[2] *= this->ZScale;
					}

					block->getPolyLinePoints()->InsertPoint(pointId, xyz);
					block->getPolyLineProps()->InsertNextTuple1(layerProp);
					pointId++;
					vertSet = false;
				}
				else
				{
					if(firstPoint)
					{
						firstPoint = false;
					}
				}
			}
			else
			{
				if(skipVertex)
				{
					skipVertex = false;
				}
			}
		}
		this->Read(file);
	}
	if((currVertNum <= vertCount || vertCount == 6) && !skipVertex)
	{
		if(vertSet)
		{
		// Check to see if the object is in Model Space and needs to be scaled
		if(inModelSpace && AutoScale)
		{
			xyz[0] += this->XAdj;
			xyz[1] += this->YAdj;

			xyz[0] *= this->XScale;
			xyz[1] *= this->YScale;
			xyz[2] *= this->ZScale;
		}

		block->getPolyLinePoints()->InsertPoint(pointId, xyz);
		block->getPolyLineProps()->InsertNextTuple1(layerProp);
		pointId++;
		}
	}

	// Add the point IDs into the newly created cell
	int numPoints = block->getPolyLinePoints()->GetNumberOfPoints() - startPointId;

	// Is the POLYLINE a PolyFace object?
	if(polyFace)
	{
		block->getPolyLineCells()->InsertNextCell(7);
		// Create end "boxes" of PolyFace
		block->getPolyLineCells()->InsertCellPoint(startPointId);
		block->getPolyLineCells()->InsertCellPoint(startPointId+1);
		block->getPolyLineCells()->InsertCellPoint(startPointId+2);
		block->getPolyLineCells()->InsertCellPoint(startPointId+3);
		block->getPolyLineCells()->InsertCellPoint(startPointId+4);
		block->getPolyLineCells()->InsertCellPoint(startPointId+5);
		block->getPolyLineCells()->InsertCellPoint(startPointId);
		for(int i = 6; i < numPoints-5; i+=6)
		{
			block->getPolyLineCells()->InsertNextCell(7);
			// Create end "boxes" of PolyFace
			block->getPolyLineCells()->InsertCellPoint(i+startPointId);
			block->getPolyLineCells()->InsertCellPoint(i+startPointId+1);
			block->getPolyLineCells()->InsertCellPoint(i+startPointId+2);
			block->getPolyLineCells()->InsertCellPoint(i+startPointId+3);
			block->getPolyLineCells()->InsertCellPoint(i+startPointId+4);
			block->getPolyLineCells()->InsertCellPoint(i+startPointId+5);
			block->getPolyLineCells()->InsertCellPoint(i+startPointId);
			// Create connecting side lines
			block->getPolyLineCells()->InsertNextCell(2);
			block->getPolyLineCells()->InsertCellPoint(i+startPointId-6);
			block->getPolyLineCells()->InsertCellPoint(i+startPointId);
			block->getPolyLineCells()->InsertNextCell(2);
			block->getPolyLineCells()->InsertCellPoint(i+startPointId-1);
			block->getPolyLineCells()->InsertCellPoint(i+startPointId+5);
			block->getPolyLineCells()->InsertNextCell(2);
			block->getPolyLineCells()->InsertCellPoint(i+startPointId-5);
			block->getPolyLineCells()->InsertCellPoint(i+startPointId+1);
			block->getPolyLineCells()->InsertNextCell(2);
			block->getPolyLineCells()->InsertCellPoint(i+startPointId-2);
			block->getPolyLineCells()->InsertCellPoint(i+startPointId+4);
		}
	}
	else
	{
		// If it is a closed polyline, then the first point must be added at the
		// end in order to "close" the line
		if(closedPoly == true)
		{
			block->getPolyLineCells()->InsertNextCell(numPoints+1);
		}
		else
		{
			block->getPolyLineCells()->InsertNextCell(numPoints);
		}
		for(int i = 0; i < numPoints; i++)
		{
			block->getPolyLineCells()->InsertCellPoint(i+startPointId);
		}
		if(closedPoly == true)
		{
			block->getPolyLineCells()->InsertCellPoint(startPointId);
		}
	}
delete[] xyz;
}

void vtkDXFParser::ParseLWPolyLineBlock(ifstream* file, vtkStdString* name, vtkDXFObjectMap* layerList, vtkDXFBlock* block, bool AutoScale)
{
	this->Read(file);
	double* xyz = new double[3];
	xyz[0] = xyz[1] = xyz[2] = 0.0;
	bool layerFound = false;
	bool inModelSpace = true;
	// Used to ensure that the zCoordinate in only scaled once
	bool zChanged = false;
	// Used to determine if the lwpolyline is closed or not.
	double closedPoly = 0.0;
	double layerProp = 0;
	
	vtkDXFLayer* layer;
	int startPointId;
	int pointId;
	while(this->CommandLine != 0)
	{
		// Add colour properties to the property array
		if(this->CommandLine == 8 && !layerFound)
		{
			// Grab the layer we are adding data to
			*name = this->ValueLine;
			layer = this->GetLayer(*name, layerList);
			//if layer not found, then we have a file with no layers defined, so use the default one
			if(layer == NULL)
			{
				layer = this->GetLayer("DEFAULT", layerList);
			}
			layerProp = layer->getLayerPropertyValue();
			startPointId = block->getLWPolyLinePoints()->GetNumberOfPoints();
			pointId = block->getLWPolyLinePoints()->GetNumberOfPoints();

			//only define the layer once, as it is shared by all the objects in this section
			layerFound = true;

			// If the colour value is negative, the layer is invisible
			if( (layer->getLayerPropertyValue() < 0 || layer->getFreezeValue() == 1) && !layer->getDrawHidden())
			{
				return;
			}
		}
		else if(this->CommandLine == 67)
		{
			inModelSpace = false;
		}
		else if(this->CommandLine == 70)
		{
			closedPoly = this->GetLine();
		}
		// Z Coordinate (Static)
		if(this->CommandLine == 38)
		{
			xyz[2] = this->GetLine();
		}
		// X Coordinate
		else if(this->CommandLine == 10 || this->CommandLine == 11)
		{
			xyz[0] = this->GetLine();
		}
		// Y Coordinate
		else if(this->CommandLine == 20 || this->CommandLine == 21)
		{
			xyz[1] = this->GetLine();
			// Check to see if the object is in Model Space and needs to be scaled
			if(inModelSpace && AutoScale)
			{
				xyz[0] += this->XAdj;
				xyz[1] += this->YAdj;

				xyz[0] *= this->XScale;
				xyz[1] *= this->YScale;

				// Make sure we only scale the Z once, since it is static for the line
				if(zChanged == false)
				{
					xyz[2] *= this->ZScale;
					zChanged = true;
				}
			}
			block->getLWPolyLinePoints()->InsertPoint(pointId, xyz);
			block->getLWPolyLineProps()->InsertNextTuple1(layerProp);
			pointId++;
		}
		// Colour
		else if(this->CommandLine == 62)
		{
			layerProp = this->GetLine();
		}
		this->Read(file);
	}
	// Add the point IDs into the newly created cell
	int numPoints = block->getLWPolyLinePoints()->GetNumberOfPoints() - startPointId;
	if(closedPoly == 1.0)
	{
		block->getLWPolyLineCells()->InsertNextCell(numPoints+1);
	}
	else
	{
		block->getLWPolyLineCells()->InsertNextCell(numPoints);
	}
	for(int i = 0; i < numPoints; i++)
	{
		block->getLWPolyLineCells()->InsertCellPoint(i+startPointId);
	}
	if(closedPoly == 1.0)
	{
		// Insert first point again, to complete the line
		block->getLWPolyLineCells()->InsertCellPoint(startPointId);
	}
delete[] xyz;
}

void vtkDXFParser::ParseFaceBlock(ifstream* file, vtkStdString* name, vtkDXFObjectMap* layerList, vtkDXFBlock* block, bool AutoScale)
{
	this->Read(file);
	double* xyz = new double[3];
	xyz[0] = xyz[1] = xyz[2] = 0.0;
	bool layerFound = false;
	bool inModelSpace = true;
	double layerProp = 0;
	
	vtkDXFLayer* layer;
	int startPointId;
	int pointId;
	while(this->CommandLine != 0)
	{
		// Add colour properties to the property array
		if(this->CommandLine == 8 && !layerFound)
		{
			*name = this->ValueLine;
			layer = this->GetLayer(*name, layerList);
			//if layer not found, then we have a file with no layers defined, so use the default one
			if(layer == NULL)
			{
				layer = this->GetLayer("DEFAULT", layerList);
			}
			layerProp = layer->getLayerPropertyValue();
			startPointId = block->getSurfPoints()->GetNumberOfPoints();
			pointId = block->getSurfPoints()->GetNumberOfPoints();
			layerFound = true;

			// If the colour value is negative, the layer is invisible
			if( (layer->getLayerPropertyValue() < 0 || layer->getFreezeValue() == 1) && !layer->getDrawHidden())
			{
				return;
			}
		}
		else if(this->CommandLine == 67)
		{
			inModelSpace = false;
		}
		// X Coordinates
		else if(this->CommandLine >= 10 && this->CommandLine <= 14)
		{
			xyz[0] = this->GetLine();
		}
		// Y Coordinats
		else if(this->CommandLine >= 20 && this->CommandLine <= 24)
		{
			xyz[1] = this->GetLine();
		}
		// Z Coordinates
		else if(this->CommandLine >= 30 && this->CommandLine <= 34)
		{
			xyz[2] = this->GetLine();

			// Check to see if the object is in Model Space and needs to be scaled
			if(inModelSpace && AutoScale)
			{
				xyz[0] += this->XAdj;
				xyz[1] += this->YAdj;

				xyz[0] *= this->XScale;
				xyz[1] *= this->YScale;
				xyz[2] *= this->ZScale;
			}

			block->getSurfPoints()->InsertPoint(pointId, xyz);
			block->getSurfProps()->InsertNextTuple1(layerProp);
			pointId++;

			// Last point in the set of 3. Create the cell to hold our triangle data
			if(this->CommandLine == 32)
			{			  
			  //triangle has 3 points
			  block->getSurfCells()->InsertNextCell(3);
			}
		}
		// Colour
		else if(this->CommandLine == 62)
		{
			layerProp = this->GetLine();
		}
		this->Read(file);
	}
	// Add the point IDs into the newly created cell
	int numPoints = block->getSurfPoints()->GetNumberOfPoints() - startPointId;
	block->getSurfCells()->InsertNextCell(numPoints);
	for(int i = 0; i < numPoints; i++)
	{
		block->getSurfCells()->InsertCellPoint(i+startPointId);
	}
	
 delete[] xyz;
}

void vtkDXFParser::ParseTextBlock(ifstream* file, vtkStdString* name, vtkDXFObjectMap* layerList, bool mText, vtkDXFBlock* block, bool AutoScale)
{
	this->Read(file);
	vtkDXFLayer* layer;
	double* xyz = new double[3];
	xyz[0] = xyz[1] = xyz[2] = 0.0;
	double* offset = new double[3];
	// Initialize the offsets to 0.0, this eliminates problems that
	// will happen later on if offset is nto initialized.
	offset[0] = offset[1] = offset[2] = 0.0;
	// Set default text scale to 1.0 (0 makes things disappear)
	double scale = 1.0;
	// Set default text rotation
	double textRotation = 0.0;
	// Justification:
	// 0 = left, 1 = center, 2 = right
	vtkStdString justification = "0";
	// Length of the word/number to be justified
	int justLen = 0;
	// How many lines are in the text? (used for justification calculation)
	int numLine = 1;
	// Used to hold text data from multiple lines(CommandLine 3 and CommandLine 1)
	vtkStdString line = "";
	bool layerFound = false;
	bool inModelSpace = true;
	double layerProp = 0;

	vtkVectorText *text = vtkVectorText::New();
	// The transform and transform filter are used to manipulate
	// the text object after it is created.
	vtkTransform *transform = vtkTransform::New();
	vtkTransformFilter *tf = vtkTransformFilter::New();
	while(this->CommandLine != 0)
	{
		// Grab the current layer that we are adding data to
		if(this->CommandLine == 8 && !layerFound)
		{
			*name = this->ValueLine;		
			layer = this->GetLayer(*name, layerList);
			layerFound = true;
			layerProp = layer->getLayerPropertyValue();

			// If the colour value is negative, the layer is invisible
			if( (layer->getLayerPropertyValue() < 0 || layer->getFreezeValue() == 1) && !layer->getDrawHidden())
			{
				return;
			}
		}
		else if(this->CommandLine == 67)
		{
			inModelSpace = false;
		}
		// X Coordinate
		else if(this->CommandLine == 10)
		{
			xyz[0] = this->GetLine();
		}
		// X Alignment Offset
		else if(this->CommandLine == 11)
		{
			offset[0] = this->GetLine();
		}
		// Y Coordinate
		else if(this->CommandLine == 20)
		{
			xyz[1] = this->GetLine();
		}
		// Y Alignment Offset
		else if(this->CommandLine == 21)
		{
			offset[1] = this->GetLine();
		}
		// Z Coordinate
		else if(this->CommandLine == 30)
		{
			xyz[2] = this->GetLine();
		}
		// Z Alignment Offset
		else if(this->CommandLine == 31)
		{
			offset[2] = this->GetLine();
		}
		else if(this->CommandLine == 3)
		{
			line.append(this->ValueLine);
		}
		// Text data line
		else if(this->CommandLine == 1)
		{
			line.append(this->ValueLine);
			// Clean the AutoCAD junk from our text
			vtkStdString cleanline = this->CleanString(line, mText, numLine);
			// Use our nice, clean text
			text->SetText(cleanline);
			// Save the length of the text added, will be used later for jusification
			justLen = line.length();
		}
		// The 40 command line is used to set the scale
		else if(this->CommandLine == 40)
		{
			scale = this->GetLine();
			// Adjust scale
			// Check to see if the object is in Model Space and needs to be scaled
			if(inModelSpace && AutoScale)
			{
				scale *= this->XScale;
			}

			// Autodesk and Paraview do not agree on scale, this fixes that
			scale *= 0.85;
		}
		// Text justification
		else if(this->CommandLine == 72)
		{
			justification = this->TrimWhiteSpace(this->ValueLine);
		}
		// Text rotation
		else if(this->CommandLine == 50)
		{
			textRotation = this->GetLine();
		}
		// Colour
		else if(this->CommandLine == 62)
		{
			layerProp = this->GetLine();
		}
		this->Read(file);
	}

	// Set text justification
	if(justification == "1")
	{ //Center justification
		// MTEXT and TEXT elements need to aligned differently
		if(mText)
		{
			xyz[0] = xyz[0] - ((0.4*scale) * justLen/numLine);
		}
		else
		{
			xyz[0] = xyz[0] - ((0.1*scale) * (justLen/2));
		}
	}
	else if(justification == "2")
	{ // Right justification
		if(mText)
		{
			xyz[0] = xyz[0] - ((0.8*scale) * justLen/numLine);
		}
		else
		{
			xyz[0] = xyz[0] - ((0.1*scale) * (justLen));
		}
	}

	// If there are offsets available, then use them
	// These offsets allow the text to be "aligned" properly
	// There is still an issue with the text not being in 
	// the proper location. This is due to the text being 
	// shifted for the alignment.
	// To have the text in the proper position, the text 
	// must be grouped properly, and then the groups must
	// be shifted as a whole into the proper positions.
	// This is not possible with the way we are currently 
	// handling the text data.
	// *NOT USING* Causes TEXT to be offset improperly
	// and MTEXT to go all over the place
	/*if(mText == true)
	{
	if(offset[0] != 0.0)
	{
		xyz[0] = xyz[0] - (offset[0] - xyz[0]);
	}
	if(offset[1] != 0.0)
	{
		xyz[1] = xyz[1] - (offset[1] - xyz[1]);
	}
	if(offset[2] != 0.0)
	{
		xyz[2] = xyz[2] - (offset[2] - xyz[2]);
	}
	}*/

	// Check to see if the object is in Model Space and needs to be scaled
	if(inModelSpace && AutoScale)
	{
		xyz[0] += this->XAdj;
		xyz[1] += this->YAdj;

		xyz[0] *= this->XScale;
		xyz[1] *= this->YScale;
		xyz[2] *= this->ZScale;
	}

	// Do not change the order of Translate and Scale, doing
	// so will cause the text to be placed in the wrong position
	transform->Translate(xyz);
	transform->Scale(scale, scale, scale);
	// Objects are drawn on the Z plane, so we can only scale on the Z axis.
	transform->RotateZ(textRotation);
	   
	tf->SetInput(text->GetOutput());
	tf->SetTransform(transform);

	tf->Update();

	for(int i = 0; i < tf->GetPolyDataOutput()->GetPoints()->GetNumberOfPoints(); i++)
	{
		block->getTextProps()->InsertNextTuple1(layerProp);
	}
	block->getText()->AddItem(tf->GetPolyDataOutput());
	
	delete[] xyz;
	delete[] offset;
	tf->Delete();
	transform->Delete();
	text->Delete();
}

void vtkDXFParser::ParsePointsBlock(ifstream* file, vtkStdString* name, vtkDXFObjectMap* layerList, vtkDXFBlock* block, bool AutoScale)
{
	this->Read(file);
	double* xyz = new double[3];
	xyz[0] = xyz[1] = xyz[2] = 0.0;
	bool layerFound = false;
	bool inModelSpace = true;
	double layerProp = 0;
	
	vtkDXFLayer* layer;
	int startPointId;
	int pointId;
	while(this->CommandLine != 0)
	{
		// Add colour properties to the property array
		if(this->CommandLine == 8 && !layerFound)
		{
			*name = this->ValueLine;
			layer = this->GetLayer(*name, layerList);
			//if layer not found, then we have a file with no layers defined, so use the default one
			if(layer == NULL)
			{
				layer = this->GetLayer("DEFAULT", layerList);
			}
			layerProp = layer->getLayerPropertyValue();
			startPointId = block->getPointPoints()->GetNumberOfPoints();
			pointId = block->getPointPoints()->GetNumberOfPoints();

			// only define the layer once, as it is shared by all the objects in this section
			layerFound = true;

			// If the colour value is negative, the layer is invisible
			if( (layer->getLayerPropertyValue() < 0 || layer->getFreezeValue() == 1) && !layer->getDrawHidden())
			{
				return;
			}
		}
		else if(this->CommandLine == 67)
		{
			inModelSpace = false;
		}
		// X Coordinate
		else if(this->CommandLine == 10)
		{
			xyz[0] = this->GetLine();
		}
		// Y Coordinate
		else if(this->CommandLine == 20)
		{
			xyz[1] = this->GetLine();
		}
		// Z Coordinate
		else if(this->CommandLine == 30)
		{
			xyz[2] = this->GetLine();
			if(inModelSpace && AutoScale)
			{
				xyz[0] += this->XAdj;
				xyz[1] += this->YAdj;

				xyz[0] *= this->XScale;
				xyz[1] *= this->YScale;
				xyz[2] *= this->ZScale;
			}

			block->getPointPoints()->InsertPoint(pointId, xyz);
			block->getPointProps()->InsertNextTuple1(layerProp);
			block->getPointCells()->InsertNextCell(1);
			block->getPointCells()->InsertCellPoint(pointId);
			pointId++;
		}
		// Colour
		else if(this->CommandLine == 62)
		{
			layerProp = this->GetLine();
		}
		this->Read(file);
	}
	delete[] xyz;
}

void vtkDXFParser::ParseCircleBlock(ifstream* file, vtkStdString* name, vtkDXFObjectMap* layerList, vtkDXFBlock* block, bool AutoScale)
{
	this->Read(file);
	double* xyz = new double[3];
	xyz[0] = xyz[1] = xyz[2] = 0.0;
	double radius = 0.0;
	bool layerFound = false;
	bool inModelSpace = true;
	double layerProp = 0;
	
	vtkDXFLayer* layer;
	vtkDiskSource *disk = vtkDiskSource::New();
	// The transform and transform filter are used to manipulate
	// the text object after it is created.
	vtkTransform *transform = vtkTransform::New();
	vtkTransformFilter *tf = vtkTransformFilter::New();
	while(this->CommandLine != 0)
	{
		// Grab the current layer that we are adding data to
		if(this->CommandLine == 8 && !layerFound)
		{
			*name = this->ValueLine;
			layer = this->GetLayer(*name, layerList);
			layerFound = true;
			layerProp = layer->getLayerPropertyValue();

			// If the colour value is negative, the layer is invisible
			if( (layer->getLayerPropertyValue() < 0 || layer->getFreezeValue() == 1) && !layer->getDrawHidden())
			{
				return;
			}
		}
		else if(this->CommandLine == 67)
		{
			inModelSpace = false;
		}
		// X Coordinate
		else if(this->CommandLine == 10)
		{
			xyz[0] = this->GetLine();
		}
		// Y Coordinate
		else if(this->CommandLine == 20)
		{
			xyz[1] = this->GetLine();
		}
		// Z Coordinate
		else if(this->CommandLine == 30)
		{
			xyz[2] = this->GetLine();
		}
		// Colour
		else if(this->CommandLine == 62)
		{
			layerProp = this->GetLine();
		}
		// The 40 command line is used to set the scale
		else if(this->CommandLine == 40)
		{
			radius = this->GetLine();
			// Check to see if the object is in Model Space and needs to be scaled
			if(inModelSpace && AutoScale)
			{
				radius *= this->XScale;
			}

			double outerRadius = radius - (0.030*radius);
			disk->SetOuterRadius(radius);
			//disk->SetInnerRadius(radius - 0.035);
			if(radius < 1.5)
			{
				disk->SetInnerRadius(radius - 0.035);
			}
			else
			{
				disk->SetInnerRadius(outerRadius);
			}
		}
		this->Read(file);
	}
	disk->SetCircumferentialResolution(10);

	// Check to see if the object is in Model Space and needs to be scaled
	if(inModelSpace && AutoScale)
	{
		xyz[0] += this->XAdj;
		xyz[1] += this->YAdj;

		xyz[0] *= this->XScale;
		xyz[1] *= this->YScale;
		xyz[2] *= this->ZScale;
	}

	// Do not change the order of Translate and Scale, doing
	// so will cause the text to be placed in the wrong position
	transform->Translate(xyz);
	   
	tf->SetInput(disk->GetOutput());
	tf->SetTransform(transform);

	tf->Update();

	for(int i = 0; i < tf->GetPolyDataOutput()->GetPoints()->GetNumberOfPoints(); i++)
	{
		block->getCircleProps()->InsertNextValue(layer->getLayerPropertyValue());
	}

	block->getCircles()->AddItem(tf->GetPolyDataOutput());
	
	delete[] xyz;
	tf->Delete();
	transform->Delete();
	disk->Delete();
}

void vtkDXFParser::ParseArcBlock(ifstream* file, vtkStdString* name, vtkDXFObjectMap* layerList, vtkDXFBlock* block, bool AutoScale)
{
	this->Read(file);
	double* centerPoint = new double[3];
	centerPoint[0] = centerPoint[1] = centerPoint[2] = 0.0;
	double radius = 0;
	double beginAngle = 0;
	double endAngle = 0;
	bool layerFound = false;
	bool inModelSpace = true;
	double layerProp = 0;
	
	vtkDXFLayer* layer;
	int startPointId;
	int pointId;
	while(this->CommandLine != 0)
	{
		// Add colour properties to the property array
		if(this->CommandLine == 8 && !layerFound)
		{
			*name = this->ValueLine;
			layer = this->GetLayer(*name, layerList);
			//if layer not found, then we have a file with no layers defined, so use the default one
			if(layer == NULL)
			{
				layer = this->GetLayer("DEFAULT", layerList);
			}
			layerProp = layer->getLayerPropertyValue();
			startPointId = block->getArcPoints()->GetNumberOfPoints();
			pointId = block->getArcPoints()->GetNumberOfPoints();

			//only define the layer once, as it is shared by all the objects in this section
			layerFound = true;

			// If the colour value is negative, the layer is invisible
			if( (layer->getLayerPropertyValue() < 0 || layer->getFreezeValue() == 1) && !layer->getDrawHidden())
			{
				return;
			}
		}
		else if(this->CommandLine == 67)
		{
			inModelSpace = false;
		}
		// X Coordinate
		else if(this->CommandLine == 10)
		{
			centerPoint[0] = this->GetLine();
		}
		// Y Coordinate
		else if(this->CommandLine == 20)
		{
			centerPoint[1] = this->GetLine();
		}
		// Z Coordinate
		else if(this->CommandLine == 30)
		{
			centerPoint[2] = this->GetLine();
		}
		// Radius
		else if(this->CommandLine == 40)
		{
			radius = this->GetLine();
		}
		// Begin Angle
		else if(this->CommandLine == 50)
		{
			beginAngle = this->GetLine();
		}
		// End Angle
		else if(this->CommandLine == 51)
		{
			endAngle = this->GetLine();
		}
		// Colour
		else if(this->CommandLine == 62)
		{
			layerProp = this->GetLine();
		}
		this->Read(file);
	}
	double angleLength = endAngle - beginAngle;
	double x;
	double y;
	double z = centerPoint[2]; //always constant

	if(inModelSpace && AutoScale)
	{
		z *= this->ZScale;
	}
	
	double increment = 10.0; //default angular distance between pts, may be reduced depending on angleLength
	int numPts = angleLength / increment + 1;
	//for visibility purposes, we want at least 5 points.  this helps with arcs of short radian length.
	if(numPts < 5)
	{
		numPts = 5;
	}
	increment = angleLength / (numPts-1); //new increment value so that points are uniformily distributed
	int count = 0;
	double currentAngle;
	if(beginAngle < endAngle)
	{
		for(currentAngle = beginAngle; (currentAngle - endAngle) <= 0.000001; currentAngle += increment)
		{
			double cosResult = radius * cos(currentAngle * (PI / 180.0));
			double sinResult = radius * sin(currentAngle * (PI / 180.0));
			x = centerPoint[0] + cosResult;
			y = centerPoint[1] + sinResult;

			// Check to see if the object is in Model Space and needs to be scaled
			if(inModelSpace && AutoScale)
			{
				x += this->XAdj;
				y += this->YAdj;

				x *= this->XScale;
				y *= this->YScale;
			}

			block->getArcPoints()->InsertPoint(pointId, x, y, z);
			block->getArcProps()->InsertNextTuple1(layerProp);
			pointId++;
			count++;
		}
		block->getArcCells()->InsertNextCell(numPts);
	}
	else //arc crosses 360 degrees and therefore endAngle < beginAngle
	{
		double oldBegin = beginAngle;
		double oldEnd = endAngle;
		beginAngle = 180 - oldEnd;
		endAngle = 540 - oldBegin;
		angleLength = endAngle - beginAngle;
		increment = 10.0;
		numPts = angleLength / increment + 1;
		if(numPts < 5)
		{
			numPts = 5;
		}
		increment = angleLength / (numPts-1);
		for(currentAngle = beginAngle; (currentAngle - endAngle) <= 0.000001; currentAngle += increment)
		{
			x = centerPoint[0] - radius * cos(currentAngle * (PI / 180.0));
			y = centerPoint[1] + radius * sin(currentAngle * (PI / 180.0));

			// Check to see if the object is in Model Space and needs to be scaled
			if(inModelSpace && AutoScale)
			{
				x += this->XAdj;
				y += this->YAdj;

				x *= this->XScale;
				y *= this->YScale;
			}

			block->getArcPoints()->InsertPoint(pointId, x, y, z);
			block->getArcProps()->InsertNextTuple1(layerProp);
			pointId++;
			count++;
		}
		block->getArcCells()->InsertNextCell(numPts);
	}
	for(int i = 0; i < count; i++)
	{
		block->getArcCells()->InsertCellPoint(i+startPointId);
	}
	
	delete[] centerPoint;
}

void vtkDXFParser::ParseSolidBlock(ifstream* file, vtkStdString* name, vtkDXFObjectMap* layerList, vtkDXFBlock* block, bool AutoScale)
{
	this->Read(file);
	double* xyz = new double[3];
	xyz[0] = xyz[1] = xyz[2] = 0.0;
	// Used to keep track of our vertices
	double xyzList[4][3];
	xyzList[0][0] = xyzList[0][1] = xyzList[0][2] =
		xyzList[1][0] = xyzList[1][1] = xyzList[1][2] =
		xyzList[2][0] = xyzList[2][1] = xyzList[2][2] = 
		xyzList[3][0] = xyzList[3][1] = xyzList[3][2] = 0.0;
	int vertCount = 0;

	bool layerFound = false;
	bool inModelSpace = true;
	double layerProp = 0;

	vtkDXFLayer* layer;
	int startPointId;
	int pointId;
	while(this->CommandLine != 0)
	{
		// Add colour properties to the property array
		if(this->CommandLine == 8 && !layerFound)
		{
			*name = this->ValueLine;
			layer = this->GetLayer(*name, layerList);
			//if layer not found, then we have a file with no layers defined, so use the default one
			if(layer == NULL)
			{
				layer = this->GetLayer("DEFAULT", layerList);
			}
			layerProp = layer->getLayerPropertyValue();
			startPointId = block->getSolidPoints()->GetNumberOfPoints();
			pointId = block->getSolidPoints()->GetNumberOfPoints();

			//only define the layer once, as it is shared by all the objects in this section
			layerFound = true;

			// If the colour value is negative, the layer is invisible
			if( (layer->getLayerPropertyValue() < 0 || layer->getFreezeValue() == 1) && !layer->getDrawHidden())
			{
				return;
			}
		}
		else if(this->CommandLine == 67)
		{
			inModelSpace = false;
		}
		// X Coordinates
		else if(this->CommandLine >= 10 && this->CommandLine <= 13)
		{
			xyz[0] = this->GetLine();
		}
		// Y Coordinates
		else if(this->CommandLine >= 20 && this->CommandLine <= 23)
		{
			xyz[1] = this->GetLine();
		}
		// Z Coordinates
		else if(this->CommandLine >= 30 && this->CommandLine <= 33)
		{
			xyz[2] = this->GetLine();
			if(inModelSpace && AutoScale)
			{
				xyz[0] += this->XAdj;
				xyz[1] += this->YAdj;

				xyz[0] *= this->XScale;
				xyz[1] *= this->YScale;
				xyz[2] *= this->ZScale;
			}
			xyzList[vertCount][0] = xyz[0];
			xyzList[vertCount][1] = xyz[1];
			xyzList[vertCount][2] = xyz[2];
			vertCount++;
		}
		// Colour
		else if(this->CommandLine == 62)
		{
			layerProp = this->GetLine();
		}
		this->Read(file);
	}
	if(layerFound)
	{
		if(xyzList[2][0] == xyzList[3][0] && xyzList[2][1] == xyzList[3][1] && xyzList[2][2] == xyzList[3][2])
		{
			vertCount--;
		}
		
		for(int pt = 0; pt < vertCount; pt++)
		{
			block->getSolidPoints()->InsertPoint(pointId, xyzList[pt][0], xyzList[pt][1], xyzList[pt][2]);
			block->getSolidProps()->InsertNextTuple1(layerProp);
			pointId++;
		}
		// Add the point IDs into the newly created cell
		int numPoints = block->getSolidPoints()->GetNumberOfPoints() - startPointId;
		for(int i = 0; i < numPoints-2; i++)
		{
			block->getSolidCells()->InsertNextCell(3);
			block->getSolidCells()->InsertCellPoint(i+startPointId);
			block->getSolidCells()->InsertCellPoint(i+1+startPointId);
			block->getSolidCells()->InsertCellPoint(i+2+startPointId);
		}
	}

delete[] xyz;
}