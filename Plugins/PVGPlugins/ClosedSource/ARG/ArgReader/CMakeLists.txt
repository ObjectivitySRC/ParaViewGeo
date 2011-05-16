PROJECT(ArgReader)
FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})

FIND_PACKAGE(Qt4)
IF(QT4_FOUND)

INCLUDE(${QT_USE_FILE})
SET( EXTRA_SRCS
Activity.cxx
)



QT4_WRAP_CPP(MOC_SRCS pqArgReader.h)

ADD_PARAVIEW_OBJECT_PANEL(IFACES IFACE_SRCS CLASS_NAME pqArgReader
                         XML_NAME ArgReader XML_GROUP sources)
                         
ADD_PARAVIEW_PLUGIN(ArgReaderCPlugin "1.0"
	GUI_RESOURCES ArgReader.qrc)
  
ADD_PARAVIEW_PLUGIN(ArgReaderSMPlugin "1.0"
	SERVER_MANAGER_XML ArgReader.xml
	SERVER_MANAGER_SOURCES vtkArgReader.cxx
	SERVER_SOURCES ${EXTRA_SRCS}
)
	
ADD_PARAVIEW_PLUGIN(ArgReaderGUIPlugin "1.0" 
                    GUI_INTERFACES ${IFACES} 
                    GUI_RESOURCES pqUI.qrc
                    SOURCES pqArgReader.cxx ${MOC_SRCS} ${IFACE_SRCS})	

ENDIF(QT4_FOUND)
                    
#CPACK CODE TO BUNDLE THE DLLS CORRECTLY
INSTALL(TARGETS ArgReaderCPlugin ArgReaderSMPlugin ArgReaderGUIPlugin  
	RUNTIME DESTINATION bin/plugins COMPONENT Runtime
	LIBRARY DESTINATION bin/plugins 
	ARCHIVE DESTINATION bin/plugins 
)                    