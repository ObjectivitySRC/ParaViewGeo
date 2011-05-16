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

#ifndef __vtkDOTReader_h
#define __vtkDOTReader_h


#include "vtkPolyDataAlgorithm.h"

class vtkStdString;
class vtkSMDoubleVectorProperty;

struct DOT_Internal;

class VTK_EXPORT vtkDOTReader : public vtkPolyDataAlgorithm
{

public:
	static vtkDOTReader* New();
  vtkTypeRevisionMacro(vtkDOTReader,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  vtkSetStringMacro(FileName);
  vtkGetStringMacro(FileName);

  // Description:
  // Set the center of the point cloud.
  vtkSetVector3Macro(Center,double);
  vtkGetVectorMacro(Center,double,3);

  // Description:
  // Set the center of the point cloud.
  //vtkSetVector3Macro(Center1,double);
  //vtkGetVectorMacro(Center1,double,3);
	//void SetCenter1(vtkSMDoubleVectorProperty *v);
	//void vtkGetObjectMacro(Center1, vtkSMDoubleVectorProperty);
	void SetCenter1(double *c);

	char* GetFromServer()
	{
		return FromServer;
	}

protected:
  vtkDOTReader();
  ~vtkDOTReader();
   

  virtual int RequestData(vtkInformation* request, vtkInformationVector** inputVector,
		vtkInformationVector* outputVector);

	int RequestInformation(vtkInformation* request,
                          vtkInformationVector** inputVector,
                          vtkInformationVector* outputVector);

  char* FileName;
	char* FromServer;
	double Center[3];
	//double *Center1;
	vtkSMDoubleVectorProperty *Center1;

private:
  vtkDOTReader(const vtkDOTReader&);  // Not implemented.
  void operator=(const vtkDOTReader&);  // Not implemented.

	DOT_Internal *Internal;

};







#endif