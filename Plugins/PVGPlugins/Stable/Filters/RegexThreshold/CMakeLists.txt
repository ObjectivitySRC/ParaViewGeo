PROJECT(RegexThreshold)
FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})

QT4_WRAP_CPP(MOC_SRCS pqRegexThreshold.h)

ADD_PARAVIEW_OBJECT_PANEL(IFACES IFACE_SRCS CLASS_NAME pqRegexThreshold
                          XML_NAME RegexThreshold XML_GROUP filters)
                         
# create a plugin for this panel
 ADD_PARAVIEW_PLUGIN(GUIRegexThreshold "1.0" 
                     GUI_INTERFACES ${IFACES} 
                     GUI_RESOURCES pqUI.qrc
                     SOURCES pqRegexThreshold.cxx ${MOC_SRCS} ${IFACE_SRCS})
                                           
                  
ADD_PARAVIEW_PLUGIN(RegexThresholdSMPlugin "1.0"                        
			SERVER_MANAGER_XML RegexThreshold.xml
			SERVER_MANAGER_SOURCES vtkRegexThreshold.cxx)
			#GUI_RESOURCE_FILES RegexThresholdCategory.xml)
                        

ADD_PARAVIEW_PLUGIN(RegexThresholdGUIPlugin "1.0"
                        GUI_RESOURCE_FILES RegexThresholdCategory.xml)
