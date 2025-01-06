cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS False)
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /EHsc")
endif()
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(UNIX AND NOT APPLE)
  set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib:$ORIGIN/")
endif()

if(POLICY CMP0091)# For msvc runtime
  cmake_policy(SET CMP0091 NEW)
endif()
if(POLICY CMP0094)# For finding python
  cmake_policy(SET CMP0094 NEW)
endif()
if(POLICY CMP0177)# For normalizing install paths
  cmake_policy(SET CMP0177 NEW)
endif()

if(NOT CMAKE_INSTALL_PREFIX OR CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  message(STATUS "Setting install dir inside build dir ${CMAKE_BINARY_DIR} ")
  set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install CACHE PATH "Install location" FORCE)
else()
  message(STATUS "Using preset install directory ${CMAKE_INSTALL_PREFIX}")
endif()

#------------------------------------------------------------------------------
# Set a default build type if none was specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
  mark_as_advanced(CMAKE_BUILD_TYPE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

set(CMAKE_DEBUG_POSTFIX "d")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

project(Pulse VERSION 4.3.1 LANGUAGES C CXX)
include(GNUInstallDirs)

function(remove_cmake_variables)
    get_cmake_property(_variableNames VARIABLES)
    list (SORT _variableNames)
    foreach (_variableName ${_variableNames})
        if ((NOT DEFINED ARGV0) OR _variableName MATCHES ${ARGV0})
            # message(STATUS "Removing ${_variableName}=${${_variableName}}")
            unset(${_variableName} CACHE)
        endif()
    endforeach()
endfunction()

#-----------------------------------------------------------------------------
# Project install directories
#-----------------------------------------------------------------------------
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install CACHE PATH "Install location" FORCE)
endif()
set(CMAKE_PREFIX_PATH ${CMAKE_INSTALL_PREFIX})
# Let's go ahead and make these directories
file(MAKE_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin)
file(MAKE_DIRECTORY ${CMAKE_INSTALL_PREFIX}/include)
file(MAKE_DIRECTORY ${CMAKE_INSTALL_PREFIX}/lib)

#-----------------------------------------------------------------------------
# Update CMake module path & cmake dir
#-----------------------------------------------------------------------------
set(CMAKE_MODULE_PATH
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/utilities
    ${CMAKE_INSTALL_PREFIX}
    ${CMAKE_INSTALL_PREFIX}/lib/cmake # Many packages install their cmake to lib
    )
set(Pulse_CMAKE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# C_AS_STATIC builds a Pulse library for use in iOS builds
# AS_SHARED builds a Pulse library for use by Unreal

option(Pulse_AS_SHARED "Build Pulse as a shared library (for use in Unreal builds)" OFF)
option(Pulse_C_AS_STATIC "Build PulseC as a static library (for use in iOS/WASM builds)" OFF)
option(Pulse_DOWNLOAD_BASELINES "Download all V&V Scenarios and their baseline result files" OFF)
option(Pulse_CSHARP_API "Build C# API" OFF)
option(Pulse_JAVA_API "Build Java API and utils for data generation and testing" ON)
option(Pulse_PYTHON_API "Build Python API and utils for data generation and testing" ON)
option(Pulse_GEN_DATA "Generate Pulse data. Requires JAVA and PYTHON API" ON)
option(Pulse_DEPENDENT_BUILD "Pulse is being built as part of a different superbuild" OFF)
option(Pulse_MSVC_STATIC_RUNTIME "Link static runtime libraries" OFF)
# This dir inteded for use during cross compiling, provide the directory to the generated bindings from a native pulse build
# This will be in a <build>/Innerbuild/src/schema
set(Pulse_NATIVE_BUILD_DIR "" CACHE PATH "For crosscompiling, provide the root folder of a native build with protobuf bindings")
set(NATIVE_BIND_DIR "")
if(EXISTS ${Pulse_NATIVE_BUILD_DIR})
  message(STATUS "Checking native bind directory...")
  set(NATIVE_BIND_DIR ${Pulse_NATIVE_BUILD_DIR}/Innerbuild/src/cpp/pulse)
  if(NOT EXISTS ${NATIVE_BIND_DIR})
    message(FATAL_ERROR "Could not find expected binding directory in provided NATIVE_BUILD_DIR, expecting:  ${NATIVE_BIND_DIR}")
  endif()
endif()
# Variable configurations between a static and a shared build
set(_private_protobuf)
set(Pulse_LIB_TYPE STATIC)
if (NOT Pulse_AS_SHARED)
  set(_private_protobuf protobuf::libprotobuf)
else()
  if(Pulse_C_AS_STATIC)
    set(Pulse_C_AS_STATIC OFF CACHE BOOL INTERNAL FORCE)
    message(WARNING "Disabling Pulse_C_AS_STATIC, as Pulse_AS_SHARED is enabled")
  endif()
  set(Pulse_LIB_TYPE SHARED)
endif()

if (MSVC)
  # Allow the MP flag to get set externally
  set(Pulse_ENABLE_MULTI_PROCESS_BUILDS ON CACHE BOOL "Enable multi-process builds")
  set(PROCESSOR_COUNT "$ENV{NUMBER_OF_PROCESSORS}")
  set(Pulse_NUM_BUILD_PROCESSES ${PROCESSOR_COUNT} CACHE STRING "The maximum number of processes for the /MP flag")
  if(Pulse_ENABLE_MULTI_PROCESS_BUILDS)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP${Pulse_NUM_BUILD_PROCESSES}" CACHE STRING INTERNAL FORCE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP${Pulse_NUM_BUILD_PROCESSES}" CACHE STRING INTERNAL FORCE)
  endif()
