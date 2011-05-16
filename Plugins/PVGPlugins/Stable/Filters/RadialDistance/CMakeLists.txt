PROJECT(RadialDistance)
FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})

ADD_PARAVIEW_PLUGIN(RadialDistanceSMPlugin "1.0"
			SERVER_MANAGER_XML RadialDistance.xml
			SERVER_MANAGER_SOURCES vtkRadialDistance.cxx)

target_link_libraries(RadialDistanceSMPlugin)

ADD_PARAVIEW_PLUGIN(RadialDistanceGUIPlugin "1.0" 
			GUI_RESOURCE_FILES RadialDistanceCategory.xml)

target_link_libraries(RadialDistanceGUIPlugin)