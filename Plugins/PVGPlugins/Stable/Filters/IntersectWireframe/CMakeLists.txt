PROJECT(IntersectFilter)
FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})

ADD_PARAVIEW_PLUGIN(IntersectFilterSMPlugin "1.0"
			SERVER_MANAGER_XML IntersectFilter.xml
			SERVER_MANAGER_SOURCES vtkIntersectFilter.cxx vtkIntersectWireframe.cxx vtkThresholdProcessId.cxx)

target_link_libraries(IntersectFilterSMPlugin)

ADD_PARAVIEW_PLUGIN(IntersectFilterGUIPlugin "1.0"
			GUI_RESOURCE_FILES IntersectFilterCategory.xml)

target_link_libraries(IntersectFilterGUIPlugin)
