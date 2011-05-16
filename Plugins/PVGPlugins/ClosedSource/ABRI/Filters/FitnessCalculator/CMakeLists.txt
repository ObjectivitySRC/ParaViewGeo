PROJECT(FitnessCalculator)
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

QT4_WRAP_CPP(MOC_SRCS pqFitnessCalculator.h)

ADD_PARAVIEW_OBJECT_PANEL(IFACES IFACE_SRCS CLASS_NAME pqFitnessCalculator
                         XML_NAME "Fitness Calculator" XML_GROUP filters)
                         
# create a plugin for this panel
ADD_PARAVIEW_PLUGIN(GUIFitnessCalculator "1.0" 
                    GUI_INTERFACES ${IFACES} 
                    GUI_RESOURCES pqUI.qrc
                    SOURCES pqFitnessCalculator.cxx ${MOC_SRCS} ${IFACE_SRCS})

ADD_PARAVIEW_PLUGIN(FitnessCalculatorSMPlugin "1.0"
	SERVER_MANAGER_XML FitnessCalculator.xml
	SERVER_MANAGER_SOURCES vtkFitnessCalculator.cxx)

ADD_PARAVIEW_PLUGIN(FitnessCalculatorGUIPlugin "1.0"
                        GUI_RESOURCE_FILES FitnessCalculatorCategory.xml)
ENDIF(QT4_FOUND)

#CPACK CODE TO BUNDLE THE DLLS CORRECTLY
INSTALL(TARGETS FitnessCalculatorGUIPlugin FitnessCalculatorSMPlugin
	RUNTIME DESTINATION bin/plugins COMPONENT Runtime
	LIBRARY DESTINATION bin/plugins 
	ARCHIVE DESTINATION bin/plugins 
)