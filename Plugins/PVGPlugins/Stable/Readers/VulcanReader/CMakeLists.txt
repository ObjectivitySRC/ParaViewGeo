PROJECT(VulcanReader)
FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})

ADD_PARAVIEW_PLUGIN(VulcanReaderClientPlugin "1.0"
	GUI_RESOURCES VulcanReader.qrc)

ADD_PARAVIEW_PLUGIN(VulcanMineSMPlugin "1.0"
	SERVER_MANAGER_XML VulcanReader.xml
	SERVER_MANAGER_SOURCES vtkVulcanTriReader.cxx vtkVulcanGridReader.cxx vtkVulcanBlockModelReader.cxx)
	
LINK_DIRECTORIES ( lib )
	
target_link_libraries (VulcanMineSMPlugin mtkExportedDGD )
target_link_libraries (VulcanMineSMPlugin mtkExportedBlockModel )
target_link_libraries (VulcanMineSMPlugin mtkExportedGrid )
target_link_libraries (VulcanMineSMPlugin mtkExportedTri )

