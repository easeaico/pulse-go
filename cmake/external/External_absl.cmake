#-----------------------------------------------------------------------------
# Add External Project
#-----------------------------------------------------------------------------
if ((DEFINED Protobuf_VERSION) AND (Protobuf_VERSION VERSION_LESS "22.0"))
  return()
endif()

include(AddExternalProject)

message(STATUS "Processing External_absl")
if(${PROJECT_NAME}_MULTI_BUILD)
  message(STATUS "Building multiple abls configurations at once")
  set(ABSL_DEBUG_BLD COMMAND ${CMAKE_COMMAND} --build . --config debug)
  set(ABSL_RELEASE_BLD COMMAND ${CMAKE_COMMAND} --build . --config release)
  set(ABSL_MULTI_BUILD BUILD_COMMAND ${ABSL_DEBUG_BLD} ${ABSL_RELEASE_BLD})
  set(ABSL_DEBUG_INSTALL COMMAND ${CMAKE_COMMAND} --build . --target install --config debug)
  set(ABSL_RELEASE_INSTALL COMMAND ${CMAKE_COMMAND} --build . --target install --config release)
  set(ABSL_MULTI_INSTALL BUILD_COMMAND ${ABSL_DEBUG_INSTALL} ${ABSL_RELEASE_INSTALL})
endif()

define_external_dirs_ex(absl)
add_external_project_ex(absl
  URL https://github.com/abseil/abseil-cpp/releases/download/20240722.0/abseil-cpp-20240722.0.tar.gz
  URL_MD5 "740fb8f35ebdf82740c294bde408b9c0"
  
  #URL https://github.com/abseil/abseil-cpp/archive/refs/tags/20240116.2.zip
  #URL_MD5 "de7b132899d3ef28edbd7212fbd6a334"
  
  CMAKE_CACHE_ARGS
    -DBUILD_SHARED_LIBS:BOOL=OFF
    -DBUILD_TESTING:BOOL=OFF
    -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
    -DABSL_PROPAGATE_CXX_STD:BOOL=ON
    -DABSL_MSVC_STATIC_RUNTIME:BOOL=${Pulse_MSVC_STATIC_RUNTIME}
  RELATIVE_INCLUDE_PATH "include"
  #DEPENDENCIES ""
  #VERBOSE
  ${ABSL_MULTI_BUILD}
  ${ABSL_MULTI_INSTALL}
)

if (NOT USE_SYSTEM_absl)
  set(absl_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/cmake/absl")
  message(STATUS "absl_DIR : ${absl_DIR}")
endif()
