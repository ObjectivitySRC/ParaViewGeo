PROJECT(GeoReferencedImageReader)
FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})

INCLUDE_DIRECTORIES(
geotiff/include
geotiff/lib
)

QT4_WRAP_CPP(MOC_SRCS pqGeoReferencedImageReader.h) 

FILE(GLOB GEOTIFF_EXTRAS1 "${CMAKE_CURRENT_SOURCE_DIR}/geotiff/include/*.c")
FILE(GLOB GEOTIFF_EXTRAS2 "${CMAKE_CURRENT_SOURCE_DIR}/geotiff/include/*.h")
FILE(GLOB GEOTIFF_EXTRAS3 "${CMAKE_CURRENT_SOURCE_DIR}/geotiff/include/*.inc")
FILE(GLOB GEOTIFF_EXTRAS4 "${CMAKE_CURRENT_SOURCE_DIR}/geotiff/lib/*.lib")

LIST(APPEND MIRARCO_EXTRA_SRCS ${GEOTIFF_EXTRAS1} ${GEOTIFF_EXTRAS2} ${GEOTIFF_EXTRAS4})



ADD_PARAVIEW_OBJECT_PANEL(IFACES IFACE_SRCS CLASS_NAME pqGeoReferencedImageReader
                         XML_NAME GeoReferencedImageReader XML_GROUP sources)
	
# create a plugin for this panel
ADD_PARAVIEW_PLUGIN(GeoReferencedImageReaderGUIPlugin "1.0" 
                    GUI_INTERFACES ${IFACES} 
                    GUI_RESOURCES pqUI.qrc
                    SOURCES pqGeoReferencedImageReader.cxx ${MOC_SRCS} ${IFACE_SRCS})
                    
ADD_PARAVIEW_PLUGIN(GeoReferencedImageReaderReaderSMPlugin "1.0"
	SERVER_MANAGER_XML GeoReferencedImageReader.xml
	SERVER_MANAGER_SOURCES vtkGeoReferencedImageReader.cxx
	SOURCES ${MIRARCO_EXTRA_SRCS} )
	                    
                    
ADD_PARAVIEW_PLUGIN(GeoReferencedImageReaderCPlugin "1.0"
	GUI_RESOURCES pqGeoReferencedImageReader.qrc)
                    