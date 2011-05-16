PROJECT(BoundingBox)
FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})

QT4_WRAP_CPP(MOC_SRCS pqBoundingBox.h)

ADD_PARAVIEW_OBJECT_PANEL(IFACES IFACE_SRCS CLASS_NAME pqBoundingBox
                         XML_NAME "Bounding Box" XML_GROUP filters)

# create a plugin for this panel
ADD_PARAVIEW_PLUGIN(GUIBoundingBox "1.0" 
                    GUI_INTERFACES ${IFACES} 
                    GUI_RESOURCES pqUI.qrc
                    SOURCES pqBoundingBox.cxx ${MOC_SRCS} ${IFACE_SRCS})

ADD_PARAVIEW_PLUGIN(BoundingBoxMPlugin "1.0"
			SERVER_MANAGER_XML BoundingBox.xml
			SERVER_MANAGER_SOURCES vtkBBox.cpp)
			#GUI_RESOURCE_FILES BoundingBoxCategory.xml)

ADD_PARAVIEW_PLUGIN(BoundingBoxGUIPlugin "1.0"
                        GUI_RESOURCE_FILES BoundingBoxCategory.xml)

