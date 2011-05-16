PROJECT(AnisotropicProximityThreshold)

FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})

ADD_PARAVIEW_PLUGIN(AnisotropicProximityThresholdSMPlugin "1.0"
	SERVER_MANAGER_XML AnisotropicProximityThreshold.xml
	SERVER_MANAGER_SOURCES vtkAnisotropicProximityThreshold.cxx)

ADD_PARAVIEW_PLUGIN(AnisotropicProximityThresholdGUIPlugin "1.0" 
  GUI_RESOURCE_FILES AnisotropicProximityThresholdCategory.xml)