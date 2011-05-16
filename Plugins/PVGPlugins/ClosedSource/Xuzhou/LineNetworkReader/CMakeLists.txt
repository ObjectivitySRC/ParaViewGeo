PROJECT(LineNetworkReader)
FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})

QT4_WRAP_CPP(MOC_SRCS pqLineNetworkReader.h)

ADD_PARAVIEW_OBJECT_PANEL(IFACES IFACE_SRCS CLASS_NAME pqLineNetworkReader
                         XML_NAME LineNetworkReader XML_GROUP sources)

ADD_PARAVIEW_PLUGIN(LineNetworkReaderClientPlugin "1.0"
	GUI_RESOURCES LineNetworkReader.qrc)    
                         
# create a plugin for this panel
ADD_PARAVIEW_PLUGIN(GUILineNetworkReader "1.0" 
                    GUI_INTERFACES ${IFACES} 
                    GUI_RESOURCES pqUI.qrc
                    SOURCES pqLineNetworkReader.cxx ${MOC_SRCS} ${IFACE_SRCS})



ADD_PARAVIEW_PLUGIN(LineNetworkReaderSMP "1.0"
	SERVER_MANAGER_XML LineNetworkReader.xml
	SERVER_MANAGER_SOURCES vtkLineNetworkReader.cpp)

