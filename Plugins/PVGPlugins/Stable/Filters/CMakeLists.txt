PROJECT(StableFilters)
INCLUDE(${PARAVIEW_USE_FILE})

FIND_PACKAGE(Qt4)
IF(QT4_FOUND)

INCLUDE(${QT_USE_FILE})

INCLUDE_DIRECTORIES(
${VTK_INCLUDE_DIR}
${PARAVIEW_INCLUDE_DIRS}
${ParaView_SOURCE_DIR}/VTK/GUISupport/Qt
${pqComponents_SOURCE_DIR}
${pqComponents_BINARY_DIR}
${pqCore_SOURCE_DIR}
${pqCore_BINARY_DIR}
${QtWidgets_SOURCE_DIR}
${QtWidgets_BINARY_DIR}
${QtChart_SOURCE_DIR}
${QtChart_BINARY_DIR}
${QtTesting_SOURCE_DIR}
${QtTesting_BINARY_DIR}
${QtPython_SOURCE_DIR}
${QtPython_BINARY_DIR}
ApplySchedule
#Annotation
BoundingBox
CleanCells
ComputeVolumes
ConvertPointsToBlocks
ExtractDataType
InsideClosedSurface
IntersectWireframe
MapAttributes
MDelaunay2D
#PlotPropertiesOverTime
PolyLineAddPoints
PolyToStruct
ProximityThreshold
RadialDistance
RegexThreshold
RelativeTransform
SequenceArrows
SharedCode
SumBillboard
SurfaceReconstructor
TensorGlyph
tetgen
TimeBillboard
TimeThreshold
TopOfCell
VTKBultIn
Ruler
)

SET( MIRARCO_SM_SRCS
ApplySchedule/vtkApplySchedule.cxx
#Annotation/vtkAnnotation.cxx
BoundingBox/vtkBBox.cxx
CleanCells/vtkCleanCells.cxx
ComputeVolumes/vtkComputeVolumes.cxx
ConvertPointsToBlocks/vtkConvertPointsToBlocks.cxx
ExtractDataType/vtkExtractPolyData.cxx
ExtractDataType/vtkExtractUnstrucGrid.cxx
InsideClosedSurface/vtkInsideClosedSurface.cxx
IntersectWireframe/vtkIntersectFilter.cxx 
IntersectWireframe/vtkIntersectWireframe.cxx
IntersectWireframe/vtkThresholdProcessId.cxx
MapAttributes/vtkMapAttributesToGrid.cxx
MapAttributes/vtkMapAttributesFromGrid.cxx
MapAttributes/vtkMapAttributes.cxx 
MDelaunay2D/vtkMDelaunay2D.cxx
#PlotPropertiesOverTime/vtkPlotPropertiesOverTime.cxx 
PolyLineAddPoints/vtkPolyLineAddPoints.cxx
PolyToStruct/vtkPolyStruct.cxx
ProximityThreshold/vtkProximityThreshold.cxx
RadialDistance/vtkRadialDistance.cxx
RegexThreshold/vtkRegexThreshold.cxx
RelativeTransform/vtkRelativeTransform.cxx
SequenceArrows/vtkSequenceArrows.cxx
SumBillboard/vtkSumBillboard.cxx
SurfaceReconstructor/vtkSurfaceReconstructor.cxx
TensorGlyph/vtkTensorGlyphFilter.cxx
tetgen/vtkTetgen.cxx
TimeBillboard/vtkTimeBillboard.cxx
TimeThreshold/vtkTimeThreshold.cxx
TopOfCell/vtkTopOfCell.cxx
Ruler/vtkRuler.cxx
)

SET( MIRARCO_EXTRA_SRCS
tetgen/tetgen.cxx
tetgen/predicates.cxx
SharedCode/TimeConverter.cxx
)

SET( MIRARCO_GUI_SRCS
ApplySchedule/pqApplySchedule.cxx
BoundingBox/pqBoundingBox.cxx 
ComputeVolumes/pqComputeVolumes.cxx
ConvertPointsToBlocks/pqConvertPointsToBlocks.cxx
RegexThreshold/pqRegexThreshold.cxx
SumBillboard/pqSumBillboardPanel.cxx
tetgen/pqTetgen.cxx
TimeThreshold/pqTimeThreshold.cxx
#PlotPropertiesOverTime/pqPlotPropertiesOverTime.cxx
#PlotPropertiesOverTime/pqPlotPropertiesOverTimePanel.cxx
#PlotPropertiesOverTime/pqLabelFormatter.cxx
RadialDistance/pqRadialDistanceWidget.cxx
SharedCode/TimeConverter.cxx
Ruler/pqRuler.cxx
)

SET(MOC_LIST
ApplySchedule/pqApplySchedule.h
BoundingBox/pqBoundingBox.h
ComputeVolumes/pqComputeVolumes.h
ConvertPointsToBlocks/pqConvertPointsToBlocks.h
RegexThreshold/pqRegexThreshold.h
SumBillboard/pqSumBillboardPanel.h
tetgen/pqTetgen.h
TimeThreshold/pqTimeThreshold.h
#PlotPropertiesOverTime/pqPlotPropertiesOverTime.h
#PlotPropertiesOverTime/pqPlotPropertiesOverTimePanel.h
#PlotPropertiesOverTime/pqLabelFormatter.h
RadialDistance/pqRadialDistanceWidget.h
Ruler/pqRuler.h

)

