PROJECT(VectorTransform)
FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})

ADD_PARAVIEW_PLUGIN(VectorTransformSMPlugin "1.0"
			SERVER_MANAGER_XML VectorTransform.xml
			SERVER_MANAGER_SOURCES vtkVectorTransform.cxx)

target_link_libraries(vtkVectorTransform)

