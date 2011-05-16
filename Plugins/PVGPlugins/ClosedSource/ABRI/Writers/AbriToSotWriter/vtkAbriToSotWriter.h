/*=========================================================================
  Program:   Visualization Toolkit
  Module:    $ vtkStopesSelection.cxx $
  Authors:    Arolde VIDJINNAGNI 
  MIRARCO, Laurentian University
  Date:    June 2009 
  Version:   0.1
=========================================================================*/

#ifndef __vtkAbriToSotWriter_h
#define __vtkAbriToSotWriter_h

#include "vtkDataWriter.h"
#include "vtkPolyData.h"
#include "TimeConverter.h"
#include <list>
#include <vector>

struct dependenciesStruct;

struct Stope 
{
  int id_stope;//id of the stope and will be the ACTIVITY_ID
  int nbBlocks;
  double weigth; //Tonnes
  double length;
  double volume;
  double nsr; //NSR 
  char* constraint_Date;
  double duration;
};
struct NSCoords
{
	vtkIdType id;
	int position; //the neighbour stope is at left or right
};
class vtkNeighboursVect;

class VTK_EXPORT vtkAbriToSotWriter : public vtkDataWriter
{
public:
	static vtkAbriToSotWriter *New();
	vtkTypeRevisionMacro(vtkAbriToSotWriter,vtkDataWriter);
	void PrintSelf(ostream& os, vtkIndent indent);
	
	// Get the input to this writer.
	vtkPolyData* GetInput();
	vtkPolyData* GetInput(int port);
	//void SetFileName(const char* filename);

	vtkSetMacro(Duration, double);
	vtkGetMacro(Duration, double); 

	vtkSetMacro(DaysByMeter, double);
	vtkGetMacro(DaysByMeter, double);

	vtkSetMacro(HzDByMeter, double);
	vtkGetMacro(HzDByMeter, double);
  	
	vtkSetStringMacro(StartDate);
  vtkGetStringMacro(StartDate);
  	
	vtkSetStringMacro(StrikePosition);
  vtkGetStringMacro(StrikePosition);
  	
	vtkSetStringMacro(ScriptPath);
  vtkGetStringMacro(ScriptPath);

	vtkSetMacro(StartingStope, int);
	vtkGetMacro(StartingStope, int);

	vtkSetMacro(TypeOfBlocks, int); //Type of block in input
	vtkGetMacro(TypeOfBlocks, int);

  vtkSetVector3Macro(BlockSize,double);
  vtkGetVectorMacro(BlockSize,double,3);
	
protected:
	vtkAbriToSotWriter();
	~vtkAbriToSotWriter();
	void ComputeStopesCenter();
	void ComputeNeighbours_Ir( vtkNeighboursVect* Nx, 
														 vtkNeighboursVect* Ny,
														 vtkNeighboursVect* Nz );
	void WriteData();
	virtual int FillInputPortInformation(int port, vtkInformation *info);
 
private:
	double HzDByMeter;
	double DaysByMeter;
	double Duration;
	char* StartDate;
	char* StrikePosition;
	char* ScriptPath;
	int StartingStope;
	int TypeOfBlocks;
	int ***blocksArray;
	double BlockSize[3];
	int d1;
	int d2;
	int d3;
	int numberOfStopes;
	int generateDependencies;
	
	vtkPolyData *input;

	vtkAbriToSotWriter(const vtkAbriToSotWriter&);  // Not implemented.
	void operator=(const vtkAbriToSotWriter&);  // Not implemented.
	int generateFactsFile();
	int validateData();
	vtkPoints *inPoints;

	//stopes center coordonnates
	vtkIntArray** stopePointsList;
	vtkDoubleArray* StopeCenterXArray; //list of X coordonnates of the centers of the stopes
	vtkDoubleArray* StopeCenterYArray;
	vtkDoubleArray* StopeCenterZArray;

};

#endif

