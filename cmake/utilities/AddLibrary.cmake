

macro(_subdir_list result curdir)
  file(GLOB children RELATIVE ${curdir} ${curdir}/*)
  set(dirlist "")
  foreach(child ${children})
    if(IS_DIRECTORY ${curdir}/${child})
      list(APPEND dirlist ${child})
    endif()
  endforeach()
  set(${result} ${dirlist})
endmacro()

function(add_library_ex target)

  set(options VERBOSE SHARED LIB_INSTALL_ONLY NO_INSTALL)
  set(oneValueArgs BIND_ROOT)
  set(multiValueArgs H_FILES BIND_H_FILES CONFIG_H_FILES
                     CPP_FILES BIND_CPP_FILES CONFIG_CPP_FILES
                     SUBDIR_LIST PUBLIC_DEPENDS PRIVATE_DEPENDS INSTALL_HEADER_DIR)
  include(CMakeParseArguments)
  cmake_parse_arguments(target "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

  message(STATUS "Configuring ${target}")
  string(TOLOWER ${target} _little_target)

  #-----------------------------------------------------------------------------
  # Verbose (display arguments)
  #-----------------------------------------------------------------------------
  if(target_VERBOSE)
    foreach(opt ${options} ${oneValueArgs} ${multiValueArgs})
      message(STATUS "${opt}:${target_${opt}}")
    endforeach()
  endif()

  #-----------------------------------------------------------------------------
  # Get files and directories
  #-----------------------------------------------------------------------------
  if( NOT target_H_FILES AND NOT target_CPP_FILES )
    message(STATUS "Auto adding source files")
    file(GLOB_RECURSE target_H_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
    file(GLOB_RECURSE target_CPP_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
    file(GLOB_RECURSE testing_FILES "${CMAKE_CURRENT_SOURCE_DIR}/test/*")
    if(testing_FILES)
      list(REMOVE_ITEM target_H_FILES ${testing_FILES})
      list(REMOVE_ITEM target_CPP_FILES ${testing_FILES})
    endif()
  endif()
  
  if( NOT target_SUBDIR_LIST )
    _subdir_list(target_SUBDIR_LIST ${CMAKE_CURRENT_SOURCE_DIR})
  endif()
  
  list(APPEND target_BUILD_INTERFACE_LIST "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>")
  foreach(subdir ${target_SUBDIR_LIST})
    if( NOT ${subdir} STREQUAL "test")
      list(APPEND target_BUILD_INTERFACE_LIST "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${subdir}>")
    endif()
  endforeach()

  #-----------------------------------------------------------------------------
  # Create target (library)
  #-----------------------------------------------------------------------------
  set(target_LIB_TYPE STATIC)
  if(target_SHARED)
    set(target_LIB_TYPE SHARED)
  endif()
  set(target_export_header ${CMAKE_CURRENT_BINARY_DIR}/${_little_target}_export.h)
  
  string(REPLACE ${CMAKE_SOURCE_DIR}/src/cpp "" REL_PATH ${CMAKE_CURRENT_SOURCE_DIR})
  foreach(h ${target_CONFIG_H_FILES})
    list(APPEND target_BUILD_CONFIG_H_FILES ${CMAKE_BINARY_DIR}/src/cpp/${REL_PATH}/${h})
  endforeach()
  foreach(cpp ${target_CONFIG_CPP_FILES})
    list(APPEND target_BUILD_CONFIG_CPP_FILES ${CMAKE_BINARY_DIR}/src/cpp/${REL_PATH}/${cpp})
  endforeach()
  
  add_library( ${target} ${target_LIB_TYPE}
    ${target_H_FILES}
    ${target_BIND_H_FILES}
    ${target_BUILD_CONFIG_H_FILES}
    ${target_export_header}
    ${target_CPP_FILES}
    ${target_BIND_CPP_FILES}
    ${target_BUILD_CONFIG_CPP_FILES}
    ) 
  include(GenerateExportHeader)
  generate_export_header(${target})

  if(target_SHARED)
    add_custom_command(TARGET ${target} POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${target}> ${CMAKE_INSTALL_PREFIX}/bin)
  else()
    add_custom_command(TARGET ${target} POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${target}> ${CMAKE_INSTALL_PREFIX}/lib)
  endif()

  #-----------------------------------------------------------------------------
  # Link libraries to current target
  #-----------------------------------------------------------------------------
  # Add dependent public targets
  foreach(d ${target_PUBLIC_DEPENDS})
    list(APPEND ${target}_PUBLIC_LIBRARIES "${d}")
  endforeach()
  if(target_VERBOSE)
    message(STATUS "${target} using public libraries : ${${target}_PUBLIC_LIBRARIES}")
  endif()
  target_link_libraries( ${target} PUBLIC
    ${${target}_PUBLIC_LIBRARIES}
    )
  # Add dependent private targets
  foreach(d ${target_PRIVATE_DEPENDS})
    list(APPEND ${target}_PRIVATE_LIBRARIES "${d}")
  endforeach()
  if(target_VERBOSE)
    message(STATUS "${target} using private libraries : ${${target}_PRIVATE_LIBRARIES}")
  endif()
  target_link_libraries( ${target} PRIVATE
    ${${target}_PRIVATE_LIBRARIES}
    )

  #-----------------------------------------------------------------------------
  # Include directories
  #-----------------------------------------------------------------------------
  target_include_directories( ${target}
    PUBLIC
      ${target_BUILD_INTERFACE_LIST}
      $<INSTALL_INTERFACE:include>
      $<INSTALL_INTERFACE:include/${${PROJECT_NAME}_INSTALL_FOLDER}>
    PRIVATE # Bind files
      ${DST_ROOT}/cpp
    )
  target_include_directories(${target} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
  target_include_directories(${target} PRIVATE ${CMAKE_SOURCE_DIR}/src/cpp)

  #-----------------------------------------------------------------------------
  # Set compile flags for the target
  #-----------------------------------------------------------------------------
  target_compile_options(${target} PRIVATE
                           $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
                                -Wall>
                           $<$<CXX_COMPILER_ID:MSVC>:
                                -W4 -MP>)

  if(NOT target_NO_INSTALL)
    if(NOT target_LIB_INSTALL_ONLY)
      #-----------------------------------------------------------------------------
      # Install headers
      #-----------------------------------------------------------------------------
      foreach(h ${target_H_FILES})
        #message(STATUS "Header at ${h}")
        get_filename_component(DEST_DIR ${h} DIRECTORY)
        #message(STATUS "Going to install include/${${PROJECT_NAME}_INSTALL_FOLDER}/${target_INSTALL_HEADER_DIR}/${DEST_DIR}/${h}")
        install(FILES
          ${h}
          DESTINATION include/${${PROJECT_NAME}_INSTALL_FOLDER}/${target_INSTALL_HEADER_DIR}/${DEST_DIR}
          COMPONENT Development
        )
      endforeach()
      install(FILES ${target_export_header} 
              DESTINATION include/${${PROJECT_NAME}_INSTALL_FOLDER}/${target_INSTALL_HEADER_DIR}
              COMPONENT Development
      )
    endif()
    
    #-----------------------------------------------------------------------------
    # Install library
    #-----------------------------------------------------------------------------
    install( TARGETS ${target} EXPORT ${PROJECT_NAME}Targets
      RUNTIME DESTINATION bin COMPONENT RuntimeLibraries
      LIBRARY DESTINATION lib COMPONENT RuntimeLibraries
      ARCHIVE DESTINATION lib COMPONENT Development
      )
  else()
    message(STATUS "Not installing ${target}")
  endif()

  #-----------------------------------------------------------------------------
  # Add the target to project folders
  #-----------------------------------------------------------------------------
  set_target_properties (${target} PROPERTIES FOLDER ${PROJECT_NAME})
  
  foreach(h ${target_H_FILES})
    list(APPEND target_FILES "${CMAKE_CURRENT_SOURCE_DIR}/${h}")
  endforeach()
  foreach(cpp ${target_CPP_FILES})
    list(APPEND target_FILES "${CMAKE_CURRENT_SOURCE_DIR}/${cpp}")
  endforeach()
  source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${target_H_FILES} ${target_CPP_FILES})

  foreach(h ${target_BIND_H_FILES})
    #file(RELATIVE_PATH h_rel ${target_BIND_ROOT} ${h})
    #message(STATUS "Header at ${h_rel}")
    list(APPEND target_BIND_FILES "${h}")
  endforeach()
  foreach(cpp ${target_BIND_CPP_FILES})
    #file(RELATIVE_PATH cpp_rel ${target_BIND_ROOT} ${cpp})
    #message(STATUS "Source at ${cpp_rel}")
    list(APPEND target_BIND_FILES "${cpp}")
  endforeach()
  source_group(TREE "${target_BIND_ROOT}" FILES ${target_BIND_FILES})

  # Configured files in the build directories
  source_group(TREE "${CMAKE_BINARY_DIR}/src/cpp/${REL_PATH}" FILES ${target_BUILD_CONFIG_H_FILES})
  source_group(TREE "${CMAKE_BINARY_DIR}/src/cpp/${REL_PATH}" FILES ${target_BUILD_CONFIG_CPP_FILES})

  source_group(TREE "${CMAKE_CURRENT_BINARY_DIR}" FILES ${target_export_header})
  
endfunction()
