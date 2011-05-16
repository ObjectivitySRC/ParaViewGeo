PROJECT(PVCollectDataSet)
FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})


INCLUDE_DIRECTORIES (../../../Stable/Filters/CleanCells)
INCLUDE_DIRECTORIES (../../../Stable/Filters/CleanCells/..)

ADD_PARAVIEW_PLUGIN(PVCollectDataSetSMPlugin "1.0"
      SERVER_MANAGER_XML PVCollectDataSet.xml
      SERVER_MANAGER_SOURCES vtkPVCollectDataSet.cxx ../../../Stable/Filters/CleanCells/vtkCleanCells.cpp)
          

target_link_libraries(PVCollectDataSetSMPlugin)

ADD_PARAVIEW_PLUGIN(PVCollectDataSetGUIPlugin "1.0"
                        GUI_RESOURCE_FILES PVCollectDataSetCategory.xml)


target_link_libraries(PVCollectDataSetGUIPlugin)