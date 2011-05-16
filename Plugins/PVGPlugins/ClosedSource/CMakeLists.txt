MACRO(paraview_build_optional_plugin name comment subdirectory default)
  OPTION(PARAVIEW_BUILD_PLUGIN_${name} "Build ${comment}" ${default})
  IF(PARAVIEW_BUILD_PLUGIN_${name})
    MESSAGE(STATUS "Plugin: ${comment} enabled")
    ADD_SUBDIRECTORY("${subdirectory}")
  ELSE(PARAVIEW_BUILD_PLUGIN_${name})
    MESSAGE(STATUS "Plugin: ${comment} disabled")
  ENDIF(PARAVIEW_BUILD_PLUGIN_${name})
ENDMACRO(paraview_build_optional_plugin)

#paraview_build_optional_plugin(
#    Creighton "Plugins-Creighton" Creighton OFF)

paraview_build_optional_plugin(
    Xuzhou "Plugins-Xuzhou" Xuzhou OFF)
	
paraview_build_optional_plugin(
    ABRI "Plugins-ABRI" ABRI OFF)
    
 paraview_build_optional_plugin(
    ARG "Plugins-ARG" ARG OFF)


