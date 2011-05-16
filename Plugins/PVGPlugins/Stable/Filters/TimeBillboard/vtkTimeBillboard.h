
#ifndef __vtkTimeBillboard_h
#define __vtkTimeBillboard_h

#include "vtkTableAlgorithm.h"
#define VTK_MSDATE 0
#define VTK_JULIAN 1
#define VTK_JULIAN_M 2
#define VTK_MATLAB 3

class VTK_EXPORT vtkTimeBillboard : public vtkTableAlgorithm
{
  public:
    static vtkTimeBillboard* New();
    vtkTypeRevisionMacro(vtkTimeBillboard, vtkTableAlgorithm);
    void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Get/Set the text string to generate in the output.
    vtkSetStringMacro(Text);
    vtkGetStringMacro(Text);
        
    vtkSetStringMacro(Format);
    vtkGetStringMacro(Format);    
               
  // Description:
  // Either scale by scalar or by vector/normal magnitude.
  vtkSetMacro(TimeFormat,int);
  vtkGetMacro(TimeFormat,int);
  void SetTimeFormatToMSDate()
    {this->SetTimeFormat(VTK_MSDATE);}; 
  void SetTimeFormatToJulian()
    {this->SetTimeFormat(VTK_JULIAN);};
  void SetTimeFormatToModifiedJulian()  
    {this->SetTimeFormat(VTK_JULIAN_M);};
  void SetTimeFormatToMatlab()
    {this->SetTimeFormat(VTK_MATLAB);};
  const char *GetTimeFormatAsString();               
  
  // Description:
  // When the component mode is UseSelected, this ivar indicated the selected
  // component. The default value is 0.
    vtkSetClampMacro(SelectedComponent,int,0,VTK_INT_MAX);
    vtkGetMacro(SelectedComponent,int);    
    
  protected:
    vtkTimeBillboard();
    ~vtkTimeBillboard();

    
    virtual int RequestData(vtkInformation* request, vtkInformationVector** inputVector, vtkInformationVector* outputVector);
    virtual int FillInputPortInformation(int port, vtkInformation* info);
    char* Format;
    char* Text;                        
    int SelectedComponent;
    int TimeFormat;
        
  
  private:
    vtkTimeBillboard(const vtkTimeBillboard&); // Not implemented
    void operator=(const vtkTimeBillboard&); // Not implemented
};

inline const char *vtkTimeBillboard::GetTimeFormatAsString(void)
{
  if ( this->TimeFormat == VTK_MSDATE )
    {
    return "MSDate";
    }
  else if ( this->TimeFormat == VTK_JULIAN ) 
    {
    return "Julian";
    }    
  else if ( this->TimeFormat == VTK_JULIAN_M ) 
    {
    return "Modified Julian";
    }
   else 
    {
    return "Matlab";
    }    
}

#endif

