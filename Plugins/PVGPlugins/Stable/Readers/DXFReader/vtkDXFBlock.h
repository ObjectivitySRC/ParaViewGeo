// Block object for DXF layers
// By: Eric Daoust && Matthew Livingstone

#ifndef __vtkDXFBlock_h
#define __vtkDXFBlock_h

#include "vtkDoubleArray.h"
#include "vtkDXFLayer.h"
#include "vtkCellArray.h"
#include <vtksys/ios/sstream>

class vtkCollection;

class VTK_EXPORT vtkDXFBlock : public vtkDXFLayer
{
public:
	static vtkDXFBlock* New();
	vtkTypeRevisionMacro(vtkDXFBlock,vtkObject);
	void PrintSelf(ostream& os, vtkIndent indent);

	double getBlockPropertyValue() {return this->blockPropertyValue;}
	double* getBlockTransform() {return this->blockTransform;}
	double* getBlockScale() {return this->blockScale;}
	vtkStdString getParentLayer() {return this->parentLayer;}
	bool getDrawBlock() {return this->drawBlock;}

	void setParentLayer(vtkStdString name) {this->parentLayer = name;}
	void setBlockPropertyValue(double value) {this->blockPropertyValue = value;}
	void setBlockTransform(double* transform) {this->blockTransform = transform;}
	void setBlockScale(double* Scale) {this->blockScale = Scale;}

	void setDrawBlock(bool option) {this->drawBlock = option;}

	void CopyFrom(vtkDXFBlock* block);

protected:
	vtkDXFBlock();
	~vtkDXFBlock();

	double blockPropertyValue;
	double* blockScale;
	double* blockTransform;
	vtkStdString parentLayer;
	bool drawBlock;

};
#endif