/*=========================================================================
  Program:   Visualization Toolkit
  Module:    $ vtkFitnessCalculator.h $
  Authors:   Nehme Bilal & Arolde VIDJINNAGNI 
  MIRARCO, Laurentian University
  Date:      
  Version:   0.1
=========================================================================*/


#ifndef __vtkFitnessCalculator_h
#define __vtkFitnessCalculator_h


#include "vtkPolyDataAlgorithm.h"
class vtkDoubleArray;
struct ABRI_Internals;

class VTK_EXPORT vtkFitnessCalculator : public vtkPolyDataAlgorithm
{

public:
    
	static vtkFitnessCalculator *New();

	vtkTypeRevisionMacro(vtkFitnessCalculator,vtkPolyDataAlgorithm);

	vtkSetMacro(BlockSizeX, double);
  vtkGetMacro(BlockSizeX, double);

	vtkSetMacro(BlockSizeY, double);
  vtkGetMacro(BlockSizeY, double);

	vtkSetMacro(BlockSizeZ, double);
  vtkGetMacro(BlockSizeZ, double);

  vtkSetMacro(AccessPosition, int);
  vtkGetMacro(AccessPosition, int);
	
	vtkSetMacro(RegularBlock, int);
	vtkGetMacro(RegularBlock, int);

	vtkSetStringMacro(geotechProp);
	vtkGetStringMacro(geotechProp);
	vtkSetStringMacro(densityInput);
	vtkGetStringMacro(densityInput);

	vtkSetStringMacro(pythonPath);
	vtkGetStringMacro(pythonPath);

	vtkSetStringMacro(requestData);
	vtkGetStringMacro(requestData);

	vtkSetStringMacro(helperFile);
	vtkGetStringMacro(helperFile);

	void Setmodified(int i)
	{
		this->modified = i;
		this->Modified();
	}
 
  virtual int FillInputPortInformation(int port, vtkInformation* info);
	virtual int FillOutputPortInformation(int port, vtkInformation* info); 

	// will get the python script from a file and copy it into a char *
	vtkStdString getPythonScript();

	void Setfunctions(const char *a);

  protected:

  vtkFitnessCalculator(void);
  ~vtkFitnessCalculator(void);	

  virtual int RequestData(vtkInformation *request,
						   vtkInformationVector **inputVector,
						   vtkInformationVector *outputVector);
  
   
private:
	
	vtkPolyData* input;
	vtkPolyData *output;
	double BlockSizeX;
	double BlockSizeY;
	double BlockSizeZ;

	int AccessPosition;
	
	int ***pointTable;
	char* geotechProp;
	char* densityInput;

	char* pythonPath;
	char* requestData;
	char* helperFile;
	int numberOfFunctions;
	ABRI_Internals *Internal;
	bool clearArray;

	int RegularBlock;

	vtkDoubleArray* fitnessArray;

	int modified;

	vtkFitnessCalculator(const vtkFitnessCalculator&);  // Not implemented.
    void operator=(const vtkFitnessCalculator&);  // Not implemented.

};

#endif
