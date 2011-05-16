/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $ vtkSliceCompositing.h $
  Author:    Marie-Gabrielle Vallet
			 Arolde VIDJINNAGNI
	     
  Date:      JULY 24, 2008
  Version:   0.6

=========================================================================*/
// .NAME vtkSliceCompositing - 

#ifndef __vtkSliceCompositing_h
#define __vtkSliceCompositing_h
#include "vtkSetGet.h"
#include "vtkPolyDataAlgorithm.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkSmartPointer.h"
#include "vtkDataArray.h"

class VTK_EXPORT vtkSliceCompositing : public vtkPolyDataAlgorithm
{
public:
   //BTX
   typedef enum vtkPointSetDefinition { VERTEX, CELLCENTER };
   //ETX
   static vtkSliceCompositing *New();
   vtkTypeRevisionMacro(vtkSliceCompositing,vtkPolyDataAlgorithm);
   void PrintSelf(ostream& os, vtkIndent indent); 

   vtkSetMacro(TypeOfSampling, int); //SetTypeOfSampling(TypeOfSampling)
   vtkGetMacro(TypeOfSampling, int); //GetTypeOfSampling(TypeOfSampling)

   vtkSetMacro(AdvancedSampling, int); //SetAdvancedSampling(AdvancedSampling)
   vtkGetMacro(AdvancedSampling, int); //GetAdvancedSampling(AdvancedSampling)

   vtkSetMacro(StepSyze, double); //SetStepSyze(StepSyze)
   vtkGetMacro(StepSyze, double); //GetStepSyze(StepSyze)

   vtkSetMacro(TypeOfOutput, int); //SetTypeOfOutput(TypeOfOutput)
   vtkGetMacro(TypeOfOutput, int); //GetTypeOfOutput(TypeOfOutput)
  /*
	 vtkSetStringMacro(HoleID);
   vtkGetStringMacro(HoleID);
*/
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
   vtkSliceCompositing();
   ~vtkSliceCompositing();

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

  
   int AdvancedSampling;//the sampling will start from the first assay
   int TypeOfSampling;//0 for downHolesCompositing  1 for benchHolesCompositing
   int TypeOfOutput;
   double StepSyze;
private:

	//char* HoleID; 
	char* StartDepth; 
	char* EndDepth; 
	char* AssaysID;   //sampling function
   void ConstructSliceOutput( vtkPolyData *input, vtkPolyData *output );
   // Loop on points from input and add a scalar field 
  // void ConstructOutput( vtkDataArray* listAugpt, vtkPolyData *input, vtkPolyData *output );
     
   vtkSliceCompositing(const vtkSliceCompositing&);  // Not implemented.
   void operator=(const vtkSliceCompositing&);  // Not implemented.
};


#endif
