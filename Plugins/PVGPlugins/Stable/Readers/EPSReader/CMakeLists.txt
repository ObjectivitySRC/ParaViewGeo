PROJECT(EPSReader)
FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})

#removed as the python code is now inline
#SET(app_SRC_CXX _vtkEPSReader.cxx)
#SET(app_DEST_CXX vtkEPSReader.cxx)



#FILE(READ ${app_SRC_CXX} temp)
#IF (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
#  FILE(WRITE ${app_DEST_CXX} "#define PYTHON_FILE \"" ${PROJECT_SOURCE_DIR} "/\"\n ")
#ELSE (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
#  FILE(WRITE ${app_DEST_CXX} "#define PYTHON_FILE \"" ${PROJECT_SOURCE_DIR} "/\"\n")
#ENDIF (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
#FILE(APPEND ${app_DEST_CXX} "${temp}")

QT4_WRAP_CPP(MOC_SRCS pqEPSReader.h)

ADD_PARAVIEW_OBJECT_PANEL(IFACES IFACE_SRCS CLASS_NAME pqEPSReader
                          XML_NAME SOTEPSReader XML_GROUP sources)
                         
# create a plugin for this panel
 ADD_PARAVIEW_PLUGIN(GUIEPSReader "1.0" 
                     GUI_INTERFACES ${IFACES} 
                     GUI_RESOURCES pqUI.qrc
                     SOURCES pqEPSReader.cxx ${MOC_SRCS} ${IFACE_SRCS})

ADD_PARAVIEW_PLUGIN(EPSReaderClientPlugin "0.3"
	GUI_RESOURCES EPSReader.qrc)

ADD_PARAVIEW_PLUGIN(EPSReaderServerManagerPlugin "1.0"
	SERVER_MANAGER_XML EPSReader.xml
	SERVER_MANAGER_SOURCES vtkEPSReader.cxx)