endif()

#-----------------------------------------------------------------------------
# iOS Support
#-----------------------------------------------------------------------------
if(iOS_TOOLCHAIN)
  message(STATUS "Setting up build for iOS")

  if(POLICY CMP0114)# For new xcode build system
    cmake_policy(SET CMP0114 NEW)
  endif()

  set(Pulse_C_AS_STATIC ON CACHE BOOL INTERNAL FORCE)
  if(Pulse_CSHARP_API)
    set(Pulse_CSHARP_API OFF CACHE BOOL INTERNAL FORCE)
    message(WARNING "Disabling C# API for iOS build")
  endif()
  if(Pulse_JAVA_API)
    set(Pulse_JAVA_API OFF CACHE BOOL INTERNAL FORCE)
    message(WARNING "Disabling Java API for iOS build")
  endif()
  if(Pulse_PYTHON_API)
    set(Pulse_PYTHON_API OFF CACHE BOOL INTERNAL FORCE)
    message(WARNING "Disabling Python API for iOS build")
  endif()
  if(Pulse_GEN_DATA)
    set(Pulse_GEN_DATA OFF CACHE BOOL INTERNAL FORCE)
    message(WARNING "Disabling data generation for iOS build")
  endif()

  if(NOT EXISTS ${NATIVE_BIND_DIR})
    message(FATAL_ERROR "Crosscompiling for iOS, you must provide Pulse_NATIVE_BUILD_DIR")
  endif()

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fembed-bitcode")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fembed-bitcode")
endif()

if (Pulse_JAVA_API)
  set(Pulse_JAVA_HOME_DIR "" CACHE PATH "Directory containing the Java version to use")
  if(Pulse_JAVA_HOME_DIR)
    set(ENV{JAVA_HOME} ${Pulse_JAVA_HOME_DIR})
    message(STATUS "Looking for your specific Java in ${Pulse_JAVA_HOME_DIR}")
  else()
    message(STATUS "Looking for Java in : $ENV{JAVA_HOME}")
  endif()
  remove_cmake_variables(Java_)
  # Pulse uses Java in its development utility scripts
  find_package(Java)
  if(Java_FOUND)
    include(UseJava)
    if(Java_JAVA_EXECUTABLE)
      message(STATUS "Using Java executable : ${Java_JAVA_EXECUTABLE}")
      set(Java_CMD ${Java_JAVA_EXECUTABLE})
    else()
      message(STATUS "Could not find Java_JAVA_EXECUTABLE")
    endif()
  else()
    message(WARNING "The Pulse `run` utility uses Java, which could not be found")
    message(WARNING "To use the `run` utility, setup Java in this environment and reconfigure cmake")
  endif()
  
  # Running into this problem when running Pulse through certain JVMs
  # There may be a better fix in the future, but for now  
  # https://stackoverflow.com/questions/78598141/first-stdmutexlock-crashes-in-application-built-with-latest-visual-studio
  if (MSVC)
    add_compile_definitions(_DISABLE_CONSTEXPR_MUTEX_CONSTRUCTOR)
  endif()
