PROJECT(GocadWriter)
FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})

ADD_PARAVIEW_PLUGIN(GocadWriterClientPlugin "1.0"
	GUI_RESOURCES GocadWriter.qrc)

ADD_PARAVIEW_PLUGIN(GocadVoxetWriterClientPlugin "1.0"
	GUI_RESOURCES GocadVoxetWriter.qrc)

ADD_PARAVIEW_PLUGIN(GocadCompositeWriterClientPlugin "1.0"
	GUI_RESOURCES GocadCompositeWriter.qrc)

ADD_PARAVIEW_PLUGIN(GocadWriterServerManagerPlugin "1.0"
	SERVER_MANAGER_XML GocadWriter.xml
	SERVER_MANAGER_SOURCES vtkGocadWriter.cxx vtkGocadBase.cxx)

ADD_PARAVIEW_PLUGIN(GocadVoxetWriterServerManagerPlugin "1.0"
	SERVER_MANAGER_XML GocadVoxetWriter.xml
	SERVER_MANAGER_SOURCES vtkGocadVoxetWriter.cxx vtkGocadBase.cxx)

ADD_PARAVIEW_PLUGIN(GocadCompositeWriterSMP "1.0"
	SERVER_MANAGER_XML GocadCompositeWriter.xml
	SERVER_MANAGER_SOURCES vtkGocadCompositeWriter.cxx vtkGocadBase.cxx vtkGocadVoxetWriter.cxx vtkGocadWriter.cxx  )	