
#ifndef __vtkAnnotation_h
#define __vtkAnnotation_h

#include "vtkTableAlgorithm.h"


class VTK_EXPORT vtkAnnotation : public vtkTableAlgorithm
{
  public:
    static vtkAnnotation* New();
    vtkTypeRevisionMacro(vtkAnnotation, vtkTableAlgorithm);
    void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Get/Set the text string to generate in the output.
    vtkSetStringMacro(Format);
    vtkGetStringMacro(Format);
    
  protected:
    vtkAnnotation();
    ~vtkAnnotation();

    
    virtual int RequestData(vtkInformation* request, vtkInformationVector** inputVector, vtkInformationVector* outputVector);
    virtual int FillInputPortInformation(int port, vtkInformation* info);
    char* Format;
    int    AllScalars;
    int    AttributeMode;
    int    ComponentMode;
    int    SelectedComponent;
  
  private:
    vtkAnnotation(const vtkAnnotation&); // Not implemented
    void operator=(const vtkAnnotation&); // Not implemented
};

#endif

