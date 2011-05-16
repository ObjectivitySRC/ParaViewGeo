PROJECT(ThresholdProcessId)
FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})


ADD_PARAVIEW_PLUGIN(ThresholdProcessIdSMPlugin "1.0"
      SERVER_MANAGER_XML ThresholdProcessId.xml
      SERVER_MANAGER_SOURCES vtkThresholdProcessId.cxx)
          

target_link_libraries(ThresholdProcessIdSMPlugin)

ADD_PARAVIEW_PLUGIN(ThresholdProcessIdGUIPlugin "1.0"
                        GUI_RESOURCE_FILES ThresholdProcessIdCategory.xml)


target_link_libraries(ThresholdProcessIdGUIPlugin)