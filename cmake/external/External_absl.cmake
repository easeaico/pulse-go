#-----------------------------------------------------------------------------
# Add External Project
#-----------------------------------------------------------------------------
if(${PROJECT_NAME}_MULTI_BUILD)
  message(STATUS "Building multiple abls configurations at once")
  set(ABSL_DEBUG_BLD COMMAND ${CMAKE_COMMAND} --build . --config debug)
  set(ABSL_RELEASE_BLD COMMAND ${CMAKE_COMMAND} --build . --config release)
  set(ABSL_MULTI_BUILD BUILD_COMMAND ${ABSL_DEBUG_BLD} ${ABSL_RELEASE_BLD})
  set(ABSL_DEBUG_INSTALL COMMAND ${CMAKE_COMMAND} --build . --target install --config debug)
  set(ABSL_RELEASE_INSTALL COMMAND ${CMAKE_COMMAND} --build . --target install --config release)
  set(ABSL_MULTI_INSTALL BUILD_COMMAND ${ABSL_DEBUG_INSTALL} ${ABSL_RELEASE_INSTALL})
endif()

set(_SHARED_ABSL OFF)
if(MSVC)
  set(_SHARED_ABSL ON)
endif()

define_external_dirs_ex(absl)
add_external_project_ex(absl
  # URL https://github.com/abseil/abseil-cpp/archive/refs/tags/20240116.0.zip
  # URL_MD5 "1aac81d51689e902d9d88e6fb0992623"
  
  URL https://github.com/abseil/abseil-cpp/archive/refs/tags/20240116.2.zip
  URL_MD5 "de7b132899d3ef28edbd7212fbd6a334"
  
  #GIT_REPOSITORY https://github.com/aaron-bray/abseil-cpp.git
  #GIT_TAG msvc_runtime_library
  #GIT_SHALLOW TRUE
  
  CMAKE_CACHE_ARGS
    -DBUILD_SHARED_LIBS:BOOL=${_SHARED_ABSL}
    -DBUILD_TESTING:BOOL=OFF
    -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
    -DABSL_PROPAGATE_CXX_STD:BOOL=ON
    #-DABSL_ABSL_MSVC_STATIC_RUNTIME:BOOL=ON
  RELATIVE_INCLUDE_PATH "include"
  #DEPENDENCIES ""
  #VERBOSE
  ${ABSL_MULTI_BUILD}
  ${ABSL_MULTI_INSTALL}
)

if (NOT USE_SYSTEM_abls)
  set(absl_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/cmake/absl")
  message(STATUS "absl_DIR : ${absl_DIR}")
endif()
