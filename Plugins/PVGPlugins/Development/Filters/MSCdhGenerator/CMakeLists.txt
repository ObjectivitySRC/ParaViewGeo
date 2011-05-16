PROJECT(MSCdhGenerator)
FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})

QT4_WRAP_CPP(MOC_SRCS pqMSCdhGenerator.h)

ADD_PARAVIEW_OBJECT_PANEL(IFACES IFACE_SRCS CLASS_NAME pqMSCdhGenerator
                         XML_NAME "MSC Drillholes Generator" XML_GROUP filters)

# create a plugin for this panel
ADD_PARAVIEW_PLUGIN(GUIMSCdhGenerator "1.0" 
                    GUI_INTERFACES ${IFACES} 
                    GUI_RESOURCES pqUI.qrc
                    SOURCES pqMSCdhGenerator.cpp ${MOC_SRCS} ${IFACE_SRCS})

ADD_PARAVIEW_PLUGIN(MSCdhGeneratorMPlugin "1.0"
			SERVER_MANAGER_XML MSCdhGenerator.xml
			SERVER_MANAGER_SOURCES vtkMSCdhGenerator.cpp
			GUI_RESOURCE_FILES MSCdhGeneratorCategory.xml)

target_link_libraries(MSCdhGeneratorMPlugin)

