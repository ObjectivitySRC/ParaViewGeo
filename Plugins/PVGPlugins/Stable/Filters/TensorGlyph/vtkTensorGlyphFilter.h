

#ifndef __vtkTensorGlyphFilter_h
#define __vtkTensorGlyphFilter_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkSmartPointer.h"
#include "vtkDataArray.h"
#include "vtkPolyData.h"
#include "vtkDataSet.h"

#include "vtkTensorGlyph.h""

class VTK_EXPORT vtkTensorGlyphFilter : public vtkTensorGlyph
{
public:
  
   static vtkTensorGlyphFilter *New();
   vtkTypeRevisionMacro(vtkTensorGlyphFilter,vtkTensorGlyph);


protected:
   vtkTensorGlyphFilter();
   ~vtkTensorGlyphFilter();


   virtual int RequestData(vtkInformation *, 
                           vtkInformationVector **, 
                           vtkInformationVector *);
};


#endif
