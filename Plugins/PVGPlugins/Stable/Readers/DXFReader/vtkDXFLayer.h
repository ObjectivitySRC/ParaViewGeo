// Layer object for DXF layers
// By: Eric Daoust && Matthew Livingstone

#ifndef __vtkDXFLayer_h
#define __vtkDXFLayer_h

#include "vtkDoubleArray.h"
#include "vtkCellArray.h"
#include <vtksys/ios/sstream>

class vtkCollection;

class VTK_EXPORT vtkDXFLayer : public vtkObject
{
public:
	static vtkDXFLayer* New();
	vtkTypeRevisionMacro(vtkDXFLayer,vtkObject);
	void PrintSelf(ostream& os, vtkIndent indent);

	vtkCollection* ParseData(const char* name);

	vtkPoints* getArcPoints() {return this->arcPoints;};
	vtkCellArray* getArcCells() {return this->arcCells;};

	vtkPoints* getPointPoints() {return this->pointPoints;};
	vtkCellArray* getPointCells() {return this->pointCells;};

	vtkPoints* getLinePoints() {return this->linePoints;};
	vtkCellArray* getLineCells() {return this->lineCells;};

	vtkPoints* getPolyLinePoints() {return this->polyLinePoints;};
	vtkCellArray* getPolyLineCells() {return this->polyLineCells;};

	vtkPoints* getLWPolyLinePoints() {return this->lwPolyLinePoints;};
	vtkCellArray* getLWPolyLineCells() {return this->lwPolyLineCells;};

	vtkPoints* getSurfPoints() {return this->surfPoints;};
	vtkCellArray* getSurfCells() {return this->surfCells;};

	vtkPoints* getSolidPoints() {return this->solidPoints;};
	vtkCellArray* getSolidCells() {return this->solidCells;};

	vtkCollection* getText() {return this->textList;}
	vtkCollection* getCircles() {return this->circleList;}

	vtkDoubleArray* getPointProps() {return this->pointProps;}
	vtkDoubleArray* getLineProps(){return this->lineProps;}
	vtkDoubleArray* getPolyLineProps(){return this->polyLineProps;}
	vtkDoubleArray* getLWPolyLineProps(){return this->lwPolyLineProps;}
	vtkDoubleArray* getSurfProps(){return this->surfProps;}
	vtkDoubleArray* getCircleProps(){return this->circleProps;}
	vtkDoubleArray* getTextProps(){return this->textProps;}
	vtkDoubleArray* getArcProps() {return this->arcProps;}
	vtkDoubleArray* getSolidProps(){return this->solidProps;}

	vtkCollection* getBlockList(){return this->blockList;}

	vtkStdString getName(){return this->name;}
	double getLayerPropertyValue(){return this->layerPropertyValue;}
	double getFreezeValue(){return this->freezeValue;}
	bool getDrawHidden(){return this->drawHidden;}


	void ShallowCopy(vtkDXFLayer *object);

	void setName(vtkStdString objName){this->name = objName;}
	void setLayerPropertyValue(double value) {this->layerPropertyValue = value;}
	void setFreezeValue(double value) {this->freezeValue = value;}
	void setDrawHidden(bool value) {this->drawHidden = value;}
	vtkDXFLayer* GetLayer(vtkStdString name, vtkCollection* layerList);


protected:
	vtkDXFLayer();
	~vtkDXFLayer();

	vtkStdString name;
	double layerPropertyValue;
	// Used to store value if layer is frozen or not
	int freezeValue;
	// Used to determine if we are drawing frozen/invisible layers
	bool drawHidden;
	vtkPoints *arcPoints;
	vtkCellArray *arcCells;

	vtkPoints *pointPoints;
	vtkCellArray *pointCells;

	vtkPoints *polyLinePoints;
	vtkCellArray *polyLineCells;

	vtkPoints *lwPolyLinePoints;
	vtkCellArray *lwPolyLineCells;

	vtkPoints *linePoints;
	vtkCellArray *lineCells;

	vtkPoints *surfPoints;
	vtkCellArray *surfCells;

	vtkPoints *solidPoints;
	vtkCellArray *solidCells;

	vtkDoubleArray *pointProps;
	vtkDoubleArray *polyLineProps;
	vtkDoubleArray *lwPolyLineProps;
	vtkDoubleArray *lineProps;
	vtkDoubleArray *surfProps;
	vtkDoubleArray *circleProps;
	vtkDoubleArray *textProps;
	vtkDoubleArray *arcProps;
	vtkDoubleArray *solidProps;

	vtkCollection *textList;
	vtkCollection *circleList;

	int layerExists;
	double currElevation;
	// Scale used for blocks
	double scale;
	int largeUnits;
	double textRotation;

	// Used for BLOCK data
	vtkCollection *blockList;
};
#endif