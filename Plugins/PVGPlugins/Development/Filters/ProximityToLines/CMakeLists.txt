PROJECT(ProximityToLines)
FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})

#QT4_WRAP_CPP(MOC_SRCS pqProximityToLines.h)

#ADD_PARAVIEW_OBJECT_PANEL(IFACES IFACE_SRCS CLASS_NAME pqProximityToLines
#                         XML_NAME "MSC Drillholes Generator" XML_GROUP filters)

# create a plugin for this panel
#ADD_PARAVIEW_PLUGIN(GUIProximityToLines "1.0" 
#                    GUI_INTERFACES ${IFACES} 
#                    GUI_RESOURCES pqUI.qrc
#                    SOURCES pqProximityToLines.cpp ${MOC_SRCS} ${IFACE_SRCS})

ADD_PARAVIEW_PLUGIN(ProximityToLinesMPlugin "1.0"
			SERVER_MANAGER_XML ProximityToLines.xml
			SERVER_MANAGER_SOURCES vtkProximityToLines.cpp
			GUI_RESOURCE_FILES ProximityToLinesCategory.xml)

target_link_libraries(ProximityToLinesMPlugin)

