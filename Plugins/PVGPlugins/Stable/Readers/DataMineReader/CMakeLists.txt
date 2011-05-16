PROJECT(DataMineReader)
FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})


#WIREFRAME READER
QT4_WRAP_CPP(MOC_SRCS pqDataMineWireFrameReader.h)

ADD_PARAVIEW_OBJECT_PANEL(IFACES IFACE_SRCS CLASS_NAME pqDataMineWireFrameReader
                    XML_NAME DataMineWireFrameReader XML_GROUP sources)
                         
# create a plugin for this panel
ADD_PARAVIEW_PLUGIN(GUIDMWireFrameReader "1.0" 
                    GUI_INTERFACES ${IFACES} 
                    GUI_RESOURCES pqDMWireFrame.qrc
                    SOURCES pqDataMineWireFrameReader.cxx ${MOC_SRCS} ${IFACE_SRCS})


ADD_PARAVIEW_PLUGIN(DMReadersCP "1.0"
	GUI_RESOURCES DMReader.qrc)
  

ADD_PARAVIEW_PLUGIN(DMReadersSMP "1.0"
	SERVER_MANAGER_XML DMReader.xml
	SERVER_MANAGER_SOURCES vtkDataMineWireFrameReader.cxx vtkDataMineDummyReader.cxx vtkDataMinePerimeterReader.cxx  vtkDataMinePointReader.cxx  vtkDataMineBlockReader.cxx vtkDataMineDrillHoleReader.cxx  vtkDataMineReader.cxx 
	SOURCES  dmfile.cxx PropertyStorage.cxx PointMap.cxx  )