PROJECT(Annotation)
FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})

ADD_PARAVIEW_PLUGIN(AnnotateSMPlugin "1.0"
			SERVER_MANAGER_XML Annotation.xml
			SERVER_MANAGER_SOURCES vtkAnnotation.cxx ../tetgen/vtkTetgen.cxx ../ComputeVolumes/vtkComputeVolumes.cpp
			SERVER_SOURCES ../tetgen/tetgen.cxx ../tetgen/predicates.cxx)

target_link_libraries(AnnotateSMPlugin)