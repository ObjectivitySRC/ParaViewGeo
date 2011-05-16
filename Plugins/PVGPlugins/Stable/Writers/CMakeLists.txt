PROJECT(StableWriters)
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
GocadWriter
SGeMSWriter
SOTWriter
)

SET( MIRARCO_SM_SRCS
GocadWriter/vtkGocadCompositeWriter.cxx
GocadWriter/vtkGocadBase.cxx
GocadWriter/vtkGocadWriter.cxx
GocadWriter/vtkGocadVoxetWriter.cxx
SGeMSWriter/vtkSGeMSWriter.cxx
SOTWriter/vtkSOTWriter.cxx
)

ADD_PARAVIEW_PLUGIN(StableWritersSMPlugin "1.0"
			SERVER_MANAGER_XML StableWriterSM.xml
			SERVER_MANAGER_SOURCES  ${MIRARCO_SM_SRCS}
			)
			
			
ADD_PARAVIEW_PLUGIN(StableWritersCPlugin "1.0"
	GUI_RESOURCE_FILES StableWriterClient.xml) #the name here cannot clash with any other xml files name!



#CPACK CODE TO BUNDLE THE DLLS CORRECTLY
INSTALL(TARGETS StableWritersCPlugin StableWritersSMPlugin
	RUNTIME DESTINATION bin/plugins COMPONENT Runtime
	LIBRARY DESTINATION bin/plugins 
	ARCHIVE DESTINATION bin/plugins 
  )
ENDIF(QT4_FOUND)