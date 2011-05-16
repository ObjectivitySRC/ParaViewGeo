PROJECT(MDelaunay2D)
FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})

ADD_PARAVIEW_PLUGIN(MDelaunay2DPlugin "1.0"
			SERVER_MANAGER_XML MDelaunay2D.xml
			SERVER_MANAGER_SOURCES vtkMDelaunay2D.cxx)
      #GUI_RESOURCE_FILES MDelaunay2DCategory.xml)

target_link_libraries(NMDelaunay2DSMPlugin)

ADD_PARAVIEW_PLUGIN(MDelaunay2DGUIPlugin "1.0"
                        GUI_RESOURCE_FILES MDelaunay2DCategory.xml)

target_link_libraries(MDelaunay2DGUIPlugin)