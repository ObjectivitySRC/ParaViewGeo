
MACRO(paraview_build_optional_plugin name comment subdirectory default)
  OPTION(PARAVIEW_BUILD_PLUGIN_${name} "Build ${comment}" ${default})
  IF(PARAVIEW_BUILD_PLUGIN_${name})
    MESSAGE(STATUS "Plugin: ${comment} enabled")
    ADD_SUBDIRECTORY("${subdirectory}")
  ELSE(PARAVIEW_BUILD_PLUGIN_${name})
    MESSAGE(STATUS "Plugin: ${comment} disabled")
  ENDIF(PARAVIEW_BUILD_PLUGIN_${name})
ENDMACRO(paraview_build_optional_plugin)

paraview_build_optional_plugin(
    Development-Filters "Development-Filters" Filters OFF)

paraview_build_optional_plugin(
    Development-Readers "Development-Readers" Readers OFF)

paraview_build_optional_plugin(
    Development-Writers "Development-Writers" Writers OFF)


paraview_build_optional_plugin(
    PVDevelopment-Filters "PVDevelopment-Filters" PVFilters OFF)

paraview_build_optional_plugin(
    PVDevelopment-Readers "PVDevelopment-Readers" PVReaders OFF)

