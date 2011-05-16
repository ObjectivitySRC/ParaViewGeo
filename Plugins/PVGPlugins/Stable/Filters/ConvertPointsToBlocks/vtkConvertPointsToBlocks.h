/*=========================================================================

   Program:   Visualization Toolkit
  Module:    $ vtkTopOfCell.h $
  Author:    Arolde VIDJINNAGNI
  MIRARCO,   Laurentian University
  Date:      May 7 2009
  Version:   0.1
=========================================================================*/


#ifndef _vtkConvertPointsToBlocks_h
#define _vtkConvertPointsToBlocks_h

#include "vtkUnstructuredGridAlgorithm.h"
#include "vtkPolyData.h"

class VTK_EXPORT vtkConvertPointsToBlocks : public vtkUnstructuredGridAlgorithm
{
public:
  static vtkConvertPointsToBlocks *New();
  vtkTypeRevisionMacro(vtkConvertPointsToBlocks,vtkUnstructuredGridAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  //dimension 
	vtkSetStringMacro(XINC);
  vtkGetStringMacro(XINC);

	vtkSetStringMacro(YINC);
  vtkGetStringMacro(YINC);

	vtkSetStringMacro(ZINC);
  vtkGetStringMacro(ZINC);

  vtkSetMacro(SizeCX, double);
  vtkGetMacro(SizeCX, double);

	vtkSetMacro(SizeCY, double);
  vtkGetMacro(SizeCY, double);

	vtkSetMacro(SizeCZ, double);
  vtkGetMacro(SizeCZ, double);

	vtkSetMacro(UseSizes, int);
  vtkGetMacro(UseSizes, int);
  

protected:
  vtkConvertPointsToBlocks();
  ~vtkConvertPointsToBlocks();
  int RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
  int FillInputPortInformation(int port, vtkInformation *info);
  int FillOutputPortInformation(int port, vtkInformation *info);

  int getSizeOfCell(vtkPolyData* input);
  vtkUnstructuredGrid* ConvertsToDefaultGeometry(vtkPolyData* polys);


private:
  vtkConvertPointsToBlocks(const vtkConvertPointsToBlocks&);
  void operator = (const vtkConvertPointsToBlocks&);

  //
	char* XINC;
	char* YINC;
	char* ZINC;
	int UseSizes;
  double SizeCX;
	double SizeCY;
	double SizeCZ; 
};
#endif
