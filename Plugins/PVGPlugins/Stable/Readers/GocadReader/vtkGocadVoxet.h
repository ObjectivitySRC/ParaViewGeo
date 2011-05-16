/*Robert Maynard
MIRARCO 
Aug 28 2008

Re-written:
Matthew Livingstone
Feb 2009

vtkGocadVoxet handles reading triangle based gocad objects
*/


#ifndef __vtkGocadVoxet_h
#define __vtkGocadVoxet_h

#include "vtkGocadAtomic.h"

//BTX
class vtkStructuredGrid;
class vtkInternalMap;
class vtkInternalProps;
//ETX

class VTK_EXPORT vtkGocadVoxet : public vtkGocadAtomic
{
public:
  static vtkGocadVoxet* New();
  vtkTypeRevisionMacro(vtkGocadVoxet,vtkGocadAtomic);
  
protected:
  vtkGocadVoxet();
  ~vtkGocadVoxet();

  //the request data will call Read that does the real work
  virtual int RequestData(vtkInformation* request, vtkInformationVector** inputVector,
		vtkInformationVector* outputVector);

	void CreateStructure(vtkStructuredGrid *output, vtkInformation *outInfo, vtkPoints *points );

	void ReadVoxetProperties(vtkStringArray* propertyNames, vtkStringArray* propertyFiles,
		vtkStringArray* asciiFiles);

	void ApplyASCIIProperties(vtkStringArray* propertyNames, vtkStringArray* propertyFiles,
		vtkStringArray* asciiFiles, int propertyCount, int* propertySize, int* propertyNoDataValues);

	void ApplyBinaryProperties(vtkStringArray* propertyNames, vtkStringArray* propertyFiles,
		vtkStringArray* asciiFiles, int propertyCount, int* propertySize, int* propertyNoDataValues);

	void ParseAxis( double axis[3] );
	void ParseAxis( int axis[3] );
  
private:
  vtkGocadVoxet(const vtkGocadVoxet&);  // Not implemented.
  void operator=(const vtkGocadVoxet&);  // Not implemented.	
};
#endif
