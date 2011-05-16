PROJECT(MineSightReader)
FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})

LINK_DIRECTORIES (./lib)
LINK_LIBRARIES (mintecMSDAL-vc8)


# On WIN32 machines, copy required .lib and .dll files to /bin/Debug
IF(WIN32)
	CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/lib/mintecMSDAL-vc8.lib ${LIBRARY_OUTPUT_PATH}/Debug/mintecMSDAL-vc8.lib COPYONLY)
	CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/lib/mintecMSDAL-vc8.dll ${LIBRARY_OUTPUT_PATH}/Debug/mintecMSDAL-vc8.dll COPYONLY)
ENDIF(WIN32)

ADD_PARAVIEW_PLUGIN(MineSightReaderClientPlugin "1.0"
	GUI_RESOURCES MineSightReader.qrc)

ADD_PARAVIEW_PLUGIN(MineSightReaderServerManagerPlugin "1.0"
	SERVER_MANAGER_XML MineSightReader.xml
	SERVER_MANAGER_SOURCES vtkMineSightReader.cxx)

