PROJECT(TimeBillboard)
FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})
                                                             
ADD_PARAVIEW_PLUGIN(TimeBillboardSMPlugin "1.0"                        
			SERVER_MANAGER_XML TimeBillboard.xml
			SERVER_MANAGER_SOURCES vtkTimeBillboard.cxx
                        SOURCES TimeConverter.cxx)
target_link_libraries(TimeBillboardSMPlugin)

