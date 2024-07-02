#-----------------------------------------------------------------------------
# Add External Project
#-----------------------------------------------------------------------------

include(AddExternalProject)
define_external_dirs_ex(pybind11)
add_external_project_ex( pybind11
  URL https://github.com/pybind/pybind11/archive/refs/tags/v2.11.1.zip
  URL_MD5 c62d9e05243bd31cdb3bae1bb2f56655
  CMAKE_CACHE_ARGS
    -DPython_ROOT_DIR:PATH=${Python_ROOT_DIR}
    -DCMAKE_INSTALL_PREFIX:STRING=${pybind11_PREFIX}/install
    -DPYBIND11_TEST:BOOL=OFF 
  RELATIVE_INCLUDE_PATH "include"
  #DEPENDENCIES ""
  #VERBOSE
)
if (NOT USE_SYSTEM_pybind11)
  set(pybind11_DIR ${pybind11_PREFIX}/install/share/cmake/pybind11)
  message(STATUS "pybind11_DIR : ${pybind11_DIR}")
endif()