else()
  remove_cmake_variables(Java_)
endif()

set(Python_CMD "python")
if (Pulse_PYTHON_API)
  set(Pulse_PYTHON_HOME_DIR "" CACHE PATH "Directory containing the python interpreter to use")
  # Pulse uses Python in its development utility scripts
  if(Pulse_PYTHON_HOME_DIR)
    set(Python_ROOT_DIR ${Pulse_PYTHON_HOME_DIR})
    set(Python3_ROOT_DIR ${Pulse_PYTHON_HOME_DIR})
    message(STATUS "Looking for your specific python in ${Pulse_PYTHON_HOME_DIR}")
  endif()
  remove_cmake_variables(Python3_)
  remove_cmake_variables(_Python3)
  find_package(Python3 COMPONENTS Interpreter Development)
  if(Python3_FOUND)
    message(STATUS "Using Python: ${Python3_EXECUTABLE}")
    set(Python_CMD ${Python3_EXECUTABLE})
  else()
    set(Python_CMD "python")
    message(WARNING "The Pulse `run` utility uses python, which could not be found")
    message(WARNING "To use the `run` utility, setup python in this environment and reconfigure cmake")
  endif()
else()
  remove_cmake_variables(Python3_)
  remove_cmake_variables(_Python3)
endif()

#-----------------------------------------------------------------------------
# SUPERBUILD
#-----------------------------------------------------------------------------
option(Pulse_SUPERBUILD "Initial pull and build of all dependent libraries/executables" ON)
if(Pulse_SUPERBUILD)
  #-----------------------------------------------------------------------------
  # Define External dependencies
  #-----------------------------------------------------------------------------
  macro(define_dependency extProj)
    list(APPEND Pulse_DEPENDENCIES ${extProj})
    option(USE_SYSTEM_${extProj} "Exclude ${extProj} from superbuild and use an existing build." OFF)
    mark_as_advanced(USE_SYSTEM_${extProj})
  endmacro()

  define_dependency(absl)
  define_dependency(Eigen3)
  define_dependency(protobuf)
  message(STATUS "We are using protobuf ${Protobuf_VERSION}")

  if(Pulse_PYTHON_API)
    if(Python3_FOUND)
      define_dependency(pybind11)
      message(STATUS "Using python: ${Python3_EXECUTABLE}")
      else()
        message(FATAL_ERROR "Unable to support Python API, cannot find Python3")
    endif()
  endif()

  if (Pulse_GEN_DATA)
    if (NOT Pulse_JAVA_API OR NOT Pulse_PYTHON_API)
      set(Pulse_GEN_DATA OFF CACHE BOOL INTERNAL FORCE)
      message(STATUS "!!! Disabling data generation in build !!!")
      message(STATUS "!!! You need both Python and JAVA API enabled to generate data. !!!")
      message(STATUS "!!! You will need to get these required data files from another build/source !!!")
    endif()
  endif()

  if(Pulse_DEPENDENCIES)
    if(MSVC OR XCode)
      if(NOT Pulse_DEPENDENT_BUILD)
        message(STATUS "Optimizing your superbuild experience")
        set(Pulse_MULTI_BUILD ON)
        set(CMAKE_CONFIGURATION_TYPES Release CACHE STRING INTERNAL FORCE)
      else()
        set(Pulse_MULTI_BUILD OFF)
        set(CMAKE_CONFIGURATION_TYPES Debug MinSizeRel Release RelWithDebInfo CACHE STRING INTERNAL FORCE)
      endif()
    endif()
    #-----------------------------------------------------------------------------
    # Solve project dependencies
    #-----------------------------------------------------------------------------
    # Call CMakeLists.txt in cmake/external which will solve the dependencies
    # and add the external projects, including this one: this top-level
    # CMakeLists.txt will be called back with SUPERBUILD=OFF, to execute
    # the rest of the code below (INNERBUILD), which explains the `return`
    add_subdirectory(cmake/external)
    return()
  else()
    message(STATUS "No dependencies, skipping superbuild")
  endif()
