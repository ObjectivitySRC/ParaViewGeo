/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkWriter.h,v $

  Copyright (c) Robert Maynard
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
//Robert Maynard, base class for all the gocad writing classes

#ifndef __vtkGocadBase_h
#define __vtkGocadBase_h

#include "vtkDataWriter.h"
#include "vtkStdString.h"

#define NO_VALUE -99999
#define TSURF_MODE 1
#define PLINE_MODE 2
#define VSET_MODE 3
#define TSURF_STRIP_MODE 4
#define TRIANGLE_POINTS 3
#define LINE_POINTS 2
#define METRES 1
#define FEET 2

class vtkStringArray;
//class vtkStdString;
class vtkPointData;

class VTK_EXPORT vtkGocadBase : public vtkDataWriter
{
public:
	static vtkGocadBase *New();
	vtkTypeRevisionMacro(vtkGocadBase,vtkDataWriter);
	void PrintSelf(ostream& os, vtkIndent indent);
	
	
	void SetHeaderName( vtkStdString headerName );
	
	virtual void SetX_Axis(int x) {this->X_Axis = x;}
	virtual void SetY_Axis(int y) {this->Y_Axis = y;}
	virtual void SetZ_Axis(int z) {this->Z_Axis = z;}
	virtual void SetAppend( bool append ) { this->Append = append; }	
	
	virtual void WriteProperties(ostream* fp, vtkStringArray* propNames, int pointId);

protected:
	vtkGocadBase();
	~vtkGocadBase();
  
  //write sections of the file
  virtual void WriteHeader(ostream* fp, char* objLabel, vtkStdString name);
  
	virtual vtkStdString FindFileName();
	virtual void FindFileName(vtkStdString &nameWithPath, vtkStdString &name);
	virtual vtkStdString GetUnitString();
	virtual char* StringToLower(const char*);
	

  //file IO
  virtual ostream* OpenFile( const char* filename );
  virtual void CloseFile( ostream* fp );
  
  //see declared modes above.  the file type specifies which output format we use.
	int FileType;
	
	vtkPointData* PointData;
		
	//see declared units above.  each axis can have a customized unit of measure
	int X_Axis;
	int Y_Axis;
	int Z_Axis;
	
	//precession of the files doubles
	int Precision;
	
	//what file mode to write use
	bool Append;
	
	//name for the header
	vtkStdString HeaderName;
  
private:
	vtkGocadBase(const vtkGocadBase&);  // Not implemented.
	void operator=(const vtkGocadBase&);  // Not implemented.
	  
};

#endif


