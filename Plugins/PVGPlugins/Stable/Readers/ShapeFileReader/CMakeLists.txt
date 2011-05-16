PROJECT(ShapeFileReader)
FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})

ADD_PARAVIEW_PLUGIN(ShapeFileReaderClientPlugin "1.0"
	GUI_RESOURCES ShapeFileReader.qrc)

ADD_PARAVIEW_PLUGIN(ShapeFileReaderServerManagerPlugin "1.0"
	SERVER_MANAGER_XML ShapeFileReader.xml
	SERVER_MANAGER_SOURCES vtkShapeFileReader.cxx
	SOURCES dimension.cxx
			hgrid.cxx
			hpolygon.cxx
			mgconst.cxx
			mgexcept.cxx
			dbfadd.c
			dbfcreate.c
			dbfdump.c
			dbfopen.c
			safileio.c
			shpadd.c
			shpcreate.c
			shpdump.c
			shpopen.c
			shprewind.c
			shptest.c
			shptree.c
			shptreedump.c
			shputils.c)

