PROJECT(PointSetReader)
FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})

QT4_WRAP_CPP(MOC_SRCS pqPointSetReader.h)

ADD_PARAVIEW_OBJECT_PANEL(IFACES IFACE_SRCS CLASS_NAME pqPointSetReader
                         XML_NAME PointSetReader XML_GROUP sources)

ADD_PARAVIEW_PLUGIN(PointSetReaderCPlugin "1.0"
	GUI_RESOURCES PointSetReader.qrc)    
                         
# create a plugin for this panel
ADD_PARAVIEW_PLUGIN(PointSetReaderGUIPlugin "1.0" 
                    GUI_INTERFACES ${IFACES} 
                    GUI_RESOURCES pqUI.qrc
                    SOURCES pqPointSetReader.cxx ${MOC_SRCS} ${IFACE_SRCS})

ADD_PARAVIEW_PLUGIN(PointSetReaderSMPlugin "1.0"
	SERVER_MANAGER_XML PointSetReader.xml
	SERVER_MANAGER_SOURCES vtkPointSetReader.cpp)