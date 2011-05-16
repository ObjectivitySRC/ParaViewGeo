PROJECT(ProximityThreshold)
FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})

ADD_PARAVIEW_PLUGIN(ProximityThresholdSMPlugin "1.0"
			SERVER_MANAGER_XML ProximityThreshold.xml
			SERVER_MANAGER_SOURCES vtkProximityThreshold.cxx)

target_link_libraries(ProximityThresholdSMPlugin)

ADD_PARAVIEW_PLUGIN(ProximityThresholdGUIPlugin "1.0" 
			GUI_RESOURCE_FILES ProximityThresholdCategory.xml)

target_link_libraries(ProximityThresholdGUIPlugin)