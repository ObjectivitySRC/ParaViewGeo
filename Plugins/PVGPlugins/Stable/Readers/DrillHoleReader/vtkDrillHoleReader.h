/*Robert Maynard
MIRARCO 
Oct 23 2008

Interface to VTK for loading of sqlite drill hole databases
Output is always multiblock, since the db stores assays / surveys / curves
*/


#ifndef __vtkDrillHoleReader_h
#define __vtkDrillHoleReader_h

#include "vtkPolyDataAlgorithm.h"

class vtkInformation;
class vtkCallbackCommand;
class vtkDataArraySelection;
class vtkSQLiteDatabase;
class vtkSQLiteQuery;
class vtkInternalGocadGroups;
class vtkStdString;
class vtkBitArray;

class VTK_EXPORT vtkDrillHoleReader : public vtkPolyDataAlgorithm
{
public:
  
  static vtkDrillHoleReader* New();
  vtkTypeRevisionMacro(vtkDrillHoleReader,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  vtkSetStringMacro(FileName); 
  vtkGetStringMacro(FileName); 

	vtkSetStringMacro(Filter);
	vtkGetStringMacro(Filter);

	vtkSetMacro(CreatePointProp, bool);
	vtkGetMacro(CreatePointProp, bool);

	vtkSetMacro(KeepCellProp, bool);
	vtkGetMacro(KeepCellProp, bool);
	
	int CanReadFile( const char* fname );
	
	vtkGetObjectMacro(DrillHoles, vtkDataArraySelection);
  int GetDrillHoleArrayStatus(const char* name);
  void SetDrillHoleArrayStatus(const char* name, int status);  
	int GetNumberOfDrillHoleArrays();
	const char* GetDrillHoleArrayName(int index);	
	

protected:
  vtkDrillHoleReader();
  ~vtkDrillHoleReader();

	 
	// The observer to modify this object when the array selections are modified.
  //BTX	
  vtkCallbackCommand* SelectionObserver;    
  static void SelectionModifiedCallback(vtkObject* caller, unsigned long eid,void* clientdata, void* calldata);	
  int SetFieldDataInfo(vtkDataArraySelection* eDSA, int association,  int numTuples, vtkInformationVector *(&infoVector));			
	void SetupOutputInformation(vtkInformation *outInfo);	
	//ETX
  
  //file IO
  virtual bool OpenDatabase(  );
  virtual void CloseQuery( vtkSQLiteQuery *query );   
            
  virtual int RequestInformation(vtkInformation* request, vtkInformationVector** inputVector, vtkInformationVector* outputVector);
  virtual int RequestData(vtkInformation* request, vtkInformationVector** inputVector, vtkInformationVector* outputVector);
  
	void ReadCurves( vtkPolyData *output );
	void ReadSegment( vtkPolyData *output, vtkStdString type, vtkStdString segment  );	

	vtkStdString ConstructSegmentQuery( vtkStdString type, vtkStdString segment );

  char* FileName;
	char* Filter;

	bool CreatePointProp;
	bool KeepCellProp;
  
  //db IO members
	vtkSQLiteDatabase* Database;	

	//number of holes to load in
	int DrillHoleCount;


	//used to store what names we should load in or not in the segment method
	vtkBitArray* NameMap;

	//collection of drill hole names to be sent to ui
	vtkDataArraySelection* DrillHoles;

private:
  vtkDrillHoleReader(const vtkDrillHoleReader&);  // Not implemented.
  void operator=(const vtkDrillHoleReader&);  // Not implemented.
};

#endif  