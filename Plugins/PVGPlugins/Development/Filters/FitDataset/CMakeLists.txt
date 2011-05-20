PROJECT(FitDataset)
FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})

#FILE(GLOB _tnt126_ "${CMAKE_CURRENT_SOURCE_DIR}/tnt_126/*.h")
#FILE(GLOB _jama125_ "${CMAKE_CURRENT_SOURCE_DIR}/jama125/*.h")


#SET( _TNT_JAMA_
#	${_tnt126_}
#	${_jama125_}
#	)

#INCLUDE_DIRECTORIES(
#./tnt_126
#./jama125
#)

ADD_PARAVIEW_PLUGIN(FitDatasetSMPlugin "1.0"
	SERVER_MANAGER_XML FitDataset.xml
	SERVER_MANAGER_SOURCES vtkFitDataset.cpp
	SERVER_SOURCES 	vtkConvexHull.cpp
					vtkLeatSquareFit.cpp
					Inc3dCHull_ConvexHull.cpp
					Inc3dCHull_Face.cpp
					Inc3dCHull_Edge.cpp
					Inc3dCHull_Vertex.cpp
					../../../common/vtkCoordinateSystemMapper.cpp
				)

ADD_PARAVIEW_PLUGIN(FitDatasetCPlugin "1.0"
	GUI_RESOURCE_FILES FitDatasetCategory.xml)


