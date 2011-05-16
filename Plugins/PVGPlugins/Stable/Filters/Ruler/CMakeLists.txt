PROJECT(Ruler)

FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})


QT4_WRAP_CPP(MOC_SRCS pqRuler.h)
ADD_PARAVIEW_OBJECT_PANEL(IFACES IFACE_SRCS CLASS_NAME 	pqRuler
  XML_NAME "Ruler" XML_GROUP filters)

#create a plugin for this panel
ADD_PARAVIEW_PLUGIN(GUIRuler "1.0" 
                    GUI_INTERFACES ${IFACES} 
                    GUI_RESOURCES pqRulerUI.qrc
                    SOURCES pqRuler.cxx ${MOC_SRCS}
${IFACE_SRCS})


ADD_PARAVIEW_PLUGIN(RulerSMPlugin "1.0"
	SERVER_MANAGER_XML Ruler.xml
	SERVER_MANAGER_SOURCES vtkRuler.cxx)

ADD_PARAVIEW_PLUGIN(RulerGUIPlugin "1.0"
	GUI_RESOURCE_FILES RulerCategory.xml)