PROJECT(ApplyScheduleToBlockModel)
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
../../../../Stable/Filters/SharedCode/
../../../../Stable/Filters/SharedCode/..
)


QT4_WRAP_CPP(MOC_SRCS pqApplyScheduleToBlockModel.h)

ADD_PARAVIEW_OBJECT_PANEL(IFACES IFACE_SRCS CLASS_NAME pqApplyScheduleToBlockModel
                         XML_NAME "ApplySchedule(BlockModel)" XML_GROUP filters)
                         
# create a plugin for this panel
ADD_PARAVIEW_PLUGIN(GUIApplyScheduleToBlockModel "1.0" 
                    GUI_INTERFACES ${IFACES} 
                    GUI_RESOURCES pqUI.qrc
                    SOURCES pqApplyScheduleToBlockModel.cxx ${MOC_SRCS} ${IFACE_SRCS})                         

ADD_PARAVIEW_PLUGIN(ApplyScheduleToBlockModelSMPlugin "1.0"
			SERVER_MANAGER_XML ApplyScheduleToBlockModel.xml
			SERVER_MANAGER_SOURCES vtkApplyScheduleToBlockModel.cpp
			SOURCES ../../../../Stable/Filters/SharedCode/TimeConverter.cxx)
			

ADD_PARAVIEW_PLUGIN(ApplyScheduleToBlockModelGUIPlugin "1.0"
                        GUI_RESOURCE_FILES ApplyScheduleToBlockModelCategory.xml)

ENDIF(QT4_FOUND)

#CPACK CODE TO BUNDLE THE DLLS CORRECTLY
INSTALL(TARGETS ApplyScheduleToBlockModelGUIPlugin ApplyScheduleToBlockModelSMPlugin
	RUNTIME DESTINATION bin/plugins COMPONENT Runtime
	LIBRARY DESTINATION bin/plugins 
	ARCHIVE DESTINATION bin/plugins 
)