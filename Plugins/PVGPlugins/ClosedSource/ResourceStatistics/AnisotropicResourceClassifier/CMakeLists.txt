PROJECT(AnisotropicResourceClassifier)

FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})

INCLUDE_DIRECTORIES (../AnisotropicProximityThreshold)
INCLUDE_DIRECTORIES (../AnisotropicProximityThreshold/..)

ADD_PARAVIEW_PLUGIN(AnisotropicResourceClassifierSMPlugin "1.0"
	SERVER_MANAGER_XML AnisotropicResourceClassifier.xml
	SERVER_MANAGER_SOURCES vtkAnisotropicResourceClassifier.cxx ../AnisotropicProximityThreshold/vtkAnisotropicProximityThreshold.cxx)

ADD_PARAVIEW_PLUGIN(AnisotropicResourceClassifierGUIPlugin "1.0"
                    GUI_RESOURCE_FILES AnisotropicResourceClassifierCategory.xml)