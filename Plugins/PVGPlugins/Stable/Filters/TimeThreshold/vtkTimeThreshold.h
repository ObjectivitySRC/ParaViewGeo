#ifndef __vtkTimeThreshold_h
#define __vtkTimeThreshold_h

#include "vtkThreshold.h"
#include "vtkUnstructuredGridAlgorithm.h"

//unix is not removed
#define VTK_MSDATE 0
#define VTK_JULIAN 1
#define VTK_JULIAN_M 2
#define VTK_MATLAB 3


class VTK_EXPORT vtkTimeThreshold : public vtkUnstructuredGridAlgorithm
{
public:
  static vtkTimeThreshold *New();
  vtkTypeRevisionMacro(vtkTimeThreshold,vtkUnstructuredGridAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);
                
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
           
  vtkSetStringMacro(UpperChar);
  vtkGetStringMacro(UpperChar);
  
  vtkSetStringMacro(LowerChar);
  vtkGetStringMacro(LowerChar);

	// Description:
  // If using scalars from point data, all scalars for all points in a cell 
  // must satisfy the threshold criterion if AllScalars is set. Otherwise, 
  // just a single scalar value satisfying the threshold criterion enables
  // will extract the cell.
  vtkSetMacro(AllScalars,int);
  vtkGetMacro(AllScalars,int);
  vtkBooleanMacro(AllScalars,int);

protected:
  vtkTimeThreshold();  
	~vtkTimeThreshold(); 

  // Usual data generation method
  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
  virtual int FillInputPortInformation(int port, vtkInformation *info);
  void Convert();
     
  int TimeFormat;
  char* UpperChar;
  char* LowerChar;
	vtkThreshold* Threshold;
	double LowerThreshold;
	double UpperThreshold;
	int    AllScalars;
      
  
private:
  vtkTimeThreshold(const vtkTimeThreshold&);  // Not implemented.
  void operator=(const vtkTimeThreshold&);  // Not implemented.
};

inline const char *vtkTimeThreshold::GetTimeFormatAsString(void)
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
