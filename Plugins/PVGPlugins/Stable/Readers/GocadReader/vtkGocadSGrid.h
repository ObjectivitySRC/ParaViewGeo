// .NAME vtkGocadSGrid from VTK\IO\vtkSimplePointsReader & vtkLocalConeSource - Example ParaView Plugin Source
// .SECTION Description
// vtkGocadSGrid is a subclass of vtkDataObjectAlgorithm to read Gocad SGrid Files

#ifndef __vtkGocadSGrid_h
#define __vtkGocadSGrid_h

#include "vtkInformation.h"
#include <vtkDataReader.h>

#include "vtkGocadAtomic.h"

//BTX
class vtkStructuredGrid;
class vtkInternalMap;
class vtkInternalProps;
//ETX

#include <string>
using namespace std;

class VTK_EXPORT vtkGocadSGrid : public vtkGocadAtomic
{
public:
  static vtkGocadSGrid* New();
  vtkTypeRevisionMacro(vtkGocadSGrid,vtkGocadAtomic);

protected:
  vtkGocadSGrid();
  ~vtkGocadSGrid();

  virtual int RequestData(vtkInformation *, vtkInformationVector **,
                          vtkInformationVector *);

	void CreateStructure(vtkStructuredGrid *output, vtkInformation *outInfo, vtkPoints *points );

	void ReadSGridProperties(vtkStringArray* propertyNames, vtkStringArray* propertyFiles,
		vtkStringArray* asciiFiles, vtkStructuredGrid *output);

	void ApplyASCIIProperties(vtkStringArray* propertyNames, vtkStringArray* propertyFiles,
		vtkStringArray* asciiFiles, int propertyCount, int* propertySize, vtkStructuredGrid *output);

	void ApplyBinaryProperties(vtkStringArray* propertyNames, vtkStringArray* propertyFiles,
		vtkStringArray* asciiFiles, int propertyCount, int* propertySize, vtkStructuredGrid *output);

	void ParseAxis( double axis[3] );
	void ParseAxis( int axis[3] );

private:
	// false = point properties, true = cell properties
	bool propertyCells;

	// Are the point coords and properties in an ASCII file?
	bool isASCIIFile;

	vtkStdString ASCIIFilePath;

};

#endif
