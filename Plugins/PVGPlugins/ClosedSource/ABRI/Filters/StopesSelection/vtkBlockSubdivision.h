/*=========================================================================
  Program:   Visualization Toolkit
  Module:    $ vtkBlockSubdivision.cxx $
  Authors:    Arolde VIDJINNAGNI
  MIRARCO, Laurentian University
  Date:    August 2009 
  Version:   0.1
=========================================================================*/

#ifndef __vtkBlockSubdivision_h
#define __vtkBlockSubdivision_h

#include "vtkPolyData.h"
#include "vtkUnstructuredGrid.h"
#include "vtkDataSet.h"
#include "vtkIdTypeArray.h"
#include "vtkDoubleArray.h"
#include "vtkIntArray.h"
#include "vtkFloatArray.h"
#include "vtkPolyDataAlgorithm.h"
#include "vtkPolyDataAlgorithm.h"




class VTK_EXPORT vtkBlockSubdivision : public vtkPolyDataAlgorithm
{
public:
    
	static vtkBlockSubdivision *New();
	vtkTypeRevisionMacro(vtkBlockSubdivision,vtkPolyDataAlgorithm);
 
  virtual int FillInputPortInformation(int port, vtkInformation* info);
  virtual int FillOutputPortInformation(int port, vtkInformation* info); 

	vtkSetMacro(DeltaByLayer, double);
	vtkSetMacro(Direction, int);
	vtkSetMacro(boundMinX, double);
	vtkSetMacro(boundMinY, double);
	vtkSetMacro(boundMinZ, double);
	vtkSetMacro(boundMaxX, double);
	vtkSetMacro(boundMaxY, double);
	vtkSetMacro(boundMaxZ, double);

	//DIMENSION ARRAYS NAME
	vtkSetStringMacro(XINC);
  vtkGetStringMacro(XINC);

	vtkSetStringMacro(YINC);
  vtkGetStringMacro(YINC);

	vtkSetStringMacro(ZINC);
  vtkGetStringMacro(ZINC);
	
	protected:

  vtkBlockSubdivision(void);
  ~vtkBlockSubdivision(void);
  
	//==================================================================================
	virtual int RequestData(vtkInformation *request,
							 vtkInformationVector **inputVector,
							 vtkInformationVector *outputVector);

	//==================================================================================
	int getIntersectionWithLayerByX( vtkPolyData* inDataset,
																   vtkIdType id, double XLMin,
																   double XLMax,	double bounds[6]);

	//==================================================================================
	int getIntersectionWithLayerByY( vtkPolyData* inDataset,
																   vtkIdType id, double YLMin,
																   double YLMax,	double bounds[6]);

	//==================================================================================
	int getIntersectionWithLayerByZ( vtkPolyData* inDataset,
																   vtkIdType id, double ZLMin,
																   double ZLMax,	double bounds[6]);

	//==================================================================================
	void shrinkBlockModelByLayersByX( double sizeOfLayer,
																		vtkPolyData* inDataset, 
																		vtkPolyData* outDataset);

		//==================================================================================
	void shrinkBlockModelByLayersByY( double sizeOfLayer,
																		vtkPolyData* inDataset, 
																		vtkPolyData* outDataset);

	//==================================================================================
	void shrinkBlockModelByLayersByZ( double sizeOfLayer,
																		vtkPolyData* inDataset, 
																		vtkPolyData* outDataset);	

	//==================================================================================

private:

	double boundMinX; 
  double boundMinY;
	double boundMinZ;
	double boundMaxX;
  double boundMaxY;
	double boundMaxZ;


	char* XINC;
	char* YINC;
	char* ZINC;
  double DeltaByLayer;
	int Direction; //0= break by X direction 1=Y or 2= Z
	double Allbounds[6];

  void operator=(const vtkBlockSubdivision&);  // Not implemented.

	vtkDoubleArray* XINCArray;
	vtkDoubleArray* YINCArray;
	vtkDoubleArray* ZINCArray;

	vtkDoubleArray* newXINCArray;
	vtkDoubleArray* newYINCArray;
	vtkDoubleArray* newZINCArray;
	vtkIntArray* layersArray; //hold the current layer for each block
	vtkIdTypeArray* oldPointId; //hold the current layer for each block

	vtkDataArray** arrays;
	
	int newCellId;
};

#endif
