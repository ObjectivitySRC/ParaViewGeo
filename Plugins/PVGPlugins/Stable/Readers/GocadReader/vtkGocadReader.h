/*Robert Maynard
MIRARCO 
Aug 18 2008

Interface to VTK for loading of GoCad group and polydata files
Output is always multiblock, since the gocad files can have multiple
objects inside a single file
*/


#ifndef __vtkGocadReader_h
#define __vtkGocadReader_h

#include "vtkMultiBlockDataSetAlgorithm.h"


class vtkInternalGocadGroups;
class vtkStdString;

//BTX
class GocadColorMap;
//ETX


class VTK_EXPORT vtkGocadReader : public vtkMultiBlockDataSetAlgorithm
{
public:
  //BTX
  enum OBJECT_MODE { CLOSE_GROUP, OPEN_GROUP, TSURF, PLINE, VSET, TSOLID, VOXET, SGRID, NOT_SUPPORTED };
  //ETX
  static vtkGocadReader* New();
  vtkTypeRevisionMacro(vtkGocadReader,vtkMultiBlockDataSetAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  vtkSetStringMacro(FileName);  // SetFileName();
  vtkGetStringMacro(FileName);  // SetFileName();

	vtkSetVector4Macro(RGB,double);
	vtkGetVector4Macro(RGB,double);

protected:
  vtkGocadReader();
  ~vtkGocadReader();

  //file reading      
  virtual bool ReadFile( );
  virtual int ReadGObject( vtkStdString *name );
  virtual void ReadHeader( vtkStdString *name );
	virtual void ParseHeaderLine( vtkStdString *name );
  virtual void ReadGroupHeader( vtkStdString *name );
  
  //adding groups / objects for the block
  void CreateBlock( vtkStdString *name );
  void CreateObject( vtkStdString *name, int mode);
  void AppendBlock( );
  
  //file IO
  virtual bool OpenFile( const char* filename );
  virtual void CloseFile(  ); 
  virtual void NextLine( );
            
  //virtual int RequestInformation(vtkInformation* request, vtkInformationVector** inputVector, vtkInformationVector* outputVector);
  virtual int RequestData(vtkInformation* request, vtkInformationVector** inputVector, vtkInformationVector* outputVector);
  
  char* FileName;
  
	//rgb
	double *RGB;

  //file IO members
  vtkStdString *Line;
  ifstream *File;

  vtkInternalGocadGroups *Internal;
	GocadColorMap *ColorMap;
    
private:
  vtkGocadReader(const vtkGocadReader&);  // Not implemented.
  void operator=(const vtkGocadReader&);  // Not implemented.
};

#endif  
