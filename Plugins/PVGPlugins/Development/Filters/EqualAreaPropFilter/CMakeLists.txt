PROJECT(EqualAreaPropFilter)
FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})

QT4_WRAP_CPP(MOC_SRCS pqEqualAreaPropFilter.h)

ADD_PARAVIEW_OBJECT_PANEL(IFACES IFACE_SRCS CLASS_NAME pqEqualAreaPropFilter
                         XML_NAME EqualAreaPropFilter XML_GROUP filters)
                         
# create a plugin for this panel
ADD_PARAVIEW_PLUGIN(GUIEqualAreaPropFilter "1.0" 
                    GUI_INTERFACES ${IFACES} 
                    GUI_RESOURCES pqUI.qrc
                    SOURCES pqEqualAreaPropFilter.cxx ${MOC_SRCS} ${IFACE_SRCS})
                                                               
ADD_PARAVIEW_PLUGIN(EqualAreaPropFilterSMPlugin "1.0"
			SERVER_MANAGER_XML EqualAreaPropFilter.xml
			SERVER_MANAGER_SOURCES vtkEqualAreaPropFilter.cxx)

target_link_libraries(EqualAreaPropFilterSMPlugin)

