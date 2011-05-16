PROJECT(ComputeVolumes)
FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})

QT4_WRAP_CPP(MOC_SRCS pqComputeVolumes.h)

ADD_PARAVIEW_OBJECT_PANEL(IFACES IFACE_SRCS CLASS_NAME pqComputeVolumes
                         XML_NAME "Compute Volumes" XML_GROUP filters)

# create a plugin for this panel
ADD_PARAVIEW_PLUGIN(GUIComputeVolumes "1.0" 
                    GUI_INTERFACES ${IFACES} 
                    GUI_RESOURCES pqUI.qrc
                    SOURCES pqComputeVolumes.cxx ${MOC_SRCS} ${IFACE_SRCS})


ADD_PARAVIEW_PLUGIN(ComputeVolumesMPlugin "1.0"
			SERVER_MANAGER_XML ComputeVolumes.xml
			SERVER_MANAGER_SOURCES vtkComputeVolumes.cpp)
			#GUI_RESOURCE_FILES ComputeVolumesCategory.xml)

target_link_libraries(ComputeVolumesMPlugin)

ADD_PARAVIEW_PLUGIN(ComputeVolumesGUIPlugin "1.0"
                        GUI_RESOURCE_FILES ComputeVolumesCategory.xml)

target_link_libraries(ComputeVolumesGUIPlugin)