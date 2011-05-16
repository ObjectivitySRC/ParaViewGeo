/*=========================================================================
  Program:   Visualization Toolkit
  Module:    $ vtkStopesSelection.h $
  Authors:    Arolde VIDJINNAGNI , Rafik FAHEM
  MIRARCO, Laurentian University
  Date:   May 2009    
  Version:   0.1
=========================================================================*/


#ifndef __vtkStopesSelection_h
#define __vtkStopesSelection_h

#include "vtkPolyData.h"
#include "vtkDoubleArray.h"
#include "vtkPolyDataAlgorithm.h"


class VTK_EXPORT vtkStopesSelection : public vtkPolyDataAlgorithm
{
public:
    
	static vtkStopesSelection *New();
	vtkTypeRevisionMacro(vtkStopesSelection,vtkPolyDataAlgorithm);
 
  virtual int FillInputPortInformation(int port, vtkInformation* info);
  virtual int FillOutputPortInformation(int port, vtkInformation* info); 
  
	
	//==================================================================================
	virtual int TestBlockType(vtkPolyData* dataset);

	vtkSetStringMacro(densityInput);
  vtkGetStringMacro(densityInput);

	vtkSetMacro(InstituteTone, double);
	vtkGetMacro(InstituteTone, double);

	vtkSetMacro(Swell, double);
	vtkGetMacro(Swell, double);

	vtkSetMacro(ExtractionCapacity, double);
	vtkGetMacro(ExtractionCapacity, double);

	vtkSetStringMacro(fileName);
	vtkGetStringMacro(fileName); 

	vtkSetMacro(fileButton, int);

	vtkSetStringMacro(XINC);
  vtkSetStringMacro(YINC);
  vtkSetStringMacro(ZINC);
	vtkSetMacro(stopeWidth, double);
	vtkSetMacro(stopeDepth, double);
 
	// Description:
  // Set/Get the function to be evaluated.
	vtkSetStringMacro(Function);
  vtkGetStringMacro(Function);

	vtkSetStringMacro(FitnessArrayName);
  vtkGetStringMacro(FitnessArrayName);

	vtkSetMacro(ReplaceInvalidValues,int);
  vtkGetMacro(ReplaceInvalidValues,int);
  vtkBooleanMacro(ReplaceInvalidValues,int);
  vtkSetMacro(ReplacementValue,double);
  vtkGetMacro(ReplacementValue,double);
 
 
  protected:

  vtkStopesSelection(void);
  ~vtkStopesSelection(void);
  
	//==================================================================================
	virtual int RequestData(vtkInformation *request,
							 vtkInformationVector **inputVector,
							 vtkInformationVector *outputVector);  

private:
	
	double InstituteTone;
	double Swell;
	double ExtractionCapacity;

	char* XINC;
	char* YINC;
	char* ZINC;
	double stopeWidth;
	double stopeDepth;
	char* densityInput;
	char* fileName;
	int fileButton;

	//******************************Calculator parameters***************************/
	char* Function;
	char* FitnessArrayName;
	double ReplacementValue;
	int ReplaceInvalidValues;

	vtkDoubleArray* XINCArray;//size array on x dimension
	vtkDoubleArray* YINCArray;//size array on y dimension
	vtkDoubleArray* ZINCArray;//size array on z dimension

	int BlockModelType;

  void operator=(const vtkStopesSelection&);  // Not implemented.	

};

#endif