endif()

#-----------------------------------------------------------------------------
#                               INNERBUILD
#-----------------------------------------------------------------------------

if(MSVC)
  # /Ob2 - Inline function expansion (Any)
  # /Oi - Enable intrinsic function 
  set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /Ob2 /Oi")
  set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /Ob2 /Oi")
  set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} /Ob2 /Oi")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Ob2 /Oi")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Ob2 /Oi")
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /Ob2 /Oi")
  if(Pulse_MSVC_STATIC_RUNTIME)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
  else()
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
  endif()
endif()

if ( CMAKE_COMPILER_IS_GNUCC )
  # Protobuf is not using the same variable name in its swap specificiation definitions
  # Resulting in a LOT of warnings per file that includes protobuf headers, slowing the build down
  set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wno-attributes")
  if(CMAKE_BUILD_TYPE EQUAL "Debug")
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wa,-mbig-obj")
  endif()
endif()

set(Pulse_INSTALL_FOLDER pulse)

include(Find)# Our find macros
include(AddLibrary)# Extras to do when creating a library
include(AddExecutable)# Extras to do when creating an executable

#-----------------------------------------------------------------------------
# Find external dependencies
#-----------------------------------------------------------------------------

find_package(Eigen3 REQUIRED)

# Tell protobuf-config.cmake to try to be compatible with FindProtobuf.cmake;
# this makes it easier for us to work with either one
set(protobuf_MODULE_COMPATIBLE ON CACHE BOOL "CMake built-in FindProtobuf.cmake module compatible")
mark_as_advanced(protobuf_MODULE_COMPATIBLE)
if(iOS_TOOLCHAIN)
  set(protobuf_MODULE_COMPATIBLE OFF)
endif()

# Try to use the config written by protobuf
find_package(protobuf QUIET CONFIG NO_DEFAULT_PATH)
if(protobuf_DIR AND protobuf_FOUND)
  # Success; set flag so we will reexport protobuf for our users
  set(Pulse_USING_PROTOBUF_DIR ON)
elseif(NOT protobuf_FOUND)
  message(STATUS "Could not find protobuf.cmake, falling back to CMake's FindProtobuf.cmake")
  # Nope; fall back on FindProtobuf.cmake
  find_package(Protobuf REQUIRED)
endif()

if(MSVC OR XCode)
  # By default, CMake will choose debug for relwithdebinfo configurations if a relwithdebinfo export package is not provided.
  # This is the case when Pulse builds protobuf. (It only builds release and debug for MSVC)
  # This default behavior prevents the Pulse relwithdebinfo from building. (Link mismatch)
  # So I am just forcing these non standard configurations to release in multiconfig builders
  set_target_properties(protobuf::libprotobuf PROPERTIES
    MAP_IMPORTED_CONFIG_MINSIZEREL Release
    MAP_IMPORTED_CONFIG_RELWITHDEBINFO Release)
endif()

set(SCHEMA_SRC "${CMAKE_SOURCE_DIR}/schema")
set(SCHEMA_DST "${CMAKE_SOURCE_DIR}/schema/bind")

add_subdirectory(src)
add_subdirectory(data)

#--------------------------------------------------------------------------
# Setup development utility scripts
#--------------------------------------------------------------------------

if(NOT WIN32)
  # Create setup shell script to create an environment for running examples
  set(LIBRARY_PATH_VAR "LD_LIBRARY_PATH")
  if( APPLE )
    set(LIBRARY_PATH_VAR "DYLD_FALLBACK_LIBRARY_PATH")
  endif()
endif()

