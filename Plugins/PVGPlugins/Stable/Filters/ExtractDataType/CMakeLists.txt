PROJECT(ExtractDataType)
FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})

ADD_PARAVIEW_PLUGIN(ExtractPolyDataServerManagerPlugin "1.0"
			SERVER_MANAGER_XML ExtractPolyData.xml
			SERVER_MANAGER_SOURCES vtkExtractPolyData.cxx)
			
ADD_PARAVIEW_PLUGIN(ExtractUnstructuredGridServerManagerPlugin "1.0"
			SERVER_MANAGER_XML ExtractUnstrucGrid.xml
			SERVER_MANAGER_SOURCES vtkExtractUnstrucGrid.cxx)

ADD_PARAVIEW_PLUGIN(ExtractDataTypeGUIPlugin "1.0"
      GUI_RESOURCE_FILES ExtractDataTypeCategory.xml)
