PROJECT(DrillholeGA)
FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})

#custom panel		
QT4_WRAP_CPP(MOC_SRCS pqDrillholeGA.h)
ADD_PARAVIEW_OBJECT_PANEL(IFACES IFACE_SRCS CLASS_NAME pqDrillholeGA
						  XML_NAME "DrillholeGA" XML_GROUP filters)
ADD_PARAVIEW_PLUGIN(DrillholeGA_GUI_Plugin "1.1"
					GUI_INTERFACES ${IFACES}
					GUI_RESOURCES pqDrillholeGA.qrc
					SOURCES pqDrillholeGA.cxx ${MOC_SRCS} ${IFACE_SRCS})

#plugin
ADD_PARAVIEW_PLUGIN(DrillholeGA_SM_Plugin "1.1"
			SERVER_MANAGER_XML DrillholeGA.xml
			SERVER_MANAGER_SOURCES vtkDrillholeGA.cxx
			SERVER_SOURCES GA.cxx)

#category
ADD_PARAVIEW_PLUGIN(DrillholeGA_Category_Plugin "1.1" 
			GUI_RESOURCE_FILES DrillholeGACategory.xml)
	

