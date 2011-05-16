/*=========================================================================
  Program:   Visualization Toolkit
  Module:    $ vtkBlockNormalization.h $
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

class VTK_EXPORT vtkBlockNormalization : public vtkPolyDataAlgorithm
{
public:
    
	static vtkBlockNormalization *New();
	vtkTypeRevisionMacro(vtkBlockNormalization,vtkPolyDataAlgorithm);
	
	//DIMENSION ARRAYS NAME
	vtkSetStringMacro(XINC);
  vtkGetStringMacro(XINC);

	vtkSetStringMacro(YINC);
  vtkGetStringMacro(YINC);

	vtkSetStringMacro(ZINC);
  vtkGetStringMacro(ZINC);

	vtkSetMacro(NormalSizeCX, double);
  vtkGetMacro(NormalSizeCX, double);

	vtkSetMacro(NormalSizeCY, double);
  vtkGetMacro(NormalSizeCY, double);

	vtkSetMacro(NormalSizeCZ, double);
  vtkGetMacro(NormalSizeCZ, double);


  virtual int FillInputPortInformation(int port, vtkInformation* info);
  virtual int FillOutputPortInformation(int port, vtkInformation* info); 

  protected:

  vtkBlockNormalization(void);
  ~vtkBlockNormalization(void);
 	
	virtual int RequestData(vtkInformation *request,
							 vtkInformationVector **inputVector,
							 vtkInformationVector *outputVector);  

	int getBlockSize(vtkPolyData* dataset);

	void SetBlocks(vtkPolyData* tempInput, 
								 vtkPolyData* tempoutput);

	double getVolumeIntersection(double cell1[3], double size1[3],
								 double cell2[3], double size2[3]);
	int getReplaceValues( vtkPolyData* tempInput);

private:

	vtkBlockNormalization(const vtkBlockNormalization&);
  void operator = (const vtkBlockNormalization&);
	
	char* XINC;
	char* YINC;
	char* ZINC;
  double NormalSizeCX;
	double NormalSizeCY;
	double NormalSizeCZ; 

	int Nbx;
	int Nby;
	int Nbz;
	
	double res[6]; //input bonding box
  //set the size if default is needed
	vtkDoubleArray* XINCArray;//size array on x dimension
	vtkDoubleArray* YINCArray;//size array on y dimension
	vtkDoubleArray* ZINCArray;//size array on z dimension

	vtkDataArray** arrays;
	vtkIntArray* threshold;

};

#endif
