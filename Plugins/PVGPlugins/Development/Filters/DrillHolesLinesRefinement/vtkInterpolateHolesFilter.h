/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $ vtkInterpolateHolesFilter.h $
  Author:    Arolde VIDJINNAGNI
	     
  Date:      JULY 24, 2008
  Version:   0.6

=========================================================================*/
// .NAME vtkInterpolateHolesFilter - 

#ifndef __vtkInterpolateHolesFilter_h
#define __vtkInterpolateHolesFilter_h
#include "vtkSetGet.h"
#include "vtkPolyDataAlgorithm.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkSmartPointer.h"
#include "vtkDataArray.h"

class VTK_EXPORT vtkInterpolateHolesFilter : public vtkPolyDataAlgorithm
{
public:
   //BTX
   typedef enum vtkPointSetDefinition { VERTEX, CELLCENTER };
   //ETX
   static vtkInterpolateHolesFilter *New();
   vtkTypeRevisionMacro(vtkInterpolateHolesFilter,vtkPolyDataAlgorithm);
   void PrintSelf(ostream& os, vtkIndent indent); 

  vtkSetMacro(TypeOfInterpolation, int); //SetTypeOfInterpolation(TypeOfInterpolation)
  vtkGetMacro(TypeOfInterpolation, int); //GetTypeOfInterpolation(TypeOfInterpolation)

   // Description:
   // Specify the point locations used to probe secondInput. Any geometry
   // can be used. New style. Equivalent to SetsecondInputConnection(1, algOutput).
  // void SetSourceConnection(vtkAlgorithmOutput* algOutput);

protected:
   vtkInterpolateHolesFilter();
   ~vtkInterpolateHolesFilter();

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
  
   int TypeOfInterpolation;
   double curvilinearStep;
     
private:
   //sampling function
   void ConstructSliceOutput( vtkPolyData *input, vtkPolyData *output );
   // Loop on points from input and add a scalar field 
  // void ConstructOutput( vtkDataArray* listAugpt, vtkPolyData *input, vtkPolyData *output );
     
   vtkInterpolateHolesFilter(const vtkInterpolateHolesFilter&);  // Not implemented.
   void operator=(const vtkInterpolateHolesFilter&);  // Not implemented.
};


#endif
