

#ifndef __vtkRegionsBoundingBox_h
#define __vtkRegionsBoundingBox_h



#include "vtkDataWriter.h"
#include "vtkPolyData.h"



class VTK_EXPORT vtkRegionsBoundingBox : public vtkDataWriter
{
public:
	static vtkRegionsBoundingBox *New();
	vtkTypeRevisionMacro(vtkRegionsBoundingBox,vtkDataWriter);
	void PrintSelf(ostream& os, vtkIndent indent);

	// Get the input to this writer.
	vtkPolyData* GetInput();
	vtkPolyData* GetInput(int port);
	//void SetFileName(const char* filename);

	vtkSetStringMacro(SegmentID);
	vtkGetStringMacro(SegmentID);

protected:
	vtkRegionsBoundingBox();
	~vtkRegionsBoundingBox();

	
	void WriteData();
	virtual int FillInputPortInformation(int port, vtkInformation *info);

	char* SegmentID;

private:
	vtkRegionsBoundingBox(const vtkRegionsBoundingBox&);  // Not implemented.
	void operator=(const vtkRegionsBoundingBox&);  // Not implemented.
	//char* StringToLower(const char*);

};

#endif