file(GLOB JSON_FILES ${CMAKE_SOURCE_DIR}/bin/*.json)
file(COPY ${JSON_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
if(WIN32)
  file(COPY ${CMAKE_SOURCE_DIR}/bin/run.bat DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
else()
  file(COPY ${CMAKE_SOURCE_DIR}/bin/run.sh DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
endif()
file(COPY ${CMAKE_SOURCE_DIR}/bin/resource DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
configure_file(${CMAKE_SOURCE_DIR}/bin/rebase.py.in "${CMAKE_INSTALL_PREFIX}/bin/rebase.py" @ONLY)
configure_file(${CMAKE_SOURCE_DIR}/bin/run.cmake.in "${CMAKE_INSTALL_PREFIX}/bin/run.cmake" @ONLY)
configure_file(${CMAKE_SOURCE_DIR}/bin/run.config.in "${CMAKE_INSTALL_PREFIX}/bin/run.config" @ONLY)
configure_file(${CMAKE_SOURCE_DIR}/docs/Doxygen/full.doxy.in ${CMAKE_INSTALL_PREFIX}/bin/docs/full.doxy @ONLY)
configure_file(${CMAKE_SOURCE_DIR}/docs/Doxygen/report.doxy.in ${CMAKE_INSTALL_PREFIX}/bin/docs/report.doxy @ONLY)


#-----------------------------------------------------------------------------
# This variable controls the prefix used to generate the following files:
#  PulseConfigVersion.cmake
#  PulseConfig.cmake
#  PulseTargets.cmake
# and it also used to initialize Pulse_INSTALL_CONFIG_DIR value.
set(export_config_name Pulse)
set(Pulse_INSTALL_CONFIG_DIR "lib/cmake/${Pulse_INSTALL_FOLDER}")
file(MAKE_DIRECTORY ${CMAKE_INSTALL_PREFIX}/${Pulse_INSTALL_CONFIG_DIR})
# Get the latest abbreviated commit hash of the working branch
find_package(Git)
if(Git_FOUND)
  execute_process(
    COMMAND ${GIT_EXECUTABLE} log -1 --format=%h
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    OUTPUT_VARIABLE export_git_hash
    OUTPUT_STRIP_TRAILING_WHITESPACE
    )
else()
  set(export_git_hash "unknown")
endif()

configure_file(cmake/PulseBuildInformation.cpp.in ${CMAKE_BINARY_DIR}/src/cpp/PulseBuildInformation.cpp)

#------------------------------------------------------------------------------
# Configure PulseConfigVersion.cmake common to build and install tree
include(CMakePackageConfigHelpers)
set(config_version_file ${PROJECT_BINARY_DIR}/PulseConfigVersion.cmake)
write_basic_package_version_file(
  ${config_version_file}
  VERSION "${Pulse_VERSION}"
  COMPATIBILITY ExactVersion
  )
#------------------------------------------------------------------------------
# Export 'PulseTargets.cmake' for a build tree
export(
  EXPORT PulseTargets
  NAMESPACE Pulse::
  FILE "${CMAKE_CURRENT_BINARY_DIR}/PulseTargets.cmake"
  )

# Configure 'PulseConfig.cmake' for a build tree
set(build_config ${CMAKE_BINARY_DIR}/PulseConfig.cmake)
configure_package_config_file(
  cmake/PulseConfigBuild.cmake.in
  ${build_config}
  INSTALL_DESTINATION "${PROJECT_BINARY_DIR}"
  )

#------------------------------------------------------------------------------
# Export 'PulseTargets.cmake' for an install tree
install(
  EXPORT PulseTargets
  NAMESPACE Pulse::
  FILE PulseTargets.cmake
  DESTINATION ${Pulse_INSTALL_CONFIG_DIR}
  )

set(install_config ${PROJECT_BINARY_DIR}/CMakeFiles/PulseConfig.cmake)
configure_package_config_file(
  cmake/PulseConfigInstall.cmake.in
  ${install_config}
  INSTALL_DESTINATION ${Pulse_INSTALL_CONFIG_DIR}
  )

# Install config files
install(
  FILES ${config_version_file} ${install_config}
  DESTINATION "${Pulse_INSTALL_CONFIG_DIR}"
  )
