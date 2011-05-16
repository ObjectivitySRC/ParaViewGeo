PROJECT(DriftsAndStopesReconstruction)
FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})

INCLUDE_DIRECTORIES (../../../Stable/Filters/CleanCells)
INCLUDE_DIRECTORIES (../../../Stable/Filters/CleanCells/..)

ADD_PARAVIEW_PLUGIN(DriftsAndStopesReconstructionMPlugin "1.0"
			SERVER_MANAGER_XML DriftsAndStopesReconstruction.xml
			SERVER_MANAGER_SOURCES vtkDriftsAndStopesReconstruction.cxx ../../../Stable/Filters/CleanCells/vtkCleanCells.cpp)
			#GUI_RESOURCE_FILES DriftsAndStopesReconstructionCategory.xml)

target_link_libraries(DriftsAndStopesReconstructionMPlugin)

ADD_PARAVIEW_PLUGIN(DriftsAndStopesReconstructionGUIPlugin "1.0"
                        GUI_RESOURCE_FILES DriftsAndStopesReconstructionCategory.xml)

target_link_libraries(DriftsAndStopesReconstructionGUIPlugin)
