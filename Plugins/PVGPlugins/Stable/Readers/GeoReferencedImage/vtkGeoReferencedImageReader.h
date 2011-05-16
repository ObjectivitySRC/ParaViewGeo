// .NAME vtkGeoReferencedImageReader 
// By: Matthew Livingstone
// Uses libtiff and geotiff libraries from http://trac.osgeo.org/geotiff/
// Reads standard .tiff, .jpg and .png files.
// Also reads GeoTIFF (with both embedded info and external text files).
// Supports .tfw files for any .tiff, .jpg or .png

#ifndef __vtkGeoReferencedImageReader_h
#define __vtkGeoReferencedImageReader_h

#include "vtkPolyDataAlgorithm.h"

class VTK_EXPORT vtkGeoReferencedImageReader : public vtkPolyDataAlgorithm
{
public:
  static vtkGeoReferencedImageReader* New();
  vtkTypeRevisionMacro(vtkGeoReferencedImageReader,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

	vtkSetStringMacro(FileName);  // SetFileName();
	vtkGetStringMacro(FileName); //GetFileName()

	vtkSetMacro(NorthernHemisphere, int);
	vtkGetMacro(NorthernHemisphere, int);

protected:
  vtkGeoReferencedImageReader();
  ~vtkGeoReferencedImageReader();

	int RequestInformation(vtkInformation* request,
                  vtkInformationVector** inputVector,
                  vtkInformationVector* outputVector);

  int RequestData(vtkInformation* request,
                  vtkInformationVector** inputVector,
                  vtkInformationVector* outputVector);

	void ParseTFW( double adfCoeff[6], bool &YInversion );

	void FindFileInfo();

	bool Flip;

private:
	vtkStdString FileExt;
  vtkStdString FileShortName;
  vtkStdString FilePath;

	int NorthernHemisphere;

	bool TFWFound;

	char* FileName; //name of file (full path), useful for obtaining object names

	vtkGeoReferencedImageReader(const vtkGeoReferencedImageReader&);  // Not implemented.
  void operator=(const vtkGeoReferencedImageReader&);  // Not implemented.

};

#endif
