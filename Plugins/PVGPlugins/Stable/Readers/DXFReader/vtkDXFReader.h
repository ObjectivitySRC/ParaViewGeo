// .NAME vtkDXFReader 
// By: Robert Maynard && Matthew Livingstone && Eric Daoust
// .SECTION Description
// vtkDXFReader is a subclass of vtkMultiBlockDataSetAlgorithm to read DXF Files
//used a wrapper to get rid of weird memory leaks

#ifndef __vtkDXFReader_h
#define __vtkDXFReader_h

#include "vtkMultiBlockDataSetAlgorithm.h"

class VTK_EXPORT vtkDXFReader : public vtkMultiBlockDataSetAlgorithm
{
public:
  static vtkDXFReader* New();
  vtkTypeRevisionMacro(vtkDXFReader,vtkMultiBlockDataSetAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  vtkSetStringMacro(FileName);  // SetFileName();
	// Set/Get for option to draw points
	vtkSetMacro(DrawPoints, bool);
	vtkGetMacro(DrawPoints, bool);
	// Set/Get for option to drawn frozen/invisible layers
	vtkSetMacro(DrawHidden, bool);
	vtkGetMacro(DrawHidden, bool);
	// Set/Get for option to scale all entities
	vtkSetMacro(AutoScale, bool);
	vtkGetMacro(AutoScale, bool);

protected:
  vtkDXFReader();
  ~vtkDXFReader();

  int RequestData(vtkInformation* request,
                  vtkInformationVector** inputVector,
                  vtkInformationVector* outputVector);

	int layerExists;
	int type;

private:

	const char* FileName; //name of file (full path), useful for obtaining object names

	// Set to enable/disable point drawing
	bool DrawPoints;
	// Set to enable/disable drawing of frozen/invisible layers
	bool DrawHidden;
	// Set to enable/disable scaling of entities
	bool AutoScale;
};

#endif
