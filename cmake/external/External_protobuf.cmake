#-----------------------------------------------------------------------------
# Add External Project
#-----------------------------------------------------------------------------
include(AddExternalProject)

if(Pulse_MULTI_BUILD)
  message(STATUS "Building multiple protobuf configurations at once")
  set(PROTOBUF_DEBUG_BLD COMMAND ${CMAKE_COMMAND} --build . --config debug)
  set(PROTOBUF_RELEASE_BLD COMMAND ${CMAKE_COMMAND} --build . --config release)
  set(PROTOBUF_MULTI_BUILD BUILD_COMMAND ${PROTOBUF_DEBUG_BLD} ${PROTOBUF_RELEASE_BLD})
  set(PROTOBUF_DEBUG_INSTALL COMMAND ${CMAKE_COMMAND} --build . --target install --config debug)
  set(PROTOBUF_RELEASE_INSTALL COMMAND ${CMAKE_COMMAND} --build . --target install --config release)
  set(PROTOBUF_MULTI_INSTALL BUILD_COMMAND ${PROTOBUF_DEBUG_INSTALL} ${PROTOBUF_RELEASE_INSTALL})
endif()

# Support getting various versions, as end users could be using a different versions
# And their application runtime environment needs to have all consistent dlls, or they fight!
# Generally, We only support the latest version at the time of a release
# And any release that somebody has requested we support for compatibility with their application

set(Protobuf_VERSION "29.0-rc1" CACHE STRING "Select the  version of ProtoBuf to build.")
set_property(CACHE Protobuf_VERSION PROPERTY STRINGS "29.0-rc1" "21.12")

if (Protobuf_VERSION VERSION_EQUAL 29.0-rc1)# Latest, Can change
  set(Protobuf_url "https://github.com/protocolbuffers/protobuf/releases/download/v${Protobuf_VERSION}/protobuf-${Protobuf_VERSION}.zip" )
  set(Protobuf_md5 "8557b1251480efa9187129ff6f1b6f8a" )
elseif (Protobuf_VERSION VERSION_EQUAL 21.12)# Last version before the ABSL dependency
  set(Protobuf_url "https://github.com/protocolbuffers/protobuf/releases/download/v${Protobuf_VERSION}/protobuf-all-${Protobuf_VERSION}.zip" )
  set(Protobuf_md5 "4ef7148d6f8b42bcdba687ea1b60292f" )
else()
  message(STATUS "Using Protobuf Version ${Protobuf_VERSION}, with no git hash, will redownload if you rebuild.")
  set(Protobuf_md5 "" )
endif()

set(BUILD_PROTOC_BINARIES ON)
if(Pulse_C_AS_STATIC)
  set(BUILD_PROTOC_BINARIES OFF)
endif()

set(_pb_args)
set(_pb_dependencies)
message(STATUS "We are using protobuf ${Protobuf_VERSION}")
if (Protobuf_VERSION VERSION_GREATER_EQUAL "22.0")
  message(STATUS "Added ABSL dependency to ${absl_DIR}")
  set (_pb_dependencies absl)
  set(_pb_args -Dprotobuf_ABSL_PROVIDER:STRING=package -Dabsl_DIR:PATH=${absl_DIR})
endif()

define_external_dirs_ex(protobuf)
add_external_project_ex( protobuf
  URL ${Protobuf_url}
  URL_MD5 ${Protobuf_md5}
  CMAKE_CACHE_ARGS
    -DBUILD_SHARED_LIBS:BOOL=OFF
    -DBUILD_TESTING:BOOL=OFF
    -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
    -Dprotobuf_BUILD_TESTS:BOOL=OFF
    -Dprotobuf_BUILD_EXAMPLES:BOOL=OFF
    -Dprotobuf_BUILD_SHARED_LIBS:BOOL=OFF
    -Dprotobuf_MSVC_STATIC_RUNTIME:BOOL=${Pulse_MSVC_STATIC_RUNTIME}
    -Dprotobuf_WITH_ZLIB:BOOL=OFF
    -Dprotobuf_BUILD_PROTOC_BINARIES:BOOL=${BUILD_PROTOC_BINARIES}
    ${_pb_args}
  ${PROTOBUF_MULTI_BUILD}
  ${PROTOBUF_MULTI_INSTALL}
  RELATIVE_INCLUDE_PATH "include"
  DEPENDENCIES ${_pb_dependencies}
  #VERBOSE
)
if(Pulse_PYTHON_API)
  message(STATUS "Pip installing protobuf")
  ExternalProject_Add_Step(protobuf pypi
    COMMAND ${Python3_EXECUTABLE} -m pip install --force-reinstall -v "protobuf==5.${Protobuf_VERSION}"
    DEPENDEES install
    ALWAYS 1
    USES_TERMINAL true)
endif()

if (NOT USE_SYSTEM_protobuf)
  set(protobuf_INSTALL ${CMAKE_INSTALL_PREFIX})
  if(WIN32 AND Protobuf_VERSION VERSION_LESS "29.0")
    set(protobuf_DIR ${protobuf_INSTALL}/cmake)
  else()
    set(protobuf_DIR ${protobuf_INSTALL}/${CMAKE_INSTALL_LIBDIR}/cmake/protobuf)
  endif()
  set(protobuf_SRC ${protobuf_PREFIX}/src)
  set(utf8_range_DIR "${protobuf_INSTALL}/${CMAKE_INSTALL_LIBDIR}/cmake/utf8_range")
  
  message(STATUS "protobuf_DIR : ${protobuf_DIR}")
  message(STATUS "utf8_range_DIR : ${utf8_range_DIR}")
endif()
