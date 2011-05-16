PROJECT(PlotPropertiesOverTime)
FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})



QT4_WRAP_CPP(MOC_SRCS pqPlotPropertiesOverTime.h pqPlotPropertiesOverTimePanel.h pqLabelFormatter.h)

ADD_PARAVIEW_OBJECT_PANEL(IFACES2 IFACE_SRCS2 CLASS_NAME pqPlotPropertiesOverTimePanel
                         XML_NAME "Plot Properties Over Time" XML_GROUP filters)
 
ADD_PARAVIEW_VIEW_MODULE(IFACES IFACE_SRCS VIEW_TYPE pqPlotPropertiesOverTime 
						#VIEW_XML_NAME MirarcoBarChart 
						VIEW_XML_GROUP views
                         )
                         
LIST(APPEND IFACES ${IFACES2})
LIST(APPEND IFACE_SRCS ${IFACE_SRCS2})

ADD_PARAVIEW_PLUGIN(GUIpqPlotPropertiesOverTime "1.0" 
					GUI_INTERFACES ${IFACES}
					GUI_SOURCES pqPlotPropertiesOverTime.cpp 
					GUI_RESOURCES pqUI.qrc
					SOURCES pqPlotPropertiesOverTimePanel.cpp pqLabelFormatter.cpp ../SharedCode/TimeConverter.cxx
                    ${MOC_SRCS} ${IFACE_SRCS})


ADD_PARAVIEW_PLUGIN(PlotPropertiesOverTimeMPlugin "1.0"
			SERVER_MANAGER_XML PlotPropertiesOverTime.xml
			SERVER_MANAGER_SOURCES vtkPlotPropertiesOverTime.cpp
			GUI_RESOURCE_FILES PlotPropertiesOverTimeCategory.xml)

#target_link_libraries(PlotPropertiesOverTimeGUIPlugin)