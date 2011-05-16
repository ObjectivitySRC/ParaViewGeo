PROJECT(PVDistanceCellsToDrillhole)
FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})




QT4_WRAP_CPP(MOC_SRCS pqPVDistanceCellsToDrillhole.h)
ADD_PARAVIEW_OBJECT_PANEL(IFACES IFACE_SRCS CLASS_NAME 	pqPVDistanceCellsToDrillhole
                          XML_NAME "Distance To Drillhole" XML_GROUP filters)


# create a plugin for this panel
ADD_PARAVIEW_PLUGIN(GUIPVDistanceCellsToDrillhole "1.0" 
                    GUI_INTERFACES ${IFACES} 
                    GUI_RESOURCES pqPVDistanceCellsToDrillholeUI.qrc
                    SOURCES pqPVDistanceCellsToDrillhole.cxx ${MOC_SRCS} ${IFACE_SRCS})


ADD_PARAVIEW_PLUGIN(PVDistanceCellsToDrillholeSMPlugin "1.0"
			SERVER_MANAGER_XML PVDistanceCellsToDrillhole.xml
			SERVER_MANAGER_SOURCES vtkPVDistanceCellsToDrillhole.cxx 
			                       vtkDistanceCellsToDrillhole.cxx )


ADD_PARAVIEW_PLUGIN(PVDistanceCellsToDrillholeGUIPlugin "1.0"
			GUI_RESOURCE_FILES PVDistanceCellsToDrillholeCategory.xml)


