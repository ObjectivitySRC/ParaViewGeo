PROJECT(CleanCells)
FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})

ADD_PARAVIEW_PLUGIN(CleanCellsMPlugin "1.0"
			SERVER_MANAGER_XML CleanCells.xml
			SERVER_MANAGER_SOURCES vtkCleanCells.cpp)
			#GUI_RESOURCE_FILES CleanCellsCategory.xml)

target_link_libraries(CleanCellsMPlugin)

ADD_PARAVIEW_PLUGIN(CleanCellsGUIPlugin "1.0"
                        GUI_RESOURCE_FILES CleanCellsCategory.xml)

target_link_libraries(CleanCellsGUIPlugin)