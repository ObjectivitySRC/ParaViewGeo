PROJECT(ApplySchedule)
FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})

QT4_WRAP_CPP(MOC_SRCS pqApplySchedule.h)

ADD_PARAVIEW_OBJECT_PANEL(IFACES IFACE_SRCS CLASS_NAME pqApplySchedule
                         XML_NAME "Apply Schedule" XML_GROUP filters)
                         
# create a plugin for this panel
ADD_PARAVIEW_PLUGIN(GUIApplySchedule "1.0" 
                    GUI_INTERFACES ${IFACES} 
                    GUI_RESOURCES pqUI.qrc
                    SOURCES pqApplySchedule.cxx ${MOC_SRCS} ${IFACE_SRCS})                         

ADD_PARAVIEW_PLUGIN(ApplyScheduleMPlugin "1.0"
			SERVER_MANAGER_XML ApplySchedule.xml
			SERVER_MANAGER_SOURCES vtkApplySchedule.cpp)
			#GUI_RESOURCE_FILES ApplyScheduleCategory.xml)

target_link_libraries(ApplyScheduleMPlugin)

ADD_PARAVIEW_PLUGIN(ApplyScheduleGUIPlugin "1.0"
                        GUI_RESOURCE_FILES ApplyScheduleCategory.xml)

target_link_libraries(ApplyScheduleGUIPlugin)