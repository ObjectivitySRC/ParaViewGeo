

#ifndef __vtkMap3dWriter_h
#define __vtkMap3dWriter_h



#include "vtkDataWriter.h"
#include "vtkPolyData.h"



class VTK_EXPORT vtkMap3dWriter : public vtkDataWriter
{
public:
	static vtkMap3dWriter *New();
	vtkTypeRevisionMacro(vtkMap3dWriter,vtkDataWriter);
	void PrintSelf(ostream& os, vtkIndent indent);
	
	vtkSetMacro(WithActivities, int);

	// Get the input to this writer.
	vtkPolyData* GetInput();
	vtkPolyData* GetInput(int port);
	//void SetFileName(const char* filename);

	vtkSetStringMacro(SegmentID);
	vtkGetStringMacro(SegmentID);
	vtkSetStringMacro(MiningBlk);
	vtkGetStringMacro(MiningBlk);
	vtkSetMacro(Thickness,double);
	vtkSetMacro(Type, int);
	vtkSetMacro(Width, double);

protected:
	vtkMap3dWriter();
	~vtkMap3dWriter();

	
	void WriteData();
	virtual int FillInputPortInformation(int port, vtkInformation *info);
  //virtual int RequestData(vtkInformation *,
    //                             vtkInformationVector **,
      //                           vtkInformationVector *);

	char* SegmentID;
	char* MiningBlk;
	int Type;
	double Thickness;
	double Width;


private:
	vtkMap3dWriter(const vtkMap3dWriter&);  // Not implemented.
	void operator=(const vtkMap3dWriter&);  // Not implemented.
	//char* StringToLower(const char*);

	vtkPolyData *input;
	vtkCellArray *inCells;
	int numberOfCells;
	vtkPoints *inPoints;
	vtkDataArray* BlkNum;
	vtkDataArray* MiningBlock;

	int WithActivities;

};

#endif


