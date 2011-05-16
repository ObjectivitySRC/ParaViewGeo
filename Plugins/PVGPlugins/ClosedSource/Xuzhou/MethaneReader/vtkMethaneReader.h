// .NAME vtkMethaneReader
// .SECTION Description

#ifndef __vtkMethaneReader_h
#define __vtkMethaneReader_h

#include "vtkMultiBlockDataSetAlgorithm.h"
#include "vtkMultiBlockDataSet.h"
#include "vtkSQLiteDatabase.h"
#include "vtkSQLQuery.h"
#include "vtkStringArray.h"
#include "vtkIntArray.h"
#include "vtkDoubleArray.h"
#include "vtkStdString.h"
#include "vtkPolyData.h"

class vtkSBuffer;

// class must inherit from vtkMultiBlockDataSetAlgorithm if you want to use MultiBlockDataSets
class VTK_EXPORT vtkMethaneReader : public vtkMultiBlockDataSetAlgorithm
{
public:
	static vtkMethaneReader* New();
	vtkTypeRevisionMacro (vtkMethaneReader, vtkMultiBlockDataSetAlgorithm);
	void PrintSelf(ostream& os, vtkIndent indent);

	vtkSetStringMacro(FileName); //SetFileName()

protected:
	vtkMethaneReader();
	~vtkMethaneReader();

	int RequestData(vtkInformation* request, 
							vtkInformationVector** inputVector,
							vtkInformationVector* outputVector);

		vtkSQLiteDatabase* db;
		vtkSQLQuery* query;

private:
	char* FileName;

	void SetPropertyName(const char* field, vtkStringArray *names);
	vtkStringArray* GetPropertyNames(int *numProperties);
	vtkDoubleArray** CreatePropertyArrays(vtkStringArray *names, int numProperties);
	vtkIntArray* GetQueryIndices();
	void ProcessMethane();
	void CreateBlock(vtkPolyData *obj, vtkMultiBlockDataSet *appender);
	void ProcessMethaneGeology(vtkIdType *node, vtkPoints *pointsPtr, vtkCellArray *cellsPtr, vtkSQLQuery *query);
	void GetQuery(vtkSBuffer *buffer);

};

#endif


