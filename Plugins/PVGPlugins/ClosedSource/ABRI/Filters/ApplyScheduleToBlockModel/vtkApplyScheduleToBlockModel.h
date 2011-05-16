
#ifndef __vtkApplyScheduleToBlockModel_h
#define __vtkApplyScheduleToBlockModel_h

#include "vtkUnstructuredGridAlgorithm.h"
#include <string>

//unix is not removed
#define VTK_MSDATE 0
#define VTK_JULIAN 1
#define VTK_JULIAN_M 2
#define VTK_MATLAB 3

using namespace std;

struct Internal;

class VTK_EXPORT vtkApplyScheduleToBlockModel : public vtkUnstructuredGridAlgorithm
{
public:

  static vtkApplyScheduleToBlockModel *New();
  vtkTypeRevisionMacro(vtkApplyScheduleToBlockModel,vtkUnstructuredGridAlgorithm);
	void PrintSelf(ostream& os, vtkIndent indent);

	vtkSetStringMacro(fileName);
	vtkGetStringMacro(fileName);

	vtkSetMacro(dateFormat_0, int);
	vtkSetMacro(dateFormat_1, int);
	vtkSetMacro(dateFormat_2, int);
	//vtkSetMacro(durationUnits, int);
	vtkSetMacro(DateType, int);

	vtkSetStringMacro(finishDatePropertyName);
	vtkGetStringMacro(finishDatePropertyName);

	vtkSetStringMacro(ScheduleSegName);
	vtkGetStringMacro(ScheduleSegName);

	vtkSetStringMacro(SegmentID);
	vtkGetStringMacro(SegmentID);

	double convertDate(string &date, string separator);

	void SetArraySelection(const char*name, int uniform, int time, int clone);

protected:
  vtkApplyScheduleToBlockModel();
  ~vtkApplyScheduleToBlockModel();
  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
  virtual int FillInputPortInformation(int port, vtkInformation *info);

	char* fileName;
	//char* ArraySelection;
	int dateFormat_0;
	int dateFormat_1;
	int dateFormat_2;
	char* ScheduleSegName;
	char* finishDatePropertyName;
	char *SegmentID;
	int durationUnits;
	int DateType;

	bool resetProperties;

private:
  vtkApplyScheduleToBlockModel(const vtkApplyScheduleToBlockModel&);  // Not implemented.
  void operator=(const vtkApplyScheduleToBlockModel&);  // Not implemented.

	Internal *internals;



};

#endif
