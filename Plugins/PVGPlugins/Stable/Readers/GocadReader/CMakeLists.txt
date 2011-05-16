PROJECT(GoCadReader)
FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})


ADD_PARAVIEW_PLUGIN(GocadReaderClientPlugin "1.0"
	GUI_RESOURCES GocadReader.qrc)

ADD_PARAVIEW_PLUGIN(GocadReaderServerManagerPlugin "1.0"
	SERVER_MANAGER_XML GocadReader.xml
	SERVER_MANAGER_SOURCES vtkGocadReader.cxx vtkGocadAtomic.cxx vtkGocadTSurf.cxx vtkGocadPLine.cxx 
		vtkGocadVSet.cxx vtkGocadTSolid.cxx vtkGocadVoxet.cxx vtkGocadSGrid.cxx) 

