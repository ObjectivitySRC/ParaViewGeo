PROJECT(DXFReader)
FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})

ADD_PARAVIEW_PLUGIN(DXFReaderClientPlugin "1.0"
	GUI_RESOURCES DXFReader.qrc)

ADD_PARAVIEW_PLUGIN(DXFReaderServerManagerPlugin "1.0"
	SERVER_MANAGER_XML DXFReader.xml
	SERVER_MANAGER_SOURCES vtkDXFReader.cxx
	SOURCES vtkDXFObjectMap.h vtkDXFObject.cxx vtkDXFLayer.cxx vtkDXFLayer.h vtkDXFParser.cxx vtkDXFParser.h vtkDXFBlock.cxx vtkDXFBlock.h)