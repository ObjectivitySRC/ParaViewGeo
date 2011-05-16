PROJECT(InsideClosedSurface)
FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})

ADD_PARAVIEW_PLUGIN(InsideClosedSurfaceSMPlugin "1.0"
			SERVER_MANAGER_XML InsideClosedSurface.xml
			SERVER_MANAGER_SOURCES vtkInsideClosedSurface.cxx
			)

target_link_libraries(InsideClosedSurfaceSMPlugin)

ADD_PARAVIEW_PLUGIN(InsideClosedSurfaceGUIPlugin "1.0"
                        GUI_RESOURCE_FILES InsideClosedSurfaceCategory.xml)

target_link_libraries(InsideClosedSurfaceGUIPlugin)