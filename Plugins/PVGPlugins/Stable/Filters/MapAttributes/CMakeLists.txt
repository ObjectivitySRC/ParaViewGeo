PROJECT(MapAttributes)
FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})

INCLUDE_DIRECTORIES (../PolyToStruct)
INCLUDE_DIRECTORIES (../PolyToStruct/..)

ADD_PARAVIEW_PLUGIN(MapAttributesToGridSMPlugin "1.0"
			SERVER_MANAGER_XML MapAttributesToGrid.xml
			SERVER_MANAGER_SOURCES vtkMapAttributesToGrid.cxx
			)

ADD_PARAVIEW_PLUGIN(MapAttributesFromGridSMPlugin "1.0"
			SERVER_MANAGER_XML MapAttributesFromGrid.xml
			SERVER_MANAGER_SOURCES vtkMapAttributesFromGrid.cxx)


ADD_PARAVIEW_PLUGIN(MapAttributesSMPlugin "1.0"
      SERVER_MANAGER_XML MapAttributes.xml
      SERVER_MANAGER_SOURCES vtkMapAttributes.cxx vtkMapAttributesFromGrid.cxx vtkMapAttributesToGrid.cxx ../PolyToStruct/vtkPolyStruct.cxx)
      

ADD_PARAVIEW_PLUGIN(MapAttributesGUIPlugin "1.0" 
                    GUI_RESOURCE_FILES MapAttributesCategory.xml )
