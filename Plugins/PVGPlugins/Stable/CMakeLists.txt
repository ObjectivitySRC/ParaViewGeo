
MACRO(paraview_build_optional_plugin name comment subdirectory default)
  OPTION(PARAVIEW_BUILD_PLUGIN_${name} "Build ${comment}" ${default})
  MARK_AS_ADVANCED(CLEAR PARAVIEW_BUILD_PLUGIN_${name})
  IF(PARAVIEW_BUILD_PLUGIN_${name})
    MESSAGE(STATUS "Plugin: ${comment} enabled")
    ADD_SUBDIRECTORY("${subdirectory}")
  ELSE(PARAVIEW_BUILD_PLUGIN_${name})
    MESSAGE(STATUS "Plugin: ${comment} disabled")
  ENDIF(PARAVIEW_BUILD_PLUGIN_${name})
ENDMACRO(paraview_build_optional_plugin)

paraview_build_optional_plugin(
    Stable-Filters "Stable-Filters" Filters ON)

paraview_build_optional_plugin(
    Stable-Readers "Stable-Readers" Readers ON)

paraview_build_optional_plugin(
    Stable-Writers "Stable-Writers" Writers ON)



