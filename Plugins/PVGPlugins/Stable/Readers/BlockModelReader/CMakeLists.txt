PROJECT(BlockModelReader)
FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})

ADD_PARAVIEW_PLUGIN(BlockModelReaderClientPlugin "1.0"
	GUI_RESOURCES BlockModelReader.qrc)

ADD_PARAVIEW_PLUGIN(BlockModelReaderServerManagerPlugin "1.0"
	SERVER_MANAGER_XML BlockModelReader.xml
	SERVER_MANAGER_SOURCES vtkBlockModelReader.cxx)

