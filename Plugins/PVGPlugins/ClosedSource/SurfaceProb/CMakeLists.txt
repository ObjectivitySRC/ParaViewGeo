PROJECT(SurfaceProb)

FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})

#plugin
ADD_PARAVIEW_PLUGIN(SurfaceProbSMPlugin "1.0"
	SERVER_MANAGER_XML SurfaceProb.xml
	SERVER_MANAGER_SOURCES vtkSurfaceProb.cxx)

#category
ADD_PARAVIEW_PLUGIN(SurfaceProb_Category_Plugin "1.1" 
			GUI_RESOURCE_FILES SurfaceProbCategory.xml)
	

