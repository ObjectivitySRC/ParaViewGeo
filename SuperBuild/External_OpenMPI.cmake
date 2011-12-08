
# The OpenMPI external project for ParaView
set(OpenMPI_source "${CMAKE_CURRENT_BINARY_DIR}/OpenMPI")
set(OpenMPI_build "${CMAKE_CURRENT_BINARY_DIR}/OpenMPI-build")
set(OpenMPI_install "${CMAKE_CURRENT_BINARY_DIR}/OpenMPI-install")

# If Windows we use CMake otherwise ./configure
if(WIN32)
  ExternalProject_Add(OpenMPI
    DOWNLOAD_DIR ${CMAKE_CURRENT_BINARY_DIR}
    SOURCE_DIR ${OpenMPI_source}
    BINARY_DIR ${OpenMPI_build}
    INSTALL_DIR ${OpenMPI_install}
    URL ${OPENMPI_URL}/${OPENMPI_GZ}
    URL_MD5 ${OPENMPI_MD5}
    CMAKE_CACHE_ARGS
#      -DCMAKE_CXX_FLAGS:STRING=${pv_tpl_cxx_flags}
#      -DCMAKE_C_FLAGS:STRING=${pv_tpl_c_flags}
      -DCMAKE_BUILD_TYPE:STRING=${CMAKE_CFG_INTDIR}
#      ${pv_tpl_compiler_args}
      ${OpenMPI_EXTRA_ARGS}
    CMAKE_ARGS
      -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
  )
else()
  # on linux build the binaries static
  ExternalProject_Add(OpenMPI
    DOWNLOAD_DIR ${CMAKE_CURRENT_BINARY_DIR}
    SOURCE_DIR ${OpenMPI_source}
    INSTALL_DIR ${OpenMPI_install}
    URL ${OPENMPI_URL}/${OPENMPI_GZ}
    URL_MD5 ${OPENMPI_MD5}
    BUILD_IN_SOURCE 1
    PATCH_COMMAND ""
    CONFIGURE_COMMAND CFLAGS=-fPIC CXXFLAGS=-fPIC <SOURCE_DIR>/configure --prefix=<INSTALL_DIR> --enable-shared
  )
endif()

set(MPIEXEC ${OpenMPI_install}/bin/mpiexec${CMAKE_EXECUTABLE_SUFFIX})
set(MPICC ${OpenMPI_install}/bin/mpicc${CMAKE_EXECUTABLE_SUFFIX})
set(MPICXX ${OpenMPI_install}/bin/mpic++${CMAKE_EXECUTABLE_SUFFIX})
set(MPI_INCLUDE_PATH ${OpenMPI_install}/include)
set(MPI_INSTALL ${OpenMPI_install})
if(WIN32)
  #set(MPI_LIBRARY optimized "${OpenMPI_install}/lib/libmpi${_LINK_LIBRARY_SUFFIX} debug ${OpenMPI_install}/lib/libmpid${_LINK_LIBRARY_SUFFIX}")
  set(MPI_LIBRARY ${OpenMPI_install}/lib/libmpi${_LINK_LIBRARY_SUFFIX})
  #set(MPI_EXTRA_LIBRARY optimized "${OpenMPI_install}/lib/libmpi_cxx${_LINK_LIBRARY_SUFFIX} debug ${OpenMPI_install}/lib/libmpi_cxxd${_LINK_LIBRARY_SUFFIX}")
  set(MPI_EXTRA_LIBRARY "${OpenMPI_install}/lib/libmpi_cxx${_LINK_LIBRARY_SUFFIX}")
else()
  set(MPI_LIBRARY ${OpenMPI_install}/lib/libmpi.a)
  set(MPI_EXTRA_LIBRARY ${OpenMPI_install}/lib/libmpi_cxx.a)
endif()
mark_as_advanced(OpenMPI_DIR)
