PROJECT(MapObjectAttributes)
FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})

#INCLUDE_DIRECTORIES (../PolyToStruct)
#INCLUDE_DIRECTORIES (../PolyToStruct/..)

#ADD_PARAVIEW_PLUGIN(MapObjectAttributesSMPlugin "1.0"
#			SERVER_MANAGER_XML MapObjectAttributes.xml
#			SERVER_MANAGER_SOURCES vtkMapAttributesToGrid.cxx
#			)

ADD_PARAVIEW_PLUGIN(MapObjectAttributesSMPlugin "1.0"
			SERVER_MANAGER_XML MapObjectAttributes.xml
			SERVER_MANAGER_SOURCES vtkMapObjectAttributes.cxx)


#ADD_PARAVIEW_PLUGIN(MapObjectAttributesSMPlugin "1.0"
#      SERVER_MANAGER_XML MapAttributes.xml
#      SERVER_MANAGER_SOURCES vtkMapAttributes.cxx vtkMapAttributesFromGrid.cxx vtkMapAttributesToGrid.cxx ../PolyToStruct/vtkPolyStruct.cxx)
      

ADD_PARAVIEW_PLUGIN(MapObjectAttributesGUIPlugin "1.0" 
                    GUI_RESOURCE_FILES MapObjectAttributesCategory.xml )
