/*=========================================================================
  Program:   Visualization Toolkit
  Module:    $ vtkStopesResearch.h $
  Authors:    Arolde VIDJINNAGNI , Rafik FAHEM
  MIRARCO, Laurentian University
  Date:   May 2009    
  Version:   0.1
=========================================================================*/


#ifndef __vtkStopesSelection_Ir_h
#define __vtkStopesSelection_Ir_h

#include "vtkPolyData.h"
#include "vtkIdTypeArray.h"
#include "vtkDoubleArray.h"
#include "vtkIntArray.h"
#include "vtkPolyDataAlgorithm.h"
#include "Stope.h"

class InternalExtractList;

class VTK_EXPORT vtkStopesResearch : public vtkPolyDataAlgorithm
{
public:
    
	static vtkStopesResearch *New();
	vtkTypeRevisionMacro(vtkStopesResearch,vtkPolyDataAlgorithm);
 
  virtual int FillInputPortInformation(int port, vtkInformation* info);
  virtual int FillOutputPortInformation(int port, vtkInformation* info); 

	vtkSetStringMacro(densityInput);
  vtkGetStringMacro(densityInput);

	vtkSetMacro(InstituteTone, double);
	vtkGetMacro(InstituteTone, double);

	vtkSetMacro(Swell, double);
	vtkGetMacro(Swell, double);

	vtkSetMacro(ExtractionCapacity, double);
	vtkGetMacro(ExtractionCapacity, double);

	vtkSetMacro(BlockModelType,int);
  vtkGetMacro(BlockModelType,int);

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

  vtkStopesResearch(void);
  ~vtkStopesResearch(void);
  
	//==================================================================================
	virtual int RequestData(vtkInformation *request,
							 vtkInformationVector **inputVector,
							 vtkInformationVector *outputVector);

	
	/****************************************************************************************/
	/* Name: initialize																																	    */
	/* Description:		The cfunction initialize all needed variables                         */
	/* parameters:		inDataset: vtkPolyData* 							                                */
	/* return value:																																			  */
	/****************************************************************************************/
	void initialize(vtkPolyData* inDataset);

	/****************************************************************************************/
	/* Name: getBlockSize		                                                                */
	/* Description:		determin the input dimension because it's irregular block model       */
	/* parameters:	dataset 	vtkPolyData*						                                      */
	/* return value:																																				*/
	/****************************************************************************************/
  void getBlockSize(vtkPolyData* dataset);

	
	/****************************************************************************************/
	/* Name: SubDivideBModel																																*/                                                                                   
	/* Description:	the function call the vtkBlockSubdivision filter to divide the block		*/	
	/*							model by layer in X and Y dimensions 																		*/
	/* parameters:		inDataset: vtkPolyData* 							                                */
	/* return value:																																			  */
	/****************************************************************************************/
	void SubDivideBModel(vtkPolyData* bModel, vtkPolyData* out);

	/****************************************************************************************/
	/* Name: ComputeDevelopmentStope																										    */
	/* Description:		compute the stopes in the block model and set stope id value          */
	/* parameters:		bModel: vtkPolyData* 																									*/
	/* return value:																																			  */
	/****************************************************************************************/
	void ComputeDevelopmentStope(vtkPolyData* inDataset);

	
	/****************************************************************************************/
	/* Name: GetScore																																				*/
	/* Description:	evaluater the fitnesses sum value and return like a score for the				*/
	/*								stope in the current extraction level														      */
	/* parameters:	inDataset 	vtkPolyData*						                                    */
	/* return value:																																				*/
	/****************************************************************************************/
	double GetScore(vtkPolyData* inDataset,
								  vtkIdType id,
								 	int level,
									double We[1]
									);

	/****************************************************************************************/
	/* Name: ComputeCaving																															    */
	/* Description:		compute the block caving algorithm and set the extraction level on		*/
	/*									the stope																														*/
	/* parameters:		inDataset: vtkPolyData* 							                                */
	/* return value:																																			  */
	/****************************************************************************************/
	void ComputeCaving(vtkPolyData* inDataset);	

	
	/****************************************************************************************/
	/* Name: setProperties		                                                              */
	/* Description:	add the properties values to the output								                  */
	/* parameters:	dataset:output of the filter																	          */
	/* return value:	none																																  */
	/****************************************************************************************/
	void setProperties(vtkPolyData* dataset);
	
private:

	int BlockModelType;
	char* XINC;
	char* YINC;
	char* ZINC;
	double stopeWidth;
	double stopeDepth;
	vtkDoubleArray* fitnessArray;//contains fitness values of the blocks
	vtkDoubleArray* blockDensity; //density of the blockmodel	
	char* densityInput;
	double Allbounds[6];
	char* Function;
	char* FitnessArrayName;
	double ReplacementValue;
	int ReplaceInvalidValues;
	vtkDoubleArray* XINCArray;
	vtkDoubleArray* YINCArray;
	vtkDoubleArray* ZINCArray;
	vtkDoubleArray* newXINCArray;
	vtkDoubleArray* newYINCArray;
	vtkDoubleArray* newZINCArray;

	int nbXLayers;
	int nbYLayers;
	vtkIntArray* XLayersArray;
	vtkIntArray* YLayersArray;
	vtkIdList*** ptsOrderedByLayer;
	vtkIntArray* stopeIdArray;
	vtkIntArray* ExtractionLevelArray;
	vtkIdType stopeId;
	
	StopeContainer* Container; //Stopes container

	//Caving informations
	double InstituteTone;
	double Swell; 
	double ExtractionCapacity; 
	double UnitCave; //based on Z direction minimum block size
	InternalExtractList* BCavingList;
	vtkIdList** CaveResultList;
	int numBerOfCells;
	
	void operator=(const vtkStopesResearch&);  // Not implemented.
};

#endif
