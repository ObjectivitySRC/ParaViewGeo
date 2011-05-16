PROJECT(Tetgen)
FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})

QT4_WRAP_CPP(MOC_SRCS pqTetgen.h)

ADD_PARAVIEW_OBJECT_PANEL(IFACES IFACE_SRCS CLASS_NAME pqTetgen
                         XML_NAME "Volumique Tetrahedralization" XML_GROUP filters)

# create a plugin for this panel
ADD_PARAVIEW_PLUGIN(GUITetgen "1.0" 
                    GUI_INTERFACES ${IFACES} 
                    GUI_RESOURCES pqUI.qrc
                    SOURCES pqTetgen.cxx ${MOC_SRCS} ${IFACE_SRCS})

ADD_PARAVIEW_PLUGIN(TetgenMPlugin "1.0"
			SERVER_MANAGER_XML Tetgen.xml
			SERVER_MANAGER_SOURCES vtkTetgen.cxx
			GUI_RESOURCE_FILES TetgenCategory.xml
			SERVER_SOURCES tetgen.cxx predicates.cxx)


