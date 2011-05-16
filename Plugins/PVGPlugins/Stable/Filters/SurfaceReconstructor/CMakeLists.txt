PROJECT(SurfaceReconstructor)
FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})

ADD_PARAVIEW_PLUGIN(SurfaceReconstructorSMPlugin "1.0"
			SERVER_MANAGER_XML SurfaceReconstructor.xml
			SERVER_MANAGER_SOURCES vtkSurfaceReconstructor.cxx)

target_link_libraries(SurfaceReconstructorSMPlugin)

ADD_PARAVIEW_PLUGIN(SurfaceReconstructorGUIPlugin "1.0" 
			GUI_RESOURCE_FILES SurfaceReconstructorCategory.xml)

target_link_libraries(SurfaceReconstructorGUIPlugin)