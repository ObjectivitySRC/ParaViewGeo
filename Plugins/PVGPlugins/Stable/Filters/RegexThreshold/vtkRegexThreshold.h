#ifndef __vtkRegexThreshold_h
#define __vtkRegexThreshold_h

#include "vtkThreshold.h"
#include "vtkStdString.h"
#include <sstream>

class vtkStringArray;

class VTK_EXPORT vtkRegexThreshold : public vtkThreshold
{
public:
  static vtkRegexThreshold *New();
  vtkTypeRevisionMacro(vtkRegexThreshold,vtkThreshold);
  void PrintSelf(ostream& os, vtkIndent indent);

  vtkSetStringMacro(Regex);
  vtkGetStringMacro(Regex);

protected:
  vtkRegexThreshold();
  ~vtkRegexThreshold();

	virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
  // Usual data generation method

  int EvaluateComponents( vtkDataArray *scalars, vtkIdType id );
  int EvaluateComponents( vtkStringArray *scalars, vtkIdType id );

  char* Regex;
  bool Search(vtkStdString& text);

private:
  vtkRegexThreshold(const vtkRegexThreshold&);  // Not implemented.
  void operator=(const vtkRegexThreshold&);  // Not implemented.
};

//BTX
template<class T>
vtkStdString vtkRegexConvert(T value)
{
  vtkstd::ostringstream buffer;
  buffer << value;

  //we do this in two steps, as it seems windows
  //has a problem with doing it in one step
  vtkStdString text = buffer.str();
  return text;
}
//ETX
#endif
