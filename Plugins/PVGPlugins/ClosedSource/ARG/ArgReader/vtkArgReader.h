/*=========================================================================

  Program:   ParaView
  Module:    $RCSfile: vtkArgReader.h,v $

  Copyright (c) Robert Maynard
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkArgReader - Reader for multi-block datasets
// .SECTION Description
// vtkArgReader reads the meta-file that point to a list
// of serial VTK files.

#ifndef __vtkArgReader_h
#define __vtkArgReader_h

#include "vtkXMLCompositeDataReader.h"
#include "vtkStdString.h"
#include "Activity.h"
#include <vtkstd/map>
#include "vtkDataArraySelection.h"

class vtkKdTree;
class vtkPolyData;
class vtkMultiBlockDataSet;

class vtkInformation;
class vtkCallbackCommand;
class vtkDataArraySelection;

struct Internal_Properties; //use to store the properties from the PUNO file or DOT file
struct Internal_filesNames;
class VTK_EXPORT vtkArgReader : public vtkXMLCompositeDataReader
{
public:
  static vtkArgReader* New();
  vtkTypeRevisionMacro(vtkArgReader,vtkXMLCompositeDataReader);
  void PrintSelf(ostream& os, vtkIndent indent);	
	
  void SetArraySelection(const char*name, int status);
	vtkSetStringMacro(ArraysInfo);
	vtkGetStringMacro(ArraysInfo);

	//Allow user to set StartDate
  vtkSetStringMacro(StartDate);
  vtkGetStringMacro(StartDate);

protected:
  vtkArgReader();
  ~vtkArgReader();  
	
	virtual int RequestInformation(vtkInformation *, vtkInformationVector **,
                      vtkInformationVector *);

  virtual void UpdateDataSelection();

  // Read the XML element for the subtree of a the composite dataset.
  // dataSetIndex is used to rank the leaf nodes in an inorder traversal.
  virtual void ReadComposite(vtkXMLDataElement* element, 
    vtkCompositeDataSet* composite, const char* filePath, 
    unsigned int &dataSetIndex);

  // Get the name of the data set being read.
  virtual const char* GetDataSetName();

  virtual int FillOutputPortInformation(int, vtkInformation* info);
	

	//BTX
	enum ActivityType{ UNDEFINED=-1, DECLINE, DEVELOPMENT, STOPES };
	//ETX

	virtual void SplitFileName(); //use to get the pathname
	virtual void CreatePunoFilesName();
	virtual void CreatePunoFiles();
	virtual bool isNumeric(vtkStdString value);

private:
  vtkArgReader(const vtkArgReader&);  // Not implemented.
  void operator=(const vtkArgReader&);  // Not implemented.

	//BTX
	void GetXMLFilePath(  vtkXMLDataElement* xmlElem, const char* filePath, vtkStdString *&prop  );
	//ETX

	void CreateDevelopmentGeometry( vtkPolyData *dev );
	void CreateDeclineGeometry( vtkPolyData *decline );
	void CreateStopeGeometry( vtkPolyData *stopes );

	void CreateDeclineActivities( ActivityStorage &storage );
	void CreateDevelopmentActivities( ActivityStorage &storage );
	void CreateStopeActivities( ActivityStorage &storage );

	//BTX
	void DumpStopesToFile( vtkstd::ofstream &ofile );
	void DumpDevelopmentToFile( vtkstd::ofstream &ofile );
	void DumpDeclineToFile( vtkstd::ofstream &ofile );
	//ETX

	//method needed by CreateDeclineActivities
	//BTX
	void CreateDevelopmentRelations( ActivityStorage &storage, vtkstd::multimap< vtkStdString, Activity* > &guidMap);
	//ETX

	//method for creating the outputFile.
	void WriteOutputFile( ActivityStorage &storage );
	
	Internal_filesNames* PunoLinksListOfFileName;
	Internal_filesNames* PunoPointsListOfFileName;
	//ugh I hate having tons of class vars like this
	vtkStdString PunoLinksFileName;
	vtkStdString PunoPointsFileName;
	vtkStdString DotAscFileName;
	vtkStdString DotRptFileName;
	vtkStdString DataMineFileName;
	vtkStdString OutFileName;	

	char* ArraysInfo;
	
	vtkDataArraySelection* ArgCellDataArraySelection;	

	//allows the user to set the start date for the activity output file
	char* StartDate;
	char* PathName;

	Internal_Properties *PUNOProperties;
	Internal_Properties *DOTProperties;

	vtkStdString DMHeaders;
	vtkStdString DotHeaders;
	vtkStdString PunoHeaders;

	bool DMstatus;
	bool Dotstatus;
	bool Punostatus;
	
};

#endif

