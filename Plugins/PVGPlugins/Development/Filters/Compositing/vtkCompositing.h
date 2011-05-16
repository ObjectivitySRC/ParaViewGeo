/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $ vtkCompositing.h $
  Author:    Arolde VIDJINNAGNI
	     
  Date:      JUNE 24, 2008
  Version:   0.1

=========================================================================*/
// .NAME vtkCompositing - 

#ifndef __vtkCompositing_h
#define __vtkCompositing_h
#include "vtkSetGet.h"
#include "vtkPolyDataAlgorithm.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkSmartPointer.h"
#include "vtkDataArray.h"

class VTK_EXPORT vtkCompositing : public vtkPolyDataAlgorithm
{
public:
   //BTX
   typedef enum vtkPointSetDefinition { VERTEX, CELLCENTER };
   //ETX
   static vtkCompositing *New();
   vtkTypeRevisionMacro(vtkCompositing,vtkPolyDataAlgorithm);
   void PrintSelf(ostream& os, vtkIndent indent); 

  vtkSetMacro(TypeCompositing, int); //SetTypeCompositing(TypeCompositings)

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
   vtkCompositing();
   ~vtkCompositing();

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

   int TypeCompositing;
    
private:
	
	char* Property; 
	char* StartDepth; 
	char* EndDepth; 
	char* AssaysID;

   // Loop on points from input and add a scalar field 
   void ConstructOutput( vtkDataArray* listAugpt, vtkPolyData *input, vtkPolyData *output );
     
   vtkCompositing(const vtkCompositing&);  // Not implemented.
   void operator=(const vtkCompositing&);  // Not implemented.
};


#endif
