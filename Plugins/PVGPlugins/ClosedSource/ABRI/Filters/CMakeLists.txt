PROJECT(ABRIFilters)
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
../../../Stable/Filters/SharedCode
FitnessCalculator 
StopesSelection 
ApplyScheduleToBlockModel 
BlockNormalization 
)

SET( ABRIFilters_SM_SRCS
FitnessCalculator/vtkFitnessCalculator.cxx
ApplyScheduleToBlockModel/vtkApplyScheduleToBlockModel.cpp
BlockNormalization/vtkBlockNormalization.cxx
StopesSelection/vtkStopesSelection.cxx
)

SET( ABRIFilters_EXTRA_SRCS
../../../Stable/Filters/SharedCode/TimeConverter.cxx
StopesSelection/vtkStopesResearch.cxx
StopesSelection/vtkBlockSubdivision.cxx
StopesSelection/Stope.cxx
StopesSelection/vtkCompleteBlockFilter.cxx

)

SET( ABRIFilters_GUI_SRCS
FitnessCalculator/pqFitnessCalculator.cxx
ApplyScheduleToBlockModel/pqApplyScheduleToBlockModel.cxx
BlockNormalization/pqBlockNormalization.cxx
StopesSelection/pqStopesSelection.cxx 
../../../Stable/Filters/SharedCode/TimeConverter.cxx
)

SET(MOC_LIST
FitnessCalculator/pqFitnessCalculator.h
ApplyScheduleToBlockModel/pqApplyScheduleToBlockModel.h
BlockNormalization/pqBlockNormalization.h
StopesSelection/pqStopesSelection.h
)

QT4_WRAP_CPP(MOC_SRCS ${MOC_LIST})

ADD_PARAVIEW_OBJECT_PANEL(IFACES1 IFACE_SRCS1 CLASS_NAME pqFitnessCalculator
                         XML_NAME "Fitness Calculator" XML_GROUP filters)

ADD_PARAVIEW_OBJECT_PANEL(IFACES2 IFACE_SRCS2 CLASS_NAME pqApplyScheduleToBlockModel
                         XML_NAME "ApplySchedule(BlockModel)" XML_GROUP filters)

ADD_PARAVIEW_OBJECT_PANEL(IFACES3 IFACE_SRCS3 CLASS_NAME pqBlockNormalization
                         XML_NAME "Block Model Normalization" XML_GROUP filters)

ADD_PARAVIEW_OBJECT_PANEL(IFACES4 IFACE_SRCS4 CLASS_NAME pqStopesSelection
                         XML_NAME StopesSelection XML_GROUP filters)
                   
    
LIST(APPEND IFACES ${IFACES1})
LIST(APPEND IFACES ${IFACES2})
LIST(APPEND IFACES ${IFACES3})
LIST(APPEND IFACES ${IFACES4})
LIST(APPEND IFACE_SRCS ${IFACE_SRCS1})
LIST(APPEND IFACE_SRCS ${IFACE_SRCS2})
LIST(APPEND IFACE_SRCS ${IFACE_SRCS3})
LIST(APPEND IFACE_SRCS ${IFACE_SRCS4})


ADD_PARAVIEW_PLUGIN(ABRIFiltersSMPlugin "1.0"
			SERVER_MANAGER_XML ABRISFSM.xml
			SERVER_MANAGER_SOURCES  ${ABRIFilters_SM_SRCS}
			SERVER_SOURCES  ${ABRIFilters_EXTRA_SRCS}
			)

ADD_PARAVIEW_PLUGIN(ABRIFiltersGUIPlugin "1.0"
             GUI_INTERFACES ${IFACES}
             GUI_RESOURCES ABRIGUI.qrc
             GUI_SOURCES ${ABRIFilters_GUI_SRCS} ${MOC_SRCS} ${IFACE_SRCS}
			)

ADD_PARAVIEW_PLUGIN(ABRIFiltersCPlugin  "1.0" 
			GUI_RESOURCE_FILES ABRISFCategory.xml)			

ENDIF(QT4_FOUND)


#CPACK CODE TO BUNDLE THE DLLS CORRECTLY
INSTALL(TARGETS ABRIFiltersGUIPlugin ABRIFiltersSMPlugin ABRIFiltersCPlugin  
	RUNTIME DESTINATION bin/plugins COMPONENT Runtime
	LIBRARY DESTINATION bin/plugins 
	ARCHIVE DESTINATION bin/plugins 
)
