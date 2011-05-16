PROJECT(RelativeTransform)
FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})

ADD_PARAVIEW_PLUGIN(RelativeTransformSMPlugin "1.0"
			SERVER_MANAGER_XML RelativeTransform.xml
			SERVER_MANAGER_SOURCES vtkRelativeTransform.cxx)

target_link_libraries(RelativeTransformSMPlugin)

ADD_PARAVIEW_PLUGIN(RelativeTransformGUIPlugin "1.0" 
			GUI_RESOURCE_FILES RelativeTransformCategory.xml)

target_link_libraries(RelativeTransformGUIPlugin)