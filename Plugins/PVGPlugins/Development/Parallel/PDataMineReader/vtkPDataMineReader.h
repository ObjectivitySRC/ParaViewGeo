/*=========================================================================
  Program:   ParaviewGeo
  Module:    $ vtkPDataMineReader.cxx $
  Author:    Arolde VIDJINNAGNI	    
  Date:      MIRARCO June 25, 2009 
  Version:   0.1
 =========================================================================*/
// .NAME PDataMineReader from vtkPDataMineReader
// .SECTION Description
// vtkPDataMineReader is a subclass of vtkPolyDataAlgorithm
//=========================================================================
// .NAME vtkPDataMineReader - Manages writing pieces of a dataMine.
// .SECTION Description
// vtkPDataMineReader will create multiple shrunked pieces of 
// a metadata file and load them on multiple nodes

#ifndef __vtkPDataMineReader_h
#define __vtkPDataMineReader_h

#include "vtkSetGet.h"
#include "vtkPolyDataAlgorithm.h"
#include "vtkDataArraySelection.h"
class  vtkMultiProcessController;
#include "vtkPointData.h"
class vtkPolyData;
#include "vtkCellData.h"
#include "vtkAppendPolyData.h"
#include "vtkDataMineBlockReader.h"
#include "vtkDataMineDrillHoleReader.h"
#include "vtkDataMinePerimeterReader.h"
#include "vtkDataMinePointReader.h" 
#include "vtkDataMineDummyReader.h"
#include "vtkDataMineWireFrameReader.h"

class vtkCellArray;

class VTK_EXPORT vtkPDataMineReader : public vtkPolyDataAlgorithm
{
public:
  static vtkPDataMineReader* New();
  vtkTypeRevisionMacro(vtkPDataMineReader,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

	vtkGetObjectMacro(CellDataArraySelection, vtkDataArraySelection);
	vtkSetObjectMacro(CellDataArraySelection, vtkDataArraySelection);

  int GetCellArrayStatus(const char* name);

  virtual void SetCellArrayStatus(const char* name, int status);  	

	int GetNumberOfCellArrays();
	const char* GetCellArrayName(int index);

  vtkSetStringMacro(FileName);
  vtkGetStringMacro(FileName);

	vtkSetMacro(NbPieces,int); 
  vtkGetMacro(NbPieces,int);

	vtkSetStringMacro(ConfigurationFile);
  vtkGetStringMacro(ConfigurationFile);
		

  virtual void SetController(vtkMultiProcessController *);
  vtkGetObjectMacro(Controller, vtkMultiProcessController);


	/***************************************************************************************/
	//return the type of reader wich will support this file format
	bool WriteFileHeader(char *fname); //write the header of all files

	/***************************************************************************************/
  bool WriteFilePages(char *fname);

	/***************************************************************************************/
	int WriteDrillHolesPages(char *fname);

	/***************************************************************************************/
	int CreateDataMinePieces();
  
	/***************************************************************************************/
	void SetNumberOfPieces(int num);//initialyze the PieceFileNames array
	
	/***************************************************************************************/
	// create an array of all piece file names
  int CreatePieceFileNames(int numProc);  

	/***************************************************************************************/  
	int ReadConfigurationsFile();

	/***************************************************************************************/
	char* GetPieceFileName(int Piec);
 
  int NumberOfPieces; //number of pieces desired.by default equal to the number of processes
  int NbPieces;
  int GhostLevel;
  int Piece;
  int MyId;
  int NumProcesses;  
  char* PathName;// The path to the input file without the file name.

	/***************************************************************************************/
  // vtkDataArraySelection* CellDataArraySelection;	
  void SplitFileName();

	/***************************************************************************************/
	//change the header for each piece
	void ChangeFileNameFromBuf(char *buf, char* fname);

	/***************************************************************************************/
	void ChangeNLastPageRecsFromBuf(char *buf,int n, bool ByteSwapped);

	/***************************************************************************************/
	void ChangeNPhysicalPagesFromBuf(char *buf, int n, bool ByteSwapped);

  int WordSize;
	int BufferSize;
	bool FMT64;

protected:
  vtkPDataMineReader();
  ~vtkPDataMineReader();

  vtkMultiProcessController* Controller;
  
	/***************************************************************************************/
	int RequestInformation(vtkInformation* request,
                          vtkInformationVector** inputVector,
                          vtkInformationVector* outputVector);

	/***************************************************************************************/
  virtual int RequestData(vtkInformation *, vtkInformationVector **,
                          vtkInformationVector *);

	/***************************************************************************************/
	//checks and see if the file is a valid format
	virtual int CanRead( const char* fname, FileTypes type );

	/***************************************************************************************/
	virtual int ReadShrunks( int start, int end, vtkPolyData* out){ return 1;};

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

	//number of possible properties in file
	int PropertyCount;	
	
	char **PieceFileNames; //contain a list of files names
	int StartPiece; 
	int EndPiece; 
  char *FileName;
	char *FilePattern;
	char* ConfigurationFile;

};
#endif
