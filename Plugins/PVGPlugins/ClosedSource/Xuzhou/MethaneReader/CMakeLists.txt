PROJECT(MethaneReader)
FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})

ADD_PARAVIEW_PLUGIN(MethaneReaderClientPlugin "1.0"
	GUI_RESOURCES MethaneReader.qrc)

ADD_PARAVIEW_PLUGIN(MethaneReaderServerManagerPlugin "1.0"
	SERVER_MANAGER_XML MethaneReader.xml
	SERVER_MANAGER_SOURCES vtkMethaneReader.cxx)

TARGET_LINK_LIBRARIES(MethaneReaderServerManagerPlugin)