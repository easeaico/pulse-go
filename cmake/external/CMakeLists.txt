if(POLICY CMP0135)# New External Project Time Stamping
  cmake_policy(SET CMP0135 NEW)
endif()
include(ExternalProject)
include(SolveDependencies)

#-----------------------------------------------------------------------------
# Git protocol option
#-----------------------------------------------------------------------------
if(NOT GIT_EXECUTABLE)
  find_package(Git REQUIRED)
endif()

#-----------------------------------------------------------------------------
# CMake global args
#-----------------------------------------------------------------------------
if(MSVC)
  # Allow the MP flag to get set externally
  set(${PROJECT_NAME}_ENABLE_MULTI_PROCESS_BUILDS ON CACHE BOOL "Enable multi-process builds")
  set(PROCESSOR_COUNT "$ENV{NUMBER_OF_PROCESSORS}")
  set(${PROJECT_NAME}_NUM_BUILD_PROCESSES ${PROCESSOR_COUNT} CACHE STRING "The maximum number of processes for the /MP flag")
  if(${PROJECT_NAME}_ENABLE_MULTI_PROCESS_BUILDS)
    set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} /MP${${PROJECT_NAME}_NUM_BUILD_PROCESSES}" CACHE STRING INTERNAL FORCE)
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /MP${${PROJECT_NAME}_NUM_BUILD_PROCESSES}" CACHE STRING INTERNAL FORCE)
  endif()
#elseif(ANDROID)
#  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -llog" CACHE INTERNAL "" FORCE)
#  set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -llog" CACHE INTERNAL "" FORCE)
#  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -llog" CACHE INTERNAL "" FORCE)
#  set(CMAKE_STATIC_LINKER_FLAGS "${CMAKE_STATIC_LINKER_FLAGS} -llog" CACHE INTERNAL "" FORCE)
#  message(STATUS "Hello ANDROID")
endif()

if(CMAKE_TOOLCHAIN_FILE)
  message(STATUS "Passing toolchain file : ${CMAKE_TOOLCHAIN_FILE}")
  set(COMPILER CMAKE_TOOLCHAIN_FILE)
else()
  set(COMPILER CMAKE_C_COMPILER
               CMAKE_CXX_COMPILER)
endif()

mark_as_superbuild(
  VARS
    CMAKE_BUILD_TYPE
    CMAKE_DEBUG_POSTFIX
    CMAKE_INSTALL_PREFIX
    #CMAKE_MODULE_PATH # Paths used in include(), find_package()
    #CMAKE_PREFIX_PATH # Path prefixes to be searched, in find_xxx functions
    ${COMPILER}
    CMAKE_C_FLAGS
    CMAKE_CXX_FLAGS
    CMAKE_EXE_LINKER_FLAGS
    CMAKE_MODULE_LINKER_FLAGS
    CMAKE_SHARED_LINKER_FLAGS
    CMAKE_STATIC_LINKER_FLAGS
    CMAKE_CXX_STANDARD
    CMAKE_CXX_STANDARD_REQUIRED
    THREADS_PREFER_PTHREAD_FLAG
    CMAKE_POSITION_INDEPENDENT_CODE
  ALL_PROJECTS
  )

#-----------------------------------------------------------------------------
# CMake args if Apple
#-----------------------------------------------------------------------------
if(APPLE)
  set(CMAKE_MACOSX_RPATH ON)
  mark_as_superbuild(
    VARS
      CMAKE_OSX_ARCHITECTURES
      CMAKE_OSX_SYSROOT
      CMAKE_OSX_DEPLOYMENT_TARGET
      CMAKE_MACOSX_RPATH
    ALL_PROJECTS
    )
endif()

#-----------------------------------------------------------------------------
# Keep track of include path for superbuild
#-----------------------------------------------------------------------------
set(CMAKE_INCLUDE_PATH )
mark_as_superbuild(VARS CMAKE_INCLUDE_PATH ALL_PROJECTS)

#-----------------------------------------------------------------------------
# Solve dependencies
#-----------------------------------------------------------------------------
set(EXTERNAL_PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR}) # Location of the "External_*" files

ExternalProject_Include_Dependencies( ${PROJECT_NAME}
  DEPENDS_VAR ${PROJECT_NAME}_DEPENDENCIES
  EP_ARGS_VAR ${PROJECT_NAME}_EP_ARGS
  SUPERBUILD_VAR ${PROJECT_NAME}_SUPERBUILD
  )


