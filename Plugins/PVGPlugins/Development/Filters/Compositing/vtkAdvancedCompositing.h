/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $ vtkAdvancedCompositing.h $
  Author:    Arolde VIDJINNAGNI
	     
  Date:      JUNE 24, 2008
  Version:   0.1

=========================================================================*/
// .NAME vtkAdvancedCompositing - 

#ifndef __vtkAdvancedCompositing_h
#define __vtkAdvancedCompositing_h
#include "vtkSetGet.h"
#include "vtkPolyDataAlgorithm.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkSmartPointer.h"
#include "vtkDataArray.h"
#include "vtkSliceCompositing.h"

class VTK_EXPORT vtkAdvancedCompositing : public vtkPolyDataAlgorithm
{
public:
   //BTX
   typedef enum vtkPointSetDefinition { VERTEX, CELLCENTER };
   //ETX
   static vtkAdvancedCompositing *New();
   vtkTypeRevisionMacro(vtkAdvancedCompositing,vtkPolyDataAlgorithm);
   void PrintSelf(ostream& os, vtkIndent indent); 

  vtkSetMacro(TypeOfCompositing, int); //SetTypeOfCompositing(TypeOfCompositings)
  vtkGetMacro(TypeOfCompositing, int); //GetTypeOfCompositing(TypeOfCompositings)

  vtkSetMacro(TypeOfSampling, int); //SetTypeOfSampling(TypeOfSampling)
  vtkGetMacro(TypeOfSampling, int); //GetTypeOfSampling(TypeOfSampling)

  vtkSetMacro(AdvancedSampling, int); //SetAdvancedSampling(AdvancedSampling)
  vtkGetMacro(AdvancedSampling, int); //GetAdvancedSampling(AdvancedSampling)

  vtkSetMacro(StepSyze, double); //SetStepSyze(StepSyze)
  vtkGetMacro(StepSyze, double); //GetStepSyze(StepSyze)

  vtkSetMacro(TypeOfOutput, int); //SetTypeOfOutput(TypeOfOutput)
  vtkGetMacro(TypeOfOutput, int); //GetTypeOfOutput(TypeOfOutput)

	 vtkSetStringMacro(Property);
   vtkGetStringMacro(Property);

	 vtkSetStringMacro(AssaysID);
   vtkGetStringMacro(AssaysID);

	 vtkSetStringMacro(StartDepth);
   vtkGetStringMacro(StartDepth);

	 vtkSetStringMacro(EndDepth);
   vtkGetStringMacro(EndDepth);

  // Description:
   // Specify the point locations used to probe secondInput. Any geometry
   // can be used. New style. Equivalent to SetsecondInputConnection(1, algOutput).
   void SetSourceConnection(vtkAlgorithmOutput* algOutput);

   // Description:
   // Get a pointer to the source object.
   vtkPolyData *GetSource();

protected:
   vtkAdvancedCompositing();
   ~vtkAdvancedCompositing();

   virtual int FillInputPortInformation(int port, vtkInformation* info);

 
  // Description:
  // This is called by the superclass.
  // This is the method you should override.
  virtual int RequestInformation(vtkInformation *request, 
                                 vtkInformationVector **inputVector, 
                                 vtkInformationVector *outputVector);

   virtual int RequestData(vtkInformation *, 
                           vtkInformationVector **, 
                           vtkInformationVector *);

   int TypeOfCompositing;//segments or middle points
   int AdvancedSampling;//the sampling will start from the first assay
   int TypeOfSampling;//0 for downHolesCompositing  1 for benchHolesCompositing
   int TypeOfOutput;
   double StepSyze;
    
private:
	
	char* Property; 
	char* StartDepth; 
	char* EndDepth; 
	char* AssaysID;

   // Loop on points from input and add a scalar field 
   void ConstructOutput( vtkDataArray* listAugpt, vtkPolyData *input, vtkPolyData *output );
     
   vtkAdvancedCompositing(const vtkAdvancedCompositing&);  // Not implemented.
   void operator=(const vtkAdvancedCompositing&);  // Not implemented.
};


#endif
