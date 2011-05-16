PROJECT(SumBillboard)
FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})

QT4_WRAP_CPP(MOC_SRCS pqSumBillboardPanel.h)

ADD_PARAVIEW_OBJECT_PANEL(IFACES IFACE_SRCS CLASS_NAME pqSumBillboardPanel
                         XML_NAME SumBillboard XML_GROUP filters)
                         
# create a plugin for this panel
ADD_PARAVIEW_PLUGIN(GUISumBillboard "1.0" 
                    GUI_INTERFACES ${IFACES} 
                    GUI_RESOURCES pqUI.qrc
                    SOURCES pqSumBillboardPanel.cxx ${MOC_SRCS} ${IFACE_SRCS})
                                           
                  
ADD_PARAVIEW_PLUGIN(SumBillboardSMPlugin "1.0"                        
			SERVER_MANAGER_XML SumBillboard.xml
			SERVER_MANAGER_SOURCES vtkSumBillboard.cxx)

