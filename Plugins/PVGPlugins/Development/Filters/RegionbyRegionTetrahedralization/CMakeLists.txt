PROJECT(RegionByRegionTetrahedralization)
FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})

SET(CMAKE_SUPPRESS_REGENERATION  1)


SET( EXTRA_SRCS
../../..//Stable/Filters/tetgen/vtkTetgen.cxx
../../../Stable/Filters/tetgen/tetgen.cxx
../../../Stable/Filters/tetgen/predicates.cxx)

ADD_PARAVIEW_PLUGIN(RegionByRegionTetrahedralizationMPlugin "1.0"
			SERVER_MANAGER_XML RegionByRegionTetrahedralization.xml
			SERVER_MANAGER_SOURCES vtkRegionByRegionTetrahedralization.cpp 
			SERVER_SOURCES ${EXTRA_SRCS})

target_link_libraries(RegionByRegionTetrahedralizationMPlugin)

ADD_PARAVIEW_PLUGIN(RegionByRegionTetrahedralizationGUIPlugin "1.0"
                        GUI_RESOURCE_FILES RegionByRegionTetrahedralizationCategory.xml)

#target_link_libraries(RegionByRegionTetrahedralizationGUIPlugin)