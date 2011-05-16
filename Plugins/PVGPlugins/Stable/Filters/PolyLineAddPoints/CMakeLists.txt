PROJECT(PolyLineAddPoints)
FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})

ADD_PARAVIEW_PLUGIN(PolyLineAddPointsPlugin "1.0"
			SERVER_MANAGER_XML PolyLineAddPoints.xml
			SERVER_MANAGER_SOURCES vtkPolyLineAddPoints.cxx)
      #GUI_RESOURCE_FILES MDelaunay2DCategory.xml)

#target_link_libraries(NMDelaunay2DSMPlugin)

ADD_PARAVIEW_PLUGIN(PolyLineAddPointsGUIPlugin "1.0"
                        GUI_RESOURCE_FILES PolyLineAddPointsCategory.xml)

#target_link_libraries(MDelaunay2DGUIPlugin)