# Where am I
message(STATUS "I am in ${CMAKE_CURRENT_SOURCE_DIR}")
# Make our build directory
cmake_path(GET CMAKE_CURRENT_SOURCE_DIR PARENT_PATH root_dir)
cmake_path(GET CMAKE_CURRENT_SOURCE_DIR FILENAME src_dir)
set(build_root "${root_dir}/dockcross-builds")
set(build_to "${build_root}/${IMAGE}")

if (EXISTS "${build_to}")
  message(STATUS "Deleting existing directory: ${build_to}")
  file(REMOVE_RECURSE  ${build_to})
endif()
message(STATUS "Creating build directory: ${build_to}")
file(MAKE_DIRECTORY ${build_to})

set(C_AS_STATIC OFF)
if(IMAGE STREQUAL "web-wasm")
  set(C_AS_STATIC ON)
  message(STATUS "Building static libraries")
endif()
  
set(NATIVE_BUILD_DIR)
if (DEFINED BIND)
  set(NATIVE_BUILD_DIR "/work/dockcross-builds/${BIND}")
  message(STATUS "Using bindings from ${build_root}/${BIND}")
else()
  message(STATUS "Will generate my own bindings")
endif()

# If we are on windows, we run docker via the Windows Subsystem for Linux
set(wsl)
if (WIN32)
  set(wsl "wsl")
endif()

if (EXISTS ${root_dir}/${IMAGE})
  message(STATUS "Found the docker script, no need to make it again")
else()
  # Create a script to run this dockcross container
  message(STATUS "Creating docker script ${root_dir}/${IMAGE}")
  execute_process(COMMAND ${wsl} docker run --rm dockcross/${IMAGE} > ./${IMAGE}
                  WORKING_DIRECTORY ${root_dir})

  message(STATUS "Changine permissions on ${root_dir}/${IMAGE}")
  execute_process(COMMAND ${wsl} ./${IMAGE} chmod +x ./${IMAGE}
                  WORKING_DIRECTORY ${root_dir})
endif()

# Now run the build script through the docker container
message(STATUS "Running CMake through the container")
if (NOT DEFINED INSTALLER)
  set(INSTALLER "APT") # Default
endif()
execute_process(COMMAND ${wsl} ./${IMAGE} cmake -DPulse_JAVA_API:BOOL=${JAVA}
                                            -DPulse_C_AS_STATIC:BOOL=${C_AS_STATIC}
                                            -DPulse_NATIVE_BUILD_DIR:PATH=${NATIVE_BUILD_DIR}
                                            -DINSTALLER:STRING=${INSTALLER}
                                            -DBUILD_DIR:PATH=dockcross-builds/${IMAGE}
                                            -DSRC_DIR:PATH=${src_dir}
                                            -P ./${src_dir}/dockcross.cmake
                WORKING_DIRECTORY ${root_dir})