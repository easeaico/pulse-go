#-----------------------------------------------------------------------------
# Add External Project
#-----------------------------------------------------------------------------
message(STATUS "Processing External_Eigen3")
include(AddExternalProject)
define_external_dirs_ex( Eigen3 )
add_external_project_ex( Eigen3
  URL "https://gitlab.com/libeigen/eigen/-/archive/5.0.0/eigen-5.0.0.zip"
  URL_HASH MD5=3804ac4802bb18a75874d7a165c5c6f3
  CMAKE_CACHE_ARGS 
    -DBUILD_TESTING:BOOL=OFF
    -DEIGEN_BUILD_DEMOS:BOOL=OFF
    -DEIGEN_BUILD_DOC:BOOL=OFF
    -DCMAKE_INSTALL_PREFIX:PATH=${Eigen3_PREFIX}/install
  RELATIVE_INCLUDE_PATH ""
  DEPENDENCIES ""
  #VERBOSE
  )
if (NOT USE_SYSTEM_Eigen3)
  set(Eigen3_DIR ${Eigen3_PREFIX}/install/share/eigen3/cmake)
  message(STATUS "Eigen3_DIR : ${Eigen3_DIR}")
endif()