QT4_WRAP_CPP(MOC_SRCS ${MOC_LIST})

ADD_PARAVIEW_OBJECT_PANEL(IFACES IFACE_SRCS CLASS_NAME pqApplySchedule
                         XML_NAME "Apply Schedule" XML_GROUP filters)

ADD_PARAVIEW_OBJECT_PANEL(IFACES2 IFACE_SRCS2 CLASS_NAME pqBoundingBox
                         XML_NAME "Bounding Box" XML_GROUP filters)
                         
ADD_PARAVIEW_OBJECT_PANEL(IFACES3 IFACE_SRCS3 CLASS_NAME pqComputeVolumes
                         XML_NAME "Compute Volumes" XML_GROUP filters)
                         
ADD_PARAVIEW_OBJECT_PANEL(IFACES4 IFACE_SRCS4 CLASS_NAME pqRegexThreshold
                          XML_NAME RegexThreshold XML_GROUP filters)

ADD_PARAVIEW_OBJECT_PANEL(IFACES5 IFACE_SRCS5 CLASS_NAME pqSumBillboardPanel
                         XML_NAME SumBillboard XML_GROUP filters)  
                                                   
ADD_PARAVIEW_OBJECT_PANEL(IFACES6 IFACE_SRCS6 CLASS_NAME pqTetgen
                         XML_NAME "Volumique Tetrahedralization" XML_GROUP filters)

ADD_PARAVIEW_OBJECT_PANEL(IFACES7 IFACE_SRCS7 CLASS_NAME pqTimeThreshold
                         XML_NAME TimeThreshold XML_GROUP filters)
                         
#ADD_PARAVIEW_OBJECT_PANEL(IFACES8 IFACE_SRCS8 CLASS_NAME pqPlotPropertiesOverTimePanel
#                         XML_NAME "Plot Properties Over Time" XML_GROUP filters)
 
#ADD_PARAVIEW_VIEW_MODULE(IFACES9 IFACE_SRCS9 VIEW_TYPE pqPlotPropertiesOverTime
#                         VIEW_XML_GROUP views)
                         
ADD_PARAVIEW_OBJECT_PANEL(IFACES10 IFACE_SRCS10 CLASS_NAME pqRadialDistanceWidget
                         XML_NAME "Radial Distance" XML_GROUP filters)

ADD_PARAVIEW_OBJECT_PANEL(IFACES11 IFACE_SRCS11 CLASS_NAME pqConvertPointsToBlocks
                         XML_NAME ConvertPointsToBlocks XML_GROUP filters)

ADD_PARAVIEW_OBJECT_PANEL(IFACES12 IFACE_SRCS12 CLASS_NAME 	pqRuler
  XML_NAME "Ruler" XML_GROUP filters)                         

LIST(APPEND IFACES ${IFACES2})
LIST(APPEND IFACES ${IFACES3})
LIST(APPEND IFACES ${IFACES4})
LIST(APPEND IFACES ${IFACES5})
LIST(APPEND IFACES ${IFACES6})
LIST(APPEND IFACES ${IFACES7})
LIST(APPEND IFACES ${IFACES8})
LIST(APPEND IFACES ${IFACES9})
LIST(APPEND IFACES ${IFACES10})
LIST(APPEND IFACES ${IFACES11})
LIST(APPEND IFACES ${IFACES12})
LIST(APPEND IFACE_SRCS ${IFACE_SRCS2})
LIST(APPEND IFACE_SRCS ${IFACE_SRCS3})
LIST(APPEND IFACE_SRCS ${IFACE_SRCS4})
LIST(APPEND IFACE_SRCS ${IFACE_SRCS5})
LIST(APPEND IFACE_SRCS ${IFACE_SRCS6})
LIST(APPEND IFACE_SRCS ${IFACE_SRCS7})
LIST(APPEND IFACE_SRCS ${IFACE_SRCS8})
LIST(APPEND IFACE_SRCS ${IFACE_SRCS9})
LIST(APPEND IFACE_SRCS ${IFACE_SRCS10})
LIST(APPEND IFACE_SRCS ${IFACE_SRCS11})
LIST(APPEND IFACE_SRCS ${IFACE_SRCS12})

ADD_PARAVIEW_PLUGIN(StableFiltersSMPlugin "1.0"
			SERVER_MANAGER_XML SFSM.xml
			SERVER_MANAGER_SOURCES  ${MIRARCO_SM_SRCS}
			SERVER_SOURCES  ${MIRARCO_EXTRA_SRCS}
			)

ADD_PARAVIEW_PLUGIN(StableFilterGUIPlugin "1.0"
             GUI_INTERFACES ${IFACES}
             GUI_RESOURCES GUI.qrc
             GUI_SOURCES ${MIRARCO_GUI_SRCS} ${MOC_SRCS} ${IFACE_SRCS}
			)

ADD_PARAVIEW_PLUGIN(StableFilterCPlugin  "1.0" 
			GUI_RESOURCE_FILES SFCategory.xml)			

ENDIF(QT4_FOUND)


#CPACK CODE TO BUNDLE THE DLLS CORRECTLY
INSTALL(TARGETS StableFilterGUIPlugin StableFiltersSMPlugin StableFilterCPlugin  
	RUNTIME DESTINATION bin/plugins COMPONENT Runtime
	LIBRARY DESTINATION bin/plugins 
	ARCHIVE DESTINATION bin/plugins 
)
