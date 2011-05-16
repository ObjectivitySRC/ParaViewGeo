PROJECT(SOTResult)
FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})
	
QT4_WRAP_CPP(MOC_SRCS pqSOTResult.h) 

ADD_PARAVIEW_OBJECT_PANEL(IFACES IFACE_SRCS CLASS_NAME pqSOTResult
                         XML_NAME SOTResult XML_GROUP sources)
	
	# create a plugin for this panel
ADD_PARAVIEW_PLUGIN(GUISOTResult "1.0" 
                    GUI_INTERFACES ${IFACES} 
                    GUI_RESOURCES pqUI.qrc
                    SOURCES pqSOTResult.cxx ${MOC_SRCS} ${IFACE_SRCS})
                    
ADD_PARAVIEW_PLUGIN(SOTResultClientPlugin "1.0"
	GUI_RESOURCES pqSOTResult.qrc)

ADD_PARAVIEW_PLUGIN(SOTResultServerManagerPlugin "1.0"
	SERVER_MANAGER_XML SOTResult.xml
	SERVER_MANAGER_SOURCES vtkSOTResult.cxx)

target_link_libraries(SOTResultServerManagerPlugin)