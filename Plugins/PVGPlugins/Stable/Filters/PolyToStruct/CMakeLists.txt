PROJECT(PolyToStruct)
FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})

ADD_PARAVIEW_PLUGIN(PolyStructSMPlugin "1.0"
			SERVER_MANAGER_XML PolyStruct.xml
			SERVER_MANAGER_SOURCES vtkPolyStruct.cxx)

target_link_libraries(PolyStructSMPlugin)

ADD_PARAVIEW_PLUGIN(PolyStructGUIPlugin "1.0"
                        GUI_RESOURCE_FILES PolyStructCategory.xml)

target_link_libraries(PolyStructGUIPlugin)