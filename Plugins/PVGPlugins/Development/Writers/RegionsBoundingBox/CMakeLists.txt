PROJECT(RegionsBoundingBox)
FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})

ADD_PARAVIEW_PLUGIN(RegionsBoundingBoxClientPlugin "1.0"
	GUI_RESOURCES RegionsBoundingBox.qrc)

ADD_PARAVIEW_PLUGIN(RegionsBoundingBoxServerManagerPlugin "1.0"
	SERVER_MANAGER_XML RegionsBoundingBox.xml
	SERVER_MANAGER_SOURCES vtkRegionsBoundingBox.cxx)