PROJECT(TimeThreshold)
FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})

QT4_WRAP_CPP(MOC_SRCS pqTimeThreshold.h)

ADD_PARAVIEW_OBJECT_PANEL(IFACES IFACE_SRCS CLASS_NAME pqTimeThreshold
                         XML_NAME TimeThreshold XML_GROUP filters)
                         
# create a plugin for this panel
ADD_PARAVIEW_PLUGIN(GUITimeThreshold "1.0" 
                    GUI_INTERFACES ${IFACES} 
                    GUI_RESOURCES pqUI.qrc
                    SOURCES pqTimeThreshold.cxx ${MOC_SRCS} ${IFACE_SRCS})
                                                               
                  
ADD_PARAVIEW_PLUGIN(TimeThresholdSMPlugin "1.0"                        
			SERVER_MANAGER_XML TimeThreshold.xml
			SERVER_MANAGER_SOURCES vtkTimeThreshold.cxx
                        SOURCES TimeConverter.cxx)

ADD_PARAVIEW_PLUGIN(TimeThresholdGUIPlugin "1.0"
                        GUI_RESOURCE_FILES TimeThresholdCategory.xml)
