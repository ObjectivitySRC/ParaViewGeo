PROJECT(Interpolation)
FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})





ADD_PARAVIEW_PLUGIN(InterpolateHolesFilterSMPlugin "1.0"
			SERVER_MANAGER_XML InterpolateHolesFilter.xml
			SERVER_MANAGER_SOURCES vtkInterpolateHolesFilter.cxx )
			#GUI_RESOURCE_FILES InterpolateHolesFilterCategory.xml)
target_link_libraries(InterpolateHolesFilterSMPlugin)


ADD_PARAVIEW_PLUGIN(InterpolateHolesFilterGUIPlugin "1.0"
                        GUI_RESOURCE_FILES InterpolateHolesFilterCategory.xml)

