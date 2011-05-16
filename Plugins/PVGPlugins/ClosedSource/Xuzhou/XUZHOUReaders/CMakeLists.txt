PROJECT(XUZHOUReader)
FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})

ADD_PARAVIEW_PLUGIN(XUZHOUReaderClientPlugin "1.0"
	GUI_RESOURCES XUZHOUReader.qrc)

ADD_PARAVIEW_PLUGIN(XUZHOUReaderServerManagerPlugin "1.0"
	SERVER_MANAGER_XML XUZHOUReader.xml
	SERVER_MANAGER_SOURCES vtkVentilationReader.cpp vtkTransportationReader.cpp vtkElectricalReader.cpp vtkDrainageReader.cpp)