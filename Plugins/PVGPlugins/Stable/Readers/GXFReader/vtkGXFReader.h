#ifndef _vtkGXFReader_h
#define _vtkGXFReader_h
#include "vtkImageAlgorithm.h"
#include "vtkStdString.h"
#include <iostream>
#include <sstream>

//BTX
struct GXFDataBlock
{
	vtkstd::string Name;
	vtkstd::stringstream Buffer;
};
//ETX
class vtkPoints;
class vtkFloatArray;


class VTK_EXPORT vtkGXFReader : public vtkImageAlgorithm
{
public:
  static vtkGXFReader *New();
  vtkTypeRevisionMacro(vtkGXFReader,vtkImageAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);
  
	vtkSetStringMacro(FileName);
  vtkGetStringMacro(FileName);
  	  
	int CanReadFile(const char* fname);

protected:
  vtkGXFReader();
  ~vtkGXFReader();

  virtual int RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);  

	bool OpenFile( const char* filename );
	void CloseFile( );
	void NextDataBlock( GXFDataBlock *block, bool ParseGrid=false ); //bool allows us to not read grid during RI to save time

	//BTX	
	void TrimWhiteSpaces( vtkstd::string &str );
	void ReplaceCommas( vtkstd::string &str );
	//ETX

	void CreateProperty( GXFDataBlock *block, vtkFloatArray *prop);

	//changes the passed in block to be uncompressed
	void UncompressBlock( GXFDataBlock* block );
	double ConvertBaseNinety( GXFDataBlock* block );
	
	//properties needed to handle gxf
  double DummyValue; //dummy value for unfilled points
  int GType; //the compression level of the file  	

	int Points; //number of points in each row
  int Rows; //number of rows in the file ( gxf is 2d )

	double RowSeperation; //space between each row
	double PointSeperation; //space between each point

	double Rotation; //rotation of the dataset from Base Coordiante System
	double Transform[2]; //used to transform real data to integers for base-90 compression specified by the #GTYPE object

	int Sense; //data order
	
	//BTX
	vtkstd::string Title; //name of the property
	//ETX

	double XOrigin; //bottom left corner of the grid in the Base Coordinate System
	double YOrigin; 
  	
	//file IO members	
	char *FileName;
  ifstream *File;
	int GridBlockPosition;		

private:
  vtkGXFReader(const vtkGXFReader&);
  void operator = (const vtkGXFReader&);
};
#endif