#-----------------------------------------------------------------------------
# Inner build of the main project
#-----------------------------------------------------------------------------
ExternalProject_Add( ${PROJECT_NAME}
  ${${PROJECT_NAME}_EP_ARGS}
  DOWNLOAD_COMMAND ""
  SOURCE_DIR ${${PROJECT_NAME}_SOURCE_DIR}
  BINARY_DIR ${CMAKE_BINARY_DIR}/Innerbuild
  ${COMMON_CMAKE_EP_ARGS}
  CMAKE_ARGS
    -DCMAKE_TOOLCHAIN_FILE:STRING=${CMAKE_TOOLCHAIN_FILE}
  CMAKE_CACHE_ARGS 
    -D${PROJECT_NAME}_SUPERBUILD:BOOL=OFF
    -D${PROJECT_NAME}_JAVA_API:BOOL=${${PROJECT_NAME}_JAVA_API}
    -D${PROJECT_NAME}_JAVA_HOME_DIR:PATH=${${PROJECT_NAME}_JAVA_HOME_DIR}
    -D${PROJECT_NAME}_CSHARP_API:BOOL=${${PROJECT_NAME}_CSHARP_API}
    -D${PROJECT_NAME}_PYTHON_API:BOOL=${${PROJECT_NAME}_PYTHON_API}
    -D${PROJECT_NAME}_PYTHON_HOME_DIR:PATH=${${PROJECT_NAME}_PYTHON_HOME_DIR}
    -D${PROJECT_NAME}_AS_SHARED:BOOL=${${PROJECT_NAME}_AS_SHARED}
    -D${PROJECT_NAME}_C_AS_STATIC:BOOL=${${PROJECT_NAME}_C_AS_STATIC}
    -D${PROJECT_NAME}_NATIVE_BUILD_DIR:PATH=${${PROJECT_NAME}_NATIVE_BUILD_DIR}
    -DEigen3_DIR:PATH=${Eigen3_DIR}
    -Dprotobuf_DIR:PATH=${protobuf_DIR}
    -Dprotobuf_SRC:PATH=${protobuf_SRC}
    -Dprotobuf_INSTALL:PATH=${protobuf_INSTALL}
    -Dpybind11_DIR:PATH=${pybind11_DIR}
    -Dabsl_DIR:PATH=${absl_DIR}
    -Dutf8_range_DIR:PATH=${utf8_range_DIR}
  #INSTALL_COMMAND ${SKIP_STEP_COMMAND}
  DEPENDS ${${PROJECT_NAME}_DEPENDENCIES}
)
set_target_properties (${PROJECT_NAME} PROPERTIES FOLDER ${PROJECT_NAME})

# Need Java Utils to generate data
if (${PROJECT_NAME}_JAVA_API)
  add_custom_target(PulseData ALL)
  add_dependencies(PulseData Pulse)
  add_custom_command(TARGET PulseData POST_BUILD
      COMMAND ${CMAKE_COMMAND} -DTYPE:STRING=genData -P run.cmake
              WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin
      BYPRODUCTS "${CMAKE_INSTALL_PREFIX}/bin/patients/StandardMale.json")
  # Don't genStates if in Debug, takes way too long
  string(APPEND _genStates
    "$<IF:$<CONFIG:Debug>,"
        "${CMAKE_COMMAND};-E;echo;!!! NOT GENERATING STATES FOR DEBUG BUILDS !!!,"
        "${CMAKE_COMMAND};-DTYPE:STRING=genStates;-P;run.cmake"
    ">")
  add_custom_command(TARGET PulseData POST_BUILD
      COMMAND "${_genStates}"
              COMMAND_EXPAND_LISTS
              WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin
      BYPRODUCTS "${CMAKE_INSTALL_PREFIX}/bin/states/StandardMale@0s.json")
  set_target_properties (PulseData PROPERTIES FOLDER ${PROJECT_NAME})
else()
  message(STATUS "!!! Without Java Utils, this build will not generate required data files needed for Pulse to execute !!!")
  message(STATUS "!!! You will need to get these required data files from another build/source !!!")
endif()

if (NOT ${PROJECT_NAME}_PYTHON_API)
  message(STATUS "!!! Without Python Utils, this build will not generate files and utilities needed for Pulse testing and development !!!")
endif()

if(${PROJECT_NAME}_DOWNLOAD_BASELINES)
  message(STATUS "Pulling the latest Pulse Baselines!")
  add_custom_command(TARGET PulseData POST_BUILD
    COMMAND ${CMAKE_COMMAND} -DTYPE:STRING=updateBaselines -P run.cmake WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin
            WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin)
  set_target_properties (PulseData PROPERTIES FOLDER ${PROJECT_NAME})
else()
  message(STATUS "!!! NOT pulling the latest Pulse Baselines !!!")
endif()
