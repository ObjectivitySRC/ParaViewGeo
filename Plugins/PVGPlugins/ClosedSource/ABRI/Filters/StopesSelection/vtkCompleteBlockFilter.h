/*=========================================================================
  Program:   Visualization Toolkit
  Module:    $ vtkCompleteBlockFilter.h $
  Authors:    Arolde VIDJINNAGNI
  MIRARCO, Laurentian University
  Date:   July 2009    
  Version:   0.1
=========================================================================*/


#ifndef __vtkBlockNormalization_h
#define __vtkBlockNormalization_h

#include "vtkPolyData.h"
#include "vtkDataSet.h"
#include "vtkIdTypeArray.h"
#include "vtkDoubleArray.h"
#include "vtkIntArray.h"
#include "vtkFloatArray.h"
#include "vtkPolyDataAlgorithm.h"

class VTK_EXPORT vtkCompleteBlockFilter : public vtkPolyDataAlgorithm
{
public:
    
	static vtkCompleteBlockFilter *New();
	vtkTypeRevisionMacro(vtkCompleteBlockFilter,vtkPolyDataAlgorithm);
	
	//DIMENSION ARRAYS NAME
	vtkSetStringMacro(XINC);
  vtkGetStringMacro(XINC);

	vtkSetStringMacro(YINC);
  vtkGetStringMacro(YINC);

	vtkSetStringMacro(ZINC);
  vtkGetStringMacro(ZINC);

	vtkSetMacro(BlockModelType,int);
  vtkGetMacro(BlockModelType,int);


  virtual int FillInputPortInformation(int port, vtkInformation* info);
  virtual int FillOutputPortInformation(int port, vtkInformation* info); 

  protected:

  vtkCompleteBlockFilter(void);
  ~vtkCompleteBlockFilter(void);
 	
	//==================================================================================
	int CompleRegularB(vtkPolyData* dataset, vtkPolyData* out);

	//==================================================================================
	int getBlockSize(vtkPolyData* dataset);

	//==================================================================================
	int CompleIRegularB(vtkPolyData* dataset, vtkPolyData* out);

	//==================================================================================
	virtual int RequestData(vtkInformation *request,
							 vtkInformationVector **inputVector,
							 vtkInformationVector *outputVector);  

	

private:

	vtkCompleteBlockFilter(const vtkCompleteBlockFilter&);
  void operator = (const vtkCompleteBlockFilter&);
	
	char* XINC;
	char* YINC;
	char* ZINC;
 
	double NormalSizeCX;
	double NormalSizeCY;
	double NormalSizeCZ;

	int Nbx;
	int Nby;
	int Nbz;
	
	double AllBounds[6]; //input bonding box
  //set the size if default is needed
	vtkDoubleArray* XINCArray;//size array on x dimension
	vtkDoubleArray* YINCArray;//size array on y dimension
	vtkDoubleArray* ZINCArray;//size array on z dimension

	int nbArrays;
	vtkDataArray** arrays;
	vtkIntArray* threshold;

	int BlockModelType;// regular block
	int numberOfNewPoints;
	vtkIdType*** pointsTable;	
	int pDimension[3];

};

#endif
