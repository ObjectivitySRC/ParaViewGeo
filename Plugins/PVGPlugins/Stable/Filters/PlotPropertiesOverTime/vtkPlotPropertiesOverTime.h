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
===========================================================================*/

#ifndef __vtkPlotPropertiesOverTime_h
#define __vtkPlotPropertiesOverTime_h

#include "vtkRectilinearGridAlgorithm.h"

//BTX
class vtkDoubleArray;
class vtkFieldData;
class vtkIntArray;
class vtkDataArraySelection;
class vtkSMStringVectorProperty;
struct vtkEHInternals;
//ETX

// .NAME vtkPlotPropertiesOverTime - Extract histogram data (binned values) from any 
// dataset
// .SECTION Description
// vtkPlotPropertiesOverTime accepts any vtkDataSet as input and produces a
// vtkPolyData containing histogram data as output.  The output vtkPolyData
// will have contain a vtkDoubleArray named "bin_extents" which contains
// the boundaries between each histogram bin, and a vtkUnsignedLongArray
// named "bin_values" which will contain the value for each bin.

class VTK_EXPORT vtkPlotPropertiesOverTime : public vtkRectilinearGridAlgorithm
{
public:
  static vtkPlotPropertiesOverTime* New();
  vtkTypeRevisionMacro(vtkPlotPropertiesOverTime, vtkRectilinearGridAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);
 
  vtkPlotPropertiesOverTime();
  ~vtkPlotPropertiesOverTime();

	vtkSetStringMacro(MSDate_Property);
	vtkGetStringMacro(MSDate_Property);

	vtkSetStringMacro(Y_Property);
	vtkGetStringMacro(Y_Property);

	//vtkSetMacro(BinCount, int);
	vtkSetMacro(Days, int);
	vtkSetMacro(Months, int);
	vtkSetMacro(Years, int);

	//vtkGetObjectMacro(Properties, vtkSMStringVectorProperty);
  //int GetPropertiesArrayStatus(const char* name);
  //void SetPropertiesArrayStatus(const char* name, int status);  
	//int GetNumberOfProperties();
	//const char* GetPropertiesArrayName(int index);	

protected: 

  virtual int FillInputPortInformation (int port, vtkInformation *info);

  // convenience method
  virtual int RequestInformation(vtkInformation* request,
                                 vtkInformationVector** inputVector,
                                 vtkInformationVector* outputVector);

  virtual int RequestData(vtkInformation *request, 
                          vtkInformationVector **inputVector, 
                          vtkInformationVector *outputVector);


	char* MSDate_Property;
	char* Y_Property;
	int BinCount;
	int Days;
	int Months;
	int Years;

  
private:
  void operator=(const vtkPlotPropertiesOverTime&); // Not implemented
  vtkPlotPropertiesOverTime(const vtkPlotPropertiesOverTime&); // Not implemented
  
  int GetInputFieldAssociation();
  vtkFieldData* GetInputFieldData(vtkDataObject* input);
};

#endif
