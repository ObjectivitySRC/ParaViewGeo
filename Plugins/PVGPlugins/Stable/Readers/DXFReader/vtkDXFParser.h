// Layer object for DXF layers
// By: Eric Daoust && Matthew Livingstone

#ifndef __vtkDXFParser_h
#define __vtkDXFParser_h

#include "vtkDoubleArray.h"
#include "vtkCellArray.h"
#include <vtksys/ios/sstream>
#include "vtkDXFLayer.h"
#include "vtkDXFBlock.h"
#include "vtkDXFObjectMap.h"

class vtkCollection;

class VTK_EXPORT vtkDXFParser : public vtkObject
{
public:
	static vtkDXFParser* New();
	vtkTypeRevisionMacro(vtkDXFParser,vtkObject);
	void PrintSelf(ostream& os, vtkIndent indent);

	void ApplyProperties();
	void Read(ifstream* file); //read one pair of entries from file

	vtkDXFObjectMap* ParseData(const char* name, bool DrawHidden, bool AutoScale);

	vtkDXFLayer* GetLayer(vtkStdString name, vtkDXFObjectMap* layerList);
	vtkDXFBlock* GetBlock(vtkStdString name, vtkDXFObjectMap* blockList);

	char* TrimWhiteSpace(vtkStdString input);
	int getIntFromLine(const char* line);
	vtkStdString getStringFromLine(const char* line);

	double getXScale(){return this->XScale;};
	double getYScale(){return this->YScale;};
	double getZScale(){return this->ZScale;};

protected:
	vtkDXFParser();
	~vtkDXFParser();

	vtkStdString name;
	double layerPropertyValue;
	
	void ParseLine(ifstream* file, vtkStdString* name, vtkDXFObjectMap* layerList, bool AutoScale);
  void ParseFace(ifstream* file, vtkStdString* name, vtkDXFObjectMap* layerList, bool AutoScale);
  void ParsePoints(ifstream* file, vtkStdString* name, vtkDXFObjectMap* layerList, bool AutoScale);
  void ParsePolyLine(ifstream* file, vtkStdString* name, vtkDXFObjectMap* layerList, bool firstPoint, bool AutoScale);
  void ParseLWPolyLine(ifstream* file, vtkStdString* name, vtkDXFObjectMap* layerList, bool AutoScale);
	void ParseText(ifstream* file, vtkStdString* name, vtkDXFObjectMap* layerList, bool mText, bool AutoScale);
	void ParseCircle(ifstream* file, vtkStdString* name, vtkDXFObjectMap* layerList, bool AutoScale);
	void ParseArc(ifstream* file, vtkStdString* name, vtkDXFObjectMap* layerList, bool AutoScale);
	void ParseSolid(ifstream* file, vtkStdString* name, vtkDXFObjectMap* layerList, bool AutoScale);

	void ParseLineBlock(ifstream* file, vtkStdString* name, vtkDXFObjectMap* layerList, vtkDXFBlock* block, bool AutoScale);
	void ParseFaceBlock(ifstream* file, vtkStdString* name, vtkDXFObjectMap* layerList, vtkDXFBlock* block, bool AutoScale);
	void ParsePointsBlock(ifstream* file, vtkStdString* name, vtkDXFObjectMap* layerList, vtkDXFBlock* block, bool AutoScale);
	void ParsePolyLineBlock(ifstream* file, vtkStdString* name, vtkDXFObjectMap* layerList, bool firstPoint, vtkDXFBlock* block, bool AutoScale);
	void ParseLWPolyLineBlock(ifstream* file, vtkStdString* name, vtkDXFObjectMap* layerList, vtkDXFBlock* block, bool AutoScale);
	void ParseTextBlock(ifstream* file, vtkStdString* name, vtkDXFObjectMap* layerList, bool mText, vtkDXFBlock* block, bool AutoScale);
	void ParseCircleBlock(ifstream* file, vtkStdString* name, vtkDXFObjectMap* layerList, vtkDXFBlock* block, bool AutoScale);
	void ParseArcBlock(ifstream* file, vtkStdString* name, vtkDXFObjectMap* layerList, vtkDXFBlock* block, bool AutoScale);
	void ParseSolidBlock(ifstream* file, vtkStdString* name, vtkDXFObjectMap* layerList, vtkDXFBlock* block, bool AutoScale);
	void ParseInsert(ifstream* file, vtkStdString* name, vtkDXFObjectMap* layerList, bool AutoScale);
	void ParseDimension(ifstream* file, vtkStdString* name, vtkDXFObjectMap* layerList, bool AutoScale);

	void ParseColorTable(ifstream* file, vtkDXFObjectMap* layerList, bool DrawHidden);
	void ParseBlockRecords(ifstream* file);
	void ParseHeader(ifstream* file);
	void ParseViewPort(ifstream* file);

	vtkStdString CleanString(vtkStdString &line, bool mText, int &numLine);
	double GetLine();

	// Used to determine if a layer has been found while parsing the file
	int layerExists;
  // Text scale
	double scale;
	// Elevation for polylines
	double currElevation;
	
	// Scale used for scaling ALL lines in *MODEL_SPACE
	double LineTypeScale;
	double XScale;
	double YScale;
	double ZScale;
	// Used to calculate the scaling values based on the extents of the
	// model space and paper space
	double ExtMinX;
	double ExtMaxX;
	double PExtMinX;
	double PExtMaxX;
	double ExtMinY;
	double ExtMaxY;
	double PExtMinY;
	double PExtMaxY;
	double ExtMinZ;
	double ExtMaxZ;
	double PExtMinZ;
	double PExtMaxZ;
	double XAdj;
	double YAdj;
	// Used for viewport information
	double ViewX;
	double ViewY;
	double ViewZoom;

	std::stringstream textString;

	// Used to ensure that BLOCKS should exist.
	bool blockRecordExists;

private:
	int CommandLine; //text of current command line
	vtkStdString ValueLine; //text of current value line

	// Used for BLOCK data
	vtkDXFObjectMap *blockList;
};
#endif