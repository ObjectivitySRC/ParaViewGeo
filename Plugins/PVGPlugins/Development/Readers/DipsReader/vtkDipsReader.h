#ifndef _vtkDipsReader_h
#define _vtkDipsReader_h
#include "vtkPolyDataAlgorithm.h"
#include <vtkstd/vector>

class vtkInternalParse;

class VTK_EXPORT vtkDipsReader : public vtkPolyDataAlgorithm
{
public:
  static vtkDipsReader *New();
  vtkTypeRevisionMacro(vtkDipsReader,vtkPolyDataAlgorithm);

	vtkSetStringMacro(FileName);
  vtkGetStringMacro(FileName);

  int CanReadFile(const char* fname);
	void PrintSelf(ostream& os, vtkIndent indent);

protected:
  vtkDipsReader();
  ~vtkDipsReader();

  int RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

  char *FileName;
	vtkStdString FileTitle;
	int numofTraverse;
	int GlobalOrient;
	int ExtraColumns;
	double Declination;
	bool Quantity;
	vtkInternalParse *Internal;

private:
  vtkDipsReader(const vtkDipsReader&);
  void operator = (const vtkDipsReader&);

	bool parseHeaderData(ifstream &file);
	bool parseData(ifstream &file, vtkCellArray* planes, vtkCellArray* lines, vtkPoints* points);
	int GetGlobalFormat(vtkStdString format);

	//BTX
	enum
		{
		DIPandDIPDIRECTION = 0,
		STRIKEandDIPR = 1,
		STRIKEandDIPL = 2,
		TRENDandPLUNGE = 3
		};
	//ETX
};
#endif
