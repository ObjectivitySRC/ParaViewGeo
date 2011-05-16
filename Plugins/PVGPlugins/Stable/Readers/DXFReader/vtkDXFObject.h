// .NAME vtkDXFObject 
// By: Eric Daoust && Matthew Livingstone
// .SECTION Description
// vtkDXFObject is a subclass of vtkMultiBlockDataSetAlgorithm to read DXF Files

#ifndef __vtkDXFObject_h
#define __vtkDXFObject_h

#include "vtkInformation.h"
#include "vtkStdString.h"
#include <vtkMultiBlockDataSetAlgorithm.h>

class VTK_EXPORT vtkDXFObject : public vtkMultiBlockDataSetAlgorithm
{
public:
  static vtkDXFObject* New();
  vtkTypeRevisionMacro(vtkDXFObject,vtkMultiBlockDataSetAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  vtkSetStringMacro(FileName);  // SetFileName();
	// Set/Get for option to draw points
	vtkSetMacro(DrawPoints, bool);
	vtkGetMacro(DrawPoints, bool);
	// Set/Get for option to drawn frozen/invisible layers
	vtkSetMacro(DrawHidden, bool);
	vtkGetMacro(DrawHidden, bool);
	// MLivingstone:
	// Set/Get for option to scale all entities
	// This was added for use in special cases, when the EXACT units are needed from the dxf file.
	// If the dxf has both model space and paper space elements, disabling this will distort the drawing
	vtkSetMacro(AutoScale, bool);
	vtkGetMacro(AutoScale, bool);

protected:
  vtkDXFObject();
  ~vtkDXFObject();

  int RequestData(vtkInformation* request,
                  vtkInformationVector** inputVector,
                  vtkInformationVector* outputVector);

  void Read(ifstream* file); //read one pair of entries from file

	int layerExists;
	int type;

private:
	vtkStdString GetObjectName();

	const char* FileName; //name of file (full path), useful for obtaining object names
	int CommandLine; //text of current command line
	vtkStdString ValueLine; //text of current value line

	// Set to enable/disable point drawing
	bool DrawPoints;
	// Set to enable/disable drawing of frozen/invisible layers
	bool DrawHidden;
	// MLivingstone:
	// Set to enable/disable scaling of entities
	// This was added for use in special cases, when the EXACT units are needed from the dxf file.
	// If the dxf has both model space and paper space elements, disabling this will distort the drawing
	bool AutoScale;
};

#endif
