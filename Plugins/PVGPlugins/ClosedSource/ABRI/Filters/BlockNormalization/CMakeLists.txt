PROJECT(BlockNormalization)
INCLUDE(${PARAVIEW_USE_FILE})


FIND_PACKAGE(Qt4)
IF(QT4_FOUND)

INCLUDE(${QT_USE_FILE})

INCLUDE_DIRECTORIES(
${VTK_INCLUDE_DIR}
${PARAVIEW_INCLUDE_DIRS}
${ParaView_SOURCE_DIR}/VTK/GUISupport/Qt
${pqComponents_SOURCE_DIR}
${pqComponents_BINARY_DIR}
${pqCore_SOURCE_DIR}
${pqCore_BINARY_DIR}
${QtWidgets_SOURCE_DIR}
${QtWidgets_BINARY_DIR}
${QtChart_SOURCE_DIR}
${QtChart_BINARY_DIR}
${QtTesting_SOURCE_DIR}
${QtTesting_BINARY_DIR}
${QtPython_SOURCE_DIR}
${QtPython_BINARY_DIR}
)

QT4_WRAP_CPP(MOC_SRCS pqBlockNormalization.h)

ADD_PARAVIEW_OBJECT_PANEL(IFACES IFACE_SRCS CLASS_NAME pqBlockNormalization
                         XML_NAME "Block Model Normalization" XML_GROUP filters)
                         
# create a plugin for this panel
ADD_PARAVIEW_PLUGIN(GUIBlockNormalization "1.0" 
                    GUI_INTERFACES ${IFACES} 
                    GUI_RESOURCES pqBlockNormalizationUI.qrc
                    SOURCES pqBlockNormalization.cxx ${MOC_SRCS} ${IFACE_SRCS})

ADD_PARAVIEW_PLUGIN(BlockNormalizationSMPlugin "1.0"
	SERVER_MANAGER_XML BlockNormalization.xml
	SERVER_MANAGER_SOURCES vtkBlockNormalization.cxx)

ADD_PARAVIEW_PLUGIN(BlockNormalizationGUIPlugin "1.0"
                        GUI_RESOURCE_FILES BlockNormalizationCategory.xml)
ENDIF(QT4_FOUND)


#CPACK CODE TO BUNDLE THE DLLS CORRECTLY
INSTALL(TARGETS BlockNormalizationGUIPlugin BlockNormalizationSMPlugin
	RUNTIME DESTINATION bin/plugins COMPONENT Runtime
	LIBRARY DESTINATION bin/plugins 
	ARCHIVE DESTINATION bin/plugins 
)