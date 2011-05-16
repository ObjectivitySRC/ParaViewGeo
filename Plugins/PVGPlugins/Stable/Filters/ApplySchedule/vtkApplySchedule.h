/*
   ParaView is a free software; you can redistribute it and/or modify it
   under the terms of the ParaView license version 1.2. 

   See License_v1.2.txt for the full ParaView license.
   A copy of this license can be obtained by contacting
   Kitware Inc.
   28 Corporate Drive
   Clifton Park, NY 12065
   USA

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*=========================================================================
MIRARCO MINING INNOVATION
Author: Nehme Bilal (nehmebilal@gmail.com)
Creation Date : December 2008
Modification date: 
===========================================================================*/

#ifndef __vtkApplySchedule_h
#define __vtkApplySchedule_h

#include "vtkDataSetAlgorithm.h"
#include "vtkStringArray.h"

//unix is not removed
#define VTK_MSDATE 0
#define VTK_JULIAN 1
#define VTK_JULIAN_M 2
#define VTK_MATLAB 3



struct Internal;

class VTK_EXPORT vtkApplySchedule : public vtkDataSetAlgorithm
{
public:

  static vtkApplySchedule *New();
  vtkTypeRevisionMacro(vtkApplySchedule,vtkDataSetAlgorithm);
	void PrintSelf(ostream& os, vtkIndent indent);

	vtkSetStringMacro(fileName);
	vtkGetStringMacro(fileName);

	vtkSetMacro(dateFormat_0, int);
	vtkSetMacro(dateFormat_1, int);
	vtkSetMacro(dateFormat_2, int);
	vtkSetMacro(durationUnits, int);
	vtkSetMacro(DateType, int);

	vtkSetStringMacro(DurationPropertyName);
	vtkGetStringMacro(DurationPropertyName);

	vtkSetStringMacro(finishDatePropertyName);
	vtkGetStringMacro(finishDatePropertyName);

	vtkSetStringMacro(ScheduleSegName);
	vtkGetStringMacro(ScheduleSegName);

	vtkSetStringMacro(SegmentID);
	vtkGetStringMacro(SegmentID);

	double convertDate(vtkStdString date, vtkStdString separator);

	void SetArraySelection(const char*name, int uniform, int time, int clone);

protected:
  vtkApplySchedule();
  ~vtkApplySchedule();
  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
  virtual int FillInputPortInformation(int port, vtkInformation *info);

	void parseProperties(ifstream& myFile);
	void distributeProperties();

	void removeWhiteSpaces(vtkStdString& s);


	char* fileName;
	//char* ArraySelection;
	int dateFormat_0;
	int dateFormat_1;
	int dateFormat_2;
	char* ScheduleSegName;
	char* DurationPropertyName;
	char* finishDatePropertyName;
	char *SegmentID;
	int durationUnits;
	int DateType;

	double durationFactor;

	bool resetProperties;

	vtkDataSet *input;
	vtkDataSet *output;

private:
  vtkApplySchedule(const vtkApplySchedule&);  // Not implemented.
  void operator=(const vtkApplySchedule&);  // Not implemented.

	Internal *internals;



};


//----------------------------------------------------------------------------
inline void vtkApplySchedule::removeWhiteSpaces(vtkStdString& s)
{
	int j = s.find_first_not_of(" ");
	int i = s.length() -1;
	if(i < 0)
		return;

	while(s.at(i) == ' ' && i>j)
	{
		--i;
	}
	s = s.substr(j,i-j+1);
}

#endif
