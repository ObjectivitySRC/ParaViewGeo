PROJECT(PVIntersectFilter)
FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})


INCLUDE_DIRECTORIES (../PVThresholdProcessId)
INCLUDE_DIRECTORIES (../PVThresholdProcessId/..)


ADD_PARAVIEW_PLUGIN(PVIntersectFilterSMPlugin "1.0"
			SERVER_MANAGER_XML PVIntersectFilter.xml
			SERVER_MANAGER_SOURCES vtkPVIntersectFilter.cxx vtkIntersectWireframe.cxx ../PVThresholdProcessId/vtkThresholdProcessId.cxx)


ADD_PARAVIEW_PLUGIN(PVIntersectFilterGUIPlugin "1.0"
			GUI_RESOURCE_FILES PVIntersectFilterCategory.xml)


