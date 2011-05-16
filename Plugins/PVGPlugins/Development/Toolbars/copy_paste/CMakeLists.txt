FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE}) 

# Add SourceToolbar
QT4_WRAP_CPP(MOC_SRCS myToolbarActions.h)

ADD_PARAVIEW_ACTION_GROUP(IFACES IFACE_SRCS 
                          CLASS_NAME myToolbarActions
                          GROUP_NAME "ToolBar/myToolbar")

ADD_PARAVIEW_PLUGIN(myToolbar "1.0"
                    GUI_INTERFACES ${IFACES}
                    SOURCES ${MOC_SRCS} ${IFACE_SRCS} 
                    myToolbarActions.cxx)