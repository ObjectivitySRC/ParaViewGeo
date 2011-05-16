/*Robert Maynard
MIRARCO 
Aug 18 2008

vtkGocadAtomic is the base class for all the sub types of gocad polygonal objects.
It handles the majority of the interfacing with the vtk framework plus file system control
*/


#ifndef __vtkGocadAtomic_h
#define __vtkGocadAtomic_h

#include "vtkDataSetAlgorithm.h"
#include "vtkStdString.h"


class vtkInternalMap;
class vtkInternalProps;
class vtkPoints;
class vtkCellArray;
class vtkStdString;
class vtkStringArray;

class VTK_EXPORT vtkGocadAtomic : public vtkDataSetAlgorithm
{

public:
  static vtkGocadAtomic* New();
  vtkTypeRevisionMacro(vtkGocadAtomic,vtkDataSetAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  //set the file pointer
  virtual void SetFile( ifstream *file );
	// Extract and set the path of the open file
	virtual void SetFilePath( char* FileName );	
  
protected:
  vtkGocadAtomic();
  ~vtkGocadAtomic();
    
  //the request data will call Read that does the real work
  virtual int RequestDataObject(vtkInformation* request, vtkInformationVector** inputVector, vtkInformationVector* outputVector);
  virtual int RequestData(vtkInformation* request, vtkInformationVector** inputVector, vtkInformationVector* outputVector);
  
  
  virtual int FillOutputPortInformation(int port, vtkInformation* info); 
  
  //basic reading methods for the g0cad file
  virtual bool ReadProperties( );
  virtual bool ReadPoints( vtkPoints *points, vtkStdString endCondition );
  virtual bool ReadCells( vtkCellArray *cells, int cellSize, vtkStdString cellName, int cellNameLength, vtkStringArray *endOfCell);  
  
  //basic IO methods
  virtual void NextLine( );  
  virtual void ParseLine(vtkStdString line, int length, double *values);
  virtual void ParseLine(vtkStdString line, int length, int *values);
	virtual void ParseLine(vtkStdString line, int length, vtkStringArray *values);
  virtual void SetFileProperties(vtkStdString line);
	virtual void SetFileCellProperties(vtkStdString line);
  virtual void ParseASCIIFileLine(vtkStdString line, int length, vtkStringArray *values);
  
  // Adding properties from outside of Atomic
  bool InsertPropertyValue( unsigned int index, double value );
	bool InsertPropertyTuple( unsigned int index, double* tuple );
  int GetPropertySize();
	int GetPropertyTupleCount( int pos );

	bool InsertCellPropertyValue( unsigned int index, double value );
	bool InsertCellPropertyTuple( unsigned int index, double* tuple );
  int GetCellPropertySize();
  
  //Methods to access the InternalMap 
  //usefull for subclasses
  void SetPointMapID( int oldID, int newID );
  int GetPointMapID( int oldID );
   
     
  //need a way to store the data type we are going to output  
  int DataType;
	
	// Hold the path of the currently open file
	vtkStdString FilePath;
  
  //since we are reading only a section of the file, we use a pointer to a currently opened file
  ifstream* File;
  
  //current line in the file
  vtkStdString *Line;
  
  //lookup table for the points, since the gocad file can have dirty id's
  vtkInternalMap *PointMap;
  
  //collection for the properties the object has
  vtkInternalProps *Properties;
	vtkInternalProps *CellProperties;
  
  //bool to confirm we have properties
  bool HaveProperties;
	  
private:
  vtkGocadAtomic(const vtkGocadAtomic&);  // Not implemented.
  void operator=(const vtkGocadAtomic&);  // Not implemented.
};
#endif