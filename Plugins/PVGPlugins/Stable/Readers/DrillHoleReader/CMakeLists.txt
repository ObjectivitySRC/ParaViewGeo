PROJECT(DrillholeReader)
FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})

QT4_WRAP_CPP(MOC_SRCS pqDrillHoleReader.h)

ADD_PARAVIEW_OBJECT_PANEL(IFACES IFACE_SRCS CLASS_NAME pqDrillHoleReader
                         XML_NAME DrillHoleReader XML_GROUP sources)
                         
# create a plugin for this panel
ADD_PARAVIEW_PLUGIN(GUIDrillHoleReader "1.0" 
                    GUI_INTERFACES ${IFACES} 
                    GUI_RESOURCES pqUI.qrc
                    SOURCES pqDrillHoleReader.cxx ${MOC_SRCS} ${IFACE_SRCS})

ADD_PARAVIEW_PLUGIN(DrillholeReaderClientPlugin "1.0"
	GUI_RESOURCES DrillHoleReader.qrc)

ADD_PARAVIEW_PLUGIN(DrillHoleSMP "1.0"
	SERVER_MANAGER_XML DrillHoleReader.xml
	SERVER_MANAGER_SOURCES vtkDrillHoleReader.cxx)

target_link_libraries(DrillHoleSMP)
