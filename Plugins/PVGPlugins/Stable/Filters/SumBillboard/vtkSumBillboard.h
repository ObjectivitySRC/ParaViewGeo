
#ifndef __vtkSumBillboard_h
#define __vtkSumBillboard_h

#include "vtkTableAlgorithm.h"


class VTK_EXPORT vtkSumBillboard : public vtkTableAlgorithm
{
  public:
    static vtkSumBillboard* New();
    vtkTypeRevisionMacro(vtkSumBillboard, vtkTableAlgorithm);
    void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Get/Set the text string to generate in the output.
    vtkSetStringMacro(Format);
    vtkGetStringMacro(Format);
               
  
  // Description:
  // When the component mode is UseSelected, this ivar indicated the selected
  // component. The default value is 0.
    vtkSetClampMacro(SelectedComponent,int,0,VTK_INT_MAX);
    vtkGetMacro(SelectedComponent,int);    
    
  protected:
    vtkSumBillboard();
    ~vtkSumBillboard();

    
    virtual int RequestData(vtkInformation* request, vtkInformationVector** inputVector, vtkInformationVector* outputVector);
    virtual int FillInputPortInformation(int port, vtkInformation* info);
    char* Format;            
    int    SelectedComponent;
        
  
  private:
    vtkSumBillboard(const vtkSumBillboard&); // Not implemented
    void operator=(const vtkSumBillboard&); // Not implemented
};

#endif

