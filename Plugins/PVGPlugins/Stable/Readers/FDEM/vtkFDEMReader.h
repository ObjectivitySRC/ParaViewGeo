// .NAME vtkFDEMReader 
// .SECTION Description
// vtkFDEMReader is a subclass of vtkStructuredPointsReader
// The purpose of the reader is to support file reading of the files 
// that where built by the Y program and are normally displayed by the program called M
#ifndef __vtkFDemReader_h
#define __vtkFDemReader_h
#define VTK_FDEM_BLOCK_SIZE 100
#define VTK_FDEM_CODE_BASE 90

#include "vtkMultiBlockDataSetAlgorithm.h"

class VTK_EXPORT vtkFDEMReader : public vtkMultiBlockDataSetAlgorithm
{
public:
  static vtkFDEMReader* New();
  vtkTypeRevisionMacro(vtkFDEMReader,vtkMultiBlockDataSetAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  vtkSetStringMacro(FileName);  // SetFileName();
  vtkGetStringMacro(FileName);  // GetFileName();
  
  vtkSetMacro(Fill, int);
  vtkGetMacro(Fill, int);
    
  vtkSetMacro(Point, int);
  vtkGetMacro(Point, int);
  
  vtkSetMacro(Crack, int);
  vtkGetMacro(Crack, int);
  
  vtkSetMacro(Joint, int);
  vtkGetMacro(Joint, int);
  
protected:
  vtkFDEMReader();
  ~vtkFDEMReader();

  virtual int RequestInformation(vtkInformation*,vtkInformationVector**,vtkInformationVector*);
  virtual int RequestData(vtkInformation*,vtkInformationVector**,vtkInformationVector*);
  
  char* FileName; 
  
  //options the user can turn on / off
  int Fill;
  int Point;
  int Crack;
  int Joint;
  
  //pointer to the open file
  FILE* RawFile;  
  
  //flag that we have parse the .y file
  bool YFileFound;
  
  //values needed to un-normalize the properties  
  double NormCoord;
  double NormForce;
  double NormStress;
  double NormVelocity;
  
  /* vars and methods needed for basic reading */
  char RawBlock[VTK_FDEM_BLOCK_SIZE];
  double Block[VTK_FDEM_BLOCK_SIZE];
  char *CharacterTable;
  long Lookup[1000];
  long Max[10];  
  
  bool ReadBlock();
  long ConvertBlock();
  long FindValue( long position ); 
  
private:     
  vtkFDEMReader(const vtkFDEMReader&);  // Not implemented.
  void operator=(const vtkFDEMReader&);  // Not implemented.
};

#endif
