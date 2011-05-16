PROJECT(Mine24DtoMap3D)
FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})

QT4_WRAP_CPP(MOC_SRCS pqMine24DtoMap3D.h)

ADD_PARAVIEW_OBJECT_PANEL(IFACES IFACE_SRCS CLASS_NAME pqMine24DtoMap3D
                         XML_NAME "Mine24D to Map3D" XML_GROUP filters)

# create a plugin for this panel
ADD_PARAVIEW_PLUGIN(GUIMine24DtoMap3D "1.0" 
                    GUI_INTERFACES ${IFACES} 
                    GUI_RESOURCES pqUI.qrc
                    SOURCES pqMine24DtoMap3D.cxx ${MOC_SRCS} ${IFACE_SRCS})

ADD_PARAVIEW_PLUGIN(Mine24DtoMap3DMPlugin "1.0"
			SERVER_MANAGER_XML Mine24DtoMap3D.xml
			SERVER_MANAGER_SOURCES vtkMine24DtoMap3D.cpp
			SOURCES Face.cpp Segments.cpp
			GUI_RESOURCE_FILES Mine24DtoMap3DCategory.xml)

target_link_libraries(Mine24DtoMap3DMPlugin)

