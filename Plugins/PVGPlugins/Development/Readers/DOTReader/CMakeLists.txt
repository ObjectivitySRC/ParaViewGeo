PROJECT(DOTReader)
FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})


QT4_WRAP_CPP(MOC_SRCS pqDOTReader.h pqDOTReaderPointWidget.h)


ADD_PARAVIEW_OBJECT_PANEL(IFACES IFACE_SRCS CLASS_NAME pqDOTReader
                         XML_NAME DOTReader XML_GROUP sources)

ADD_PARAVIEW_PLUGIN(DOTReaderClientPlugin "1.0"
	GUI_RESOURCES DOTReader.qrc)    
                       

# create a plugin for this panel
ADD_PARAVIEW_PLUGIN(GUIDOTReader "1.0" 
                    GUI_INTERFACES ${IFACES} 
                    GUI_RESOURCES pqUI.qrc
                    GUI_SOURCES pqDOTReader.cxx
                    SOURCES pqDOTReaderPointWidget.cpp
                    ${MOC_SRCS} ${IFACE_SRCS})


ADD_PARAVIEW_PLUGIN(DOTReaderSMP "1.0"
	SERVER_MANAGER_XML DOTReader.xml
	SERVER_MANAGER_SOURCES vtkDOTReader.cpp)

