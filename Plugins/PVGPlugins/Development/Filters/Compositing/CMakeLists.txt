PROJECT(Compositing)
FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})

ADD_PARAVIEW_PLUGIN(InsidePointsSurfaceSMPlugin "1.0"
			SERVER_MANAGER_XML InsidePointsSurface.xml
			SERVER_MANAGER_SOURCES vtkInsidePointsSurface.cxx vtkPolyStruct.cxx)
			#GUI_RESOURCE_FILES InsidePointsSurfaceCategory.xml)

target_link_libraries(InsidePointsSurfaceSMPlugin)

ADD_PARAVIEW_PLUGIN(InsideLinesSurfaceSMPlugin "1.0"
			SERVER_MANAGER_XML InsideLinesSurface.xml
			SERVER_MANAGER_SOURCES vtkInsideLinesSurface.cxx)
			#GUI_RESOURCE_FILES InsideLinesSurfaceCategory.xml)

target_link_libraries(InsideLinesSurfaceSMPlugin)

ADD_PARAVIEW_PLUGIN(SliceCompositingSMPlugin "1.0"
			SERVER_MANAGER_XML SliceCompositing.xml
			SERVER_MANAGER_SOURCES vtkSliceCompositing.cxx)
			#GUI_RESOURCE_FILES SliceCompositingCategory.xml )

target_link_libraries(SliceCompositingSMPlugin)

ADD_PARAVIEW_PLUGIN(AdvancedCompositingSMPlugin "1.0"
			SERVER_MANAGER_XML AdvancedCompositing.xml
			SERVER_MANAGER_SOURCES vtkAdvancedCompositing.cxx vtkSliceCompositing.cxx)
			#GUI_RESOURCE_FILES AdvancedCompositingCategory.xml )

target_link_libraries(AdvancedCompositingSMPlugin)

ADD_PARAVIEW_PLUGIN(CompositingSMPlugin "1.0"
      SERVER_MANAGER_XML Compositing.xml
      SERVER_MANAGER_SOURCES vtkCompositing.cxx vtkAdvancedCompositing.cxx vtkInsideLinesSurface.cxx  vtkSliceCompositing.cxx  vtkInsidePointsSurface.cxx vtkPolyStruct.cxx)
      #GUI_RESOURCE_FILES CompositingCategory.xml)
      
target_link_libraries(CompositingSMPlugin)

ADD_PARAVIEW_PLUGIN(CompositingGUIPlugin "1.0"
                        GUI_RESOURCE_FILES CompositingCategory.xml)

target_link_libraries(CompositingGUIPlugin)