PROJECT(IsotropicResourceClassifier)

FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})

INCLUDE_DIRECTORIES (../../../Stable/Filters/ProximityThreshold)
INCLUDE_DIRECTORIES (../../../Stable/Filters/ProximityThreshold/..)

ADD_PARAVIEW_PLUGIN(IsotropicResourceClassifierSMPlugin "1.0"
	SERVER_MANAGER_XML IsotropicResourceClassifier.xml
	SERVER_MANAGER_SOURCES vtkIsotropicResourceClassifier.cxx ../../../Stable/Filters/ProximityThreshold/vtkProximityThreshold.cxx)

ADD_PARAVIEW_PLUGIN(IsotropicResourceClassifierGUIPlugin "1.0"
                    GUI_RESOURCE_FILES IsotropicResourceClassifierCategory.xml)