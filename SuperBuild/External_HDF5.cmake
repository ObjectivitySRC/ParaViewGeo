
# The HDF5 external project for ParaView
set(HDF5_source "${CMAKE_CURRENT_BINARY_DIR}/HDF5")
set(HDF5_install "${CMAKE_CURRENT_BINARY_DIR}")

set(HDF5_binary "${CMAKE_CURRENT_BINARY_DIR}/HDF5-build")

ExternalProject_Add(HDF5
  URL ${HDF5_URL}/${HDF5_GZ}
  URL_MD5 ${HDF5_MD5}
  UPDATE_COMMAND ""
  SOURCE_DIR ${HDF5_source}
  BINARY_DIR ${HDF5_binary}
  INSTALL_DIR ${HDF5_install}
  CMAKE_CACHE_ARGS
    -DBUILD_SHARED_LIBS:BOOL=ON
    -DBUILD_TESTING:BOOL=OFF
    -DCMAKE_CXX_FLAGS:STRING=${pv_tpl_cxx_flags}
    -DCMAKE_C_FLAGS:STRING=${pv_tpl_c_flags}
    -DCMAKE_BUILD_TYPE:STRING=${CMAKE_CFG_INTDIR}
    ${pv_tpl_compiler_args}
    -DHDF5_ENABLE_Z_LIB_SUPPORT:BOOL=ON
    -DZLIB_INCLUDE_DIR:STRING=${ZLIB_INCLUDE_DIR}
    -DZLIB_LIBRARY:STRING=${ZLIB_LIBRARY}
  CMAKE_ARGS
    -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
  DEPENDS ${HDF5_dependencies}
  )


if(WIN32)
  set(HDF5_INCLUDE_DIR ${HDF5_install}/include)
  set(HDF5_LIBRARY ${HDF5_install}/lib/hdf5dll${_LINK_LIBRARY_SUFFIX})
else()
  set(HDF5_INCLUDE_DIR ${HDF5_install}/include)
  set(HDF5_LIBRARY ${HDF5_install}/lib/libhdf5${_LINK_LIBRARY_SUFFIX})
endif()