PROJECT(DrillHolesLinesRefinement)
FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})






QT4_WRAP_CPP(MOC_SRCS pqDrillHolesPointsRefinement.h)
ADD_PARAVIEW_OBJECT_PANEL(IFACES IFACE_SRCS CLASS_NAME pqDrillHolesPointsRefinement
                         XML_NAME "Holes Points HierachicalDataSet" XML_GROUP filters)
# create a plugin for this panel
ADD_PARAVIEW_PLUGIN(GUIDrillHolesPointsRefinement "1.0" 
                    GUI_INTERFACES ${IFACES} 
                    GUI_RESOURCES pqDrillHolesPointsRefinementUI.qrc
                    SOURCES pqDrillHolesPointsRefinement.cxx ${MOC_SRCS} ${IFACE_SRCS})
target_link_libraries(GUIDrillHolesPointsRefinement)

ADD_PARAVIEW_PLUGIN(DrillHolesPointsRefinementSMPlugin "1.0"
			SERVER_MANAGER_XML DrillHolesPointsRefinement.xml
			SERVER_MANAGER_SOURCES vtkDrillHolesPointsRefinement.cxx )
			#GUI_RESOURCE_FILES DrillHolesPointsRefinementCategory.xml)
target_link_libraries(DrillHolesPointsRefinementSMPlugin)






ADD_PARAVIEW_PLUGIN(InterpolateHolesFilterSMPlugin "1.0"
			SERVER_MANAGER_XML InterpolateHolesFilter.xml
			SERVER_MANAGER_SOURCES vtkInterpolateHolesFilter.cxx )
			#GUI_RESOURCE_FILES InterpolateHolesFilterCategory.xml)
target_link_libraries(InterpolateHolesFilterSMPlugin)






QT4_WRAP_CPP(MOC_SRCS pqDrillHolesAdvancedRefinement.h)
ADD_PARAVIEW_OBJECT_PANEL(IFACES IFACE_SRCS CLASS_NAME pqDrillHolesAdvancedRefinement
                         XML_NAME "Advanced Holes Refinement" XML_GROUP filters)
# create a plugin for this panel
ADD_PARAVIEW_PLUGIN(GUIDrillHolesAdvancedRefinement "1.0" 
                    GUI_INTERFACES ${IFACES} 
                    GUI_RESOURCES pqDrillHolesAdvancedRefinementUI.qrc
                    SOURCES pqDrillHolesAdvancedRefinement.cxx ${MOC_SRCS} ${IFACE_SRCS})
target_link_libraries(GUIDrillHolesAdvancedRefinement)
ADD_PARAVIEW_PLUGIN(DrillHolesAdvancedRefinementSMPlugin "1.0"
			SERVER_MANAGER_XML DrillHolesAdvancedRefinement.xml
			SERVER_MANAGER_SOURCES vtkDrillHolesAdvancedRefinement.cxx vtkInterpolateHolesFilter.cxx )
			#GUI_RESOURCE_FILES DrillHolesAdvancedRefinementCategory.xml)
target_link_libraries(DrillHolesAdvancedRefinementSMPlugin)







QT4_WRAP_CPP(MOC_SRCS pqDrillHolesLinesRefinement.h)
ADD_PARAVIEW_OBJECT_PANEL(IFACES IFACE_SRCS CLASS_NAME pqDrillHolesLinesRefinement
                         XML_NAME "Holes Lines HierachicalDataSet" XML_GROUP filters)
# create a plugin for this panel
ADD_PARAVIEW_PLUGIN(GUIDrillHolesLinesRefinement "1.0" 
                    GUI_INTERFACES ${IFACES} 
                    GUI_RESOURCES pqDrillHolesLinesRefinementUI.qrc
                    SOURCES pqDrillHolesLinesRefinement.cxx ${MOC_SRCS} ${IFACE_SRCS})
target_link_libraries(GUIDrillHolesLinesRefinement)
ADD_PARAVIEW_PLUGIN(DrillHolesLinesRefinementSMPlugin "1.0"
			SERVER_MANAGER_XML DrillHolesLinesRefinement.xml
			SERVER_MANAGER_SOURCES vtkDrillHolesLinesRefinement.cxx vtkDrillHolesPointsRefinement.cxx 
						vtkInterpolateHolesFilter.cxx vtkDrillHolesAdvancedRefinement.cxx )
			#GUI_RESOURCE_FILES DrillHolesLinesRefinementCategory.xml)
target_link_libraries(DrillHolesLinesRefinementSMPlugin)



ADD_PARAVIEW_PLUGIN(DrillHolesLinesRefinementGUIPlugin "1.0"
                        GUI_RESOURCE_FILES DrillHolesLinesRefinementCategory.xml)

target_link_libraries(DrillHolesLinesRefinementGUIPlugin)