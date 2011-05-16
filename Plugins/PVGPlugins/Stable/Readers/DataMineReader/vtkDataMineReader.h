// .NAME DataMineReader from vtkDataMineReader
// .SECTION Description
// vtkDataMineReader is a subclass of vtkPolyDataAlgorithm
// to read DataMine binary Files (point, perimeter, wframe<points/triangle>)

#ifndef __DataMineReader_h
#define __DataMineReader_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkDataArraySelection.h"
#include "dmfile.h"

class vtkPolyData;
class vtkCallbackCommand;
class vtkPoints;
class vtkCellArray;
//BTX
class PropertyStorage;
class PointMap;
//ETX

class VTK_EXPORT vtkDataMineReader : public vtkPolyDataAlgorithm
{
public:
  static vtkDataMineReader* New();
  vtkTypeRevisionMacro(vtkDataMineReader,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

	
  vtkGetObjectMacro(CellDataArraySelection, vtkDataArraySelection);
	vtkSetObjectMacro(CellDataArraySelection, vtkDataArraySelection);
  int GetCellArrayStatus(const char* name);

  virtual void SetCellArrayStatus(const char* name, int status);  	

	int GetNumberOfCellArrays();
	const char* GetCellArrayName(int index);
     
  vtkSetStringMacro(FileName);
  vtkGetStringMacro(FileName);
  
  
protected:
  vtkDataMineReader();
  ~vtkDataMineReader();

  virtual int RequestInformation(vtkInformation *, vtkInformationVector **,
                          vtkInformationVector *);
  virtual int RequestData(vtkInformation *, vtkInformationVector **,
                          vtkInformationVector *);
  
 
  // The observer to modify this object when the array selections are
  // modified.
  //BTX
  vtkCallbackCommand* SelectionObserver;    
  static void SelectionModifiedCallback(vtkObject* caller, unsigned long eid, void* clientdata, void* calldata);
	vtkDataArraySelection* CellDataArraySelection;	
  int SetFieldDataInfo(vtkDataArraySelection* eDSA, int association,  int numTuples, vtkInformationVector *(&infoVector));		
	void SetupOutputInformation(vtkInformation *outInfo);
	virtual void UpdateDataSelection();	
	//ETX
	
	//cleaning output data
	virtual void CleanData(vtkPolyData* preClean, vtkPolyData* output);
	
	//checks and see if the file is a valid format
	virtual int CanRead( const char* fname, FileTypes type );
	
	//DM file reading methods
	virtual void Read( vtkPoints* points, vtkCellArray *cells ){};
	virtual void ParseProperties( Data *values );		
	
	//returns true if the property was created
	virtual bool AddProperty(char* varname, const int &pos, const bool &numeric);
	virtual void SegmentProperties( const int &records );
	
	//internal storage	
	PointMap *PointMapping;
  PropertyStorage *Properties;
  
  //user defined filenames
  char *FileName;	
 
	//number of possible properties in file
	int PropertyCount;	
	
	//the output mode type;
	VTKCellType CellMode;
};
#endif
