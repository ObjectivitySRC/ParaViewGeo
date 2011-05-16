PROJECT(ConvertPointsToBlocks)
FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})



QT4_WRAP_CPP(MOC_SRCS pqConvertPointsToBlocks.h)
ADD_PARAVIEW_OBJECT_PANEL(IFACES IFACE_SRCS CLASS_NAME 	pqConvertPointsToBlocks
                          XML_NAME ConvertPointsToBlocks XML_GROUP filters)


# create a plugin for this panel
ADD_PARAVIEW_PLUGIN(GUIConvertPointsToBlocks "1.0" 
                    GUI_INTERFACES ${IFACES} 
                    GUI_RESOURCES pqConvertPointsToBlocksUI.qrc
                    SOURCES pqConvertPointsToBlocks.cxx ${MOC_SRCS}
${IFACE_SRCS})


ADD_PARAVIEW_PLUGIN(ConvertPointsToBlocksSMPlugin "1.0"
	SERVER_MANAGER_XML ConvertPointsToBlocks.xml
	SERVER_MANAGER_SOURCES vtkConvertPointsToBlocks.cxx)


ADD_PARAVIEW_PLUGIN(ConvertPointsToBlocksGUIPlugin "1.0"
			GUI_RESOURCE_FILES ConvertPointsToBlocksCategory.xml)
