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
  set(Pulse_ENABLE_MULTI_PROCESS_BUILDS ON CACHE BOOL "Enable multi-process builds")
  set(PROCESSOR_COUNT "$ENV{NUMBER_OF_PROCESSORS}")
  set(Pulse_NUM_BUILD_PROCESSES ${PROCESSOR_COUNT} CACHE STRING "The maximum number of processes for the /MP flag")
  if(Pulse_ENABLE_MULTI_PROCESS_BUILDS)
    set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} /MP${Pulse_NUM_BUILD_PROCESSES}" CACHE STRING INTERNAL FORCE)
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /MP${Pulse_NUM_BUILD_PROCESSES}" CACHE STRING INTERNAL FORCE)
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

ExternalProject_Include_Dependencies( Pulse
  DEPENDS_VAR Pulse_DEPENDENCIES
  EP_ARGS_VAR Pulse_EP_ARGS
  SUPERBUILD_VAR Pulse_SUPERBUILD
  )


#-----------------------------------------------------------------------------
# Inner build of the main project
#-----------------------------------------------------------------------------
ExternalProject_Add( Pulse
  ${Pulse_EP_ARGS}
  DOWNLOAD_COMMAND ""
  SOURCE_DIR ${Pulse_SOURCE_DIR}
  BINARY_DIR ${CMAKE_BINARY_DIR}/Innerbuild
  ${COMMON_CMAKE_EP_ARGS}
  CMAKE_ARGS
    -DCMAKE_TOOLCHAIN_FILE:STRING=${CMAKE_TOOLCHAIN_FILE}
  CMAKE_CACHE_ARGS 
    -DPulse_SUPERBUILD:BOOL=OFF
    -DPulse_JAVA_API:BOOL=${Pulse_JAVA_API}
    -DPulse_JAVA_HOME_DIR:PATH=${Pulse_JAVA_HOME_DIR}
    -DPulse_CSHARP_API:BOOL=${Pulse_CSHARP_API}
    -DPulse_PYTHON_API:BOOL=${Pulse_PYTHON_API}
    -DPulse_PYTHON_HOME_DIR:PATH=${Pulse_PYTHON_HOME_DIR}
    -DPulse_AS_SHARED:BOOL=${Pulse_AS_SHARED}
    -DPulse_C_AS_STATIC:BOOL=${Pulse_C_AS_STATIC}
    -DPulse_NATIVE_BUILD_DIR:PATH=${Pulse_NATIVE_BUILD_DIR}
    -DPulse_MSVC_STATIC_RUNTIME:BOOL=${Pulse_MSVC_STATIC_RUNTIME}
    -DEigen3_DIR:PATH=${Eigen3_DIR}
    -Dprotobuf_DIR:PATH=${protobuf_DIR}
    -Dprotobuf_SRC:PATH=${protobuf_SRC}
    -Dprotobuf_INSTALL:PATH=${protobuf_INSTALL}
    -Dpybind11_DIR:PATH=${pybind11_DIR}
    -Dabsl_DIR:PATH=${absl_DIR}
    -Dutf8_range_DIR:PATH=${utf8_range_DIR}
  #INSTALL_COMMAND ${SKIP_STEP_COMMAND}
  DEPENDS ${Pulse_DEPENDENCIES}
)
set_target_properties (Pulse PROPERTIES FOLDER Pulse)

# Need Java Utils to generate data
if (Pulse_JAVA_API)
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
  set_target_properties (PulseData PROPERTIES FOLDER Pulse)
else()
  message(STATUS "!!! Without Java Utils, this build will not generate required data files needed for Pulse to execute !!!")
  message(STATUS "!!! You will need to get these required data files from another build/source !!!")
endif()

if (NOT Pulse_PYTHON_API)
  message(STATUS "!!! Without Python Utils, this build will not generate files and utilities needed for Pulse testing and development !!!")
endif()

if(Pulse_DOWNLOAD_BASELINES)
  message(STATUS "Pulling the latest Pulse Baselines!")
  add_custom_command(TARGET PulseData POST_BUILD
    COMMAND ${CMAKE_COMMAND} -DTYPE:STRING=updateBaselines -P run.cmake WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin
            WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin)
  set_target_properties (PulseData PROPERTIES FOLDER Pulse)
else()
  message(STATUS "!!! NOT pulling the latest Pulse Baselines !!!")
endif()
