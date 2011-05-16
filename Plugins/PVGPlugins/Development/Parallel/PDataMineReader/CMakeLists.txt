PROJECT(PDataMineReader)
FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})


INCLUDE_DIRECTORIES (../../../Stable/Readers/DataMineReader)
INCLUDE_DIRECTORIES (../../../Stable/Readers/DataMineReader/..)



ADD_PARAVIEW_PLUGIN(PDMReaderCP "1.0"
	GUI_RESOURCES PDMReader.qrc)
  

ADD_PARAVIEW_PLUGIN(PDMReaderSMP "1.0"
	SERVER_MANAGER_XML PDMReader.xml
	SERVER_MANAGER_SOURCES vtkPDataMineReader.cxx
												 vtkPDataMineBlockReader.cxx 
												 vtkPDataMineDrillHoleReader.cxx
												 vtkPDataMinePerimeterReader.cxx
												 vtkPDataMineWireFrameReader.cxx
												 vtkPDataMinePointReader.cxx 
												 vtkPDataMineDummyReader.cxx 
												../../../Stable/Readers/DataMineReader/vtkDataMineWireFrameReader.cxx 
												../../../Stable/Readers/DataMineReader/vtkDataMineDummyReader.cxx 
												../../../Stable/Readers/DataMineReader/vtkDataMinePerimeterReader.cxx 
												../../../Stable/Readers/DataMineReader/vtkDataMinePointReader.cxx  
												../../../Stable/Readers/DataMineReader/vtkDataMineBlockReader.cxx 
												../../../Stable/Readers/DataMineReader/vtkDataMineDrillHoleReader.cxx
												../../../Stable/Readers/DataMineReader/vtkDataMineReader.cxx
												
												
	SOURCES ../../../Stable/Readers/DataMineReader/dmfile.cxx
					../../../Stable/Readers/DataMineReader/PointMap.cxx
					../../../Stable/Readers/DataMineReader/PropertyStorage.cxx)
