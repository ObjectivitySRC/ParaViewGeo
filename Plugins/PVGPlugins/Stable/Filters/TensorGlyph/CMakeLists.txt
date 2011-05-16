PROJECT(TensorGlyphFilter)
FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})

ADD_PARAVIEW_PLUGIN(TensorGlyphFilterSMPlugin "1.0"
			SERVER_MANAGER_XML TensorGlyphFilter.xml
			SERVER_MANAGER_SOURCES vtkTensorGlyphFilter.cpp )			

ADD_PARAVIEW_PLUGIN(TensorGlyphFilterGUIPlugin "1.0"
                        GUI_RESOURCE_FILES TensorGlyphFilterCategory.xml)

