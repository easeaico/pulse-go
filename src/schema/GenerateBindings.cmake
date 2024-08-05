
set(SCHEMA_SRC ${SRC_ROOT}/schema)
set(CDM_DIR "pulse/cdm/bind")
set(ENGINE_DIR "pulse/engine/bind")
set(STUDY_DIR "pulse/study/bind")
# Let the build also know package locations
get_directory_property(hasParent PARENT_DIRECTORY)
if(hasParent)
  set(CDM_PACKAGE ${CDM_DIR} PARENT_SCOPE)
  set(ENGINE_PACKAGE ${ENGINE_DIR} PARENT_SCOPE)
  set(STUDY_PACKAGE ${STUDY_DIR} PARENT_SCOPE)
endif()

if(EXISTS ${NATIVE_BIND_DIR})
  # Let's copy this directory to our build
  message(STATUS "!! NOT generating bindings !!!")
  message(STATUS "Copying binding directory from " ${NATIVE_BIND_DIR})
  message(STATUS " to " ${CMAKE_BINARY_DIR}/src/cpp)
  file(COPY ${NATIVE_BIND_DIR} DESTINATION ${CMAKE_BINARY_DIR}/src/cpp)
  return()
endif()

if( Protobuf_PROTOC_EXECUTABLE )
  set(BINDER ${Protobuf_PROTOC_EXECUTABLE})
else()#
  message(FATAL_ERROR "Where is the protoc binder?")
endif()

if( NOT protobuf_SRC )
  set(protobuf_SRC CACHE PATH "Location of protobuf source code")
  message(FATAL_ERROR "Please specify protobuf_SRC")
else()
  message(STATUS "protobuf_SRC is located at ${protobuf_SRC}")
endif()

macro(delete_bindings _root)
  file(GLOB _OLD_BIND_FILES "${_root}/*")
  if(_OLD_BIND_FILES)
    file(REMOVE ${_OLD_BIND_FILES})
  endif()
endmacro()


message(STATUS "Generating Schema Bindings" )
message(STATUS "Using : ${BINDER}")
message(STATUS "SRC_ROOT: ${SRC_ROOT}")
message(STATUS "SCHEMA_SRC: ${SCHEMA_SRC}")

file(GLOB_RECURSE _FILES "${SCHEMA_SRC}/*.proto")

set(_RUN_PROTOC OFF)
if(EXISTS ${DST_ROOT}/schema_last_built)
  foreach(f ${_FILES})
    if(${f} IS_NEWER_THAN ${DST_ROOT}/schema_last_built)
      message(STATUS "${f} has changed since the last build")
      set(_RUN_PROTOC ON)
    endif()
  endforeach()
else()
  set(_RUN_PROTOC ON)
endif()
if(FORCE)
  set(_RUN_PROTOC ON)
endif()

if(NOT _RUN_PROTOC)
  message(STATUS "Not generating bindings, nothing has changed since last build")
  return()
endif()

##################
## C++ Bindings ##
##################

set(cpp_bindings_DIR "${DST_ROOT}/cpp")
file(MAKE_DIRECTORY "${cpp_bindings_DIR}")
delete_bindings(${cpp_bindings_DIR})
foreach(f ${_FILES})
  message(STATUS "C++ Binding file ${f}")
  execute_process(COMMAND ${BINDER} --proto_path=${SCHEMA_SRC}
                                    --cpp_out=${cpp_bindings_DIR}
                                    # or 
                                    #--cpp_out=dllexport_decl=_DECL:${cpp_bindings_DIR}
                                    ${f})
endforeach()
message(STATUS "cpp bindings are here : ${cpp_bindings_DIR}" )

if(Pulse_JAVA_API)
  ###################
  ## Java Bindings ##
  ###################

  set(java_bindings_DIR "${DST_ROOT}/java")
  file(MAKE_DIRECTORY "${java_bindings_DIR}")
  delete_bindings(${java_bindings_DIR})

  #if(NOT EXISTS "${protobuf_SRC}/java/core/src/main/java/com/google/protobuf/Any.java")
    message(STATUS "Generating Java Protobuf files")
    set(__API_PROTO_FILES any.proto
                          any_test.proto
                          api.proto
                          descriptor.proto
                          duration.proto
                          empty.proto
                          field_mask.proto
                          #map_lite_unittest.proto
                          #map_proto2_unittest.proto
                          #map_unittest.proto
                          source_context.proto
                          struct.proto
                          #test_messages_proto2.proto
                          #test_messages_proto3.proto
                          timestamp.proto
                          type.proto
                          #unittest.proto
                          #unittest_arena.proto
                          #unittest_custom_options.proto
                          #unittest_drop_unknown_fields.proto
                          #unittest_embed_optimize_for.proto
                          #unittest_empty.proto
                          #unittest_enormous_descriptor.proto
                          #unittest_import.proto
                          #unittest_import_lite.proto
                          #unittest_import_public.proto
                          #unittest_import_public_lite.proto
                          #unittest_lazy_dependencies.proto
                          #unittest_lazy_dependencies_custom_option.proto
                          #unittest_lazy_dependencies_enum.proto
                          #unittest_lite.proto
                          #unittest_lite_imports_nonlite.proto
                          #unittest_mset.proto
                          #unittest_mset_wire_format.proto
                          #unittest_no_arena.proto
                          #unittest_no_arena_import.proto
                          #unittest_no_arena_lite.proto
                          #unittest_no_field_presence.proto
                          #unittest_no_generic_services.proto
                          #unittest_optimize_for.proto
                          #unittest_preserve_unknown_enum.proto
                          #unittest_preserve_unknown_enum2.proto
                          #unittest_proto3.proto
                          #unittest_proto3_arena.proto
                          #unittest_proto3_arena_lite.proto
                          #unittest_proto3_lite.proto
                          #unittest_well_known_types.proto
                          wrappers.proto
                          )

    #Generate the java API files from their proto files
    foreach(f ${__API_PROTO_FILES})
      execute_process(COMMAND ${BINDER} --proto_path=${protobuf_SRC}/src/
                                        --java_out=${java_bindings_DIR}
                                          "${protobuf_SRC}/src/google/protobuf/${f}")
      message(STATUS "Java Binding file ${protobuf_SRC}/src/google/protobuf/${f}")
    endforeach()
    execute_process(COMMAND ${BINDER} --proto_path=${protobuf_SRC}/src/
                                        --java_out=${java_bindings_DIR}
                                          "java/core/src/main/resources/google/protobuf/java_features.proto")
    message(STATUS "Java Binding file ${protobuf_SRC}/java/core/src/main/resources/google/protobuf/java_features.proto")

  # Copy these files to our source directory
  file(COPY "${protobuf_SRC}/java/core/src/main/java/com"
       DESTINATION ${java_bindings_DIR}
  )
  file(COPY "${protobuf_SRC}/java/util/src/main/java/com"
       DESTINATION ${java_bindings_DIR}
  )

  foreach(f ${_FILES})
    message(STATUS "Java Binding file ${f}")
    execute_process(COMMAND ${BINDER} --proto_path=${SCHEMA_SRC}
                                      --java_out=${java_bindings_DIR}
                                      ${f})
  endforeach()
  message(STATUS "java bindings are here : ${java_bindings_DIR}" )
endif()

#################
## C# Bindings ##
#################
if(Pulse_CSHARP_API)
  set(csharp_bindings_DIR "${DST_ROOT}/csharp")
  file(MAKE_DIRECTORY "${csharp_bindings_DIR}")
  delete_bindings(${csharp_bindings_DIR})
  foreach(f ${_FILES})
    message(STATUS "C# Binding file ${f}")
    execute_process(COMMAND ${BINDER} --proto_path=${SCHEMA_SRC}
                                      --csharp_out=${csharp_bindings_DIR}
                                      --csharp_opt=base_namespace
                                      ${f})
  endforeach()
  message(STATUS "csharp bindings are here : ${csharp_bindings_DIR}" )
endif()

if(Pulse_PYTHON_API)
  #####################
  ## Python Bindings ##
  #####################
  find_package (Python3 COMPONENTS Interpreter)
  if(Python3_FOUND)
    set(python_bindings_DIR "${DST_ROOT}/python")
    file(MAKE_DIRECTORY "${python_bindings_DIR}")
    delete_bindings(${python_bindings_DIR})
    
    set( ENV{PROTOC} ${BINDER} )
    foreach(f ${_FILES})
      message(STATUS "Python Binding file ${f}")
      execute_process(COMMAND ${BINDER} --proto_path=${SCHEMA_SRC}
                                        --python_out=${python_bindings_DIR}
                                        ${f})
    endforeach()
    # Hard coded for now, will need to be more clever on how it finds each file.
    # Necessary for setup.py file to find the bindings to install.
    file(COPY "${SRC_ROOT}/python/__init__.py" DESTINATION "${python_bindings_DIR}")
    file(COPY "${SRC_ROOT}/python/__init__.py" DESTINATION "${python_bindings_DIR}/pulse")
    file(COPY "${SRC_ROOT}/python/__init__.py" DESTINATION "${python_bindings_DIR}/pulse/cdm")
    file(COPY "${SRC_ROOT}/python/__init__.py" DESTINATION "${python_bindings_DIR}/pulse/engine")
    file(COPY "${SRC_ROOT}/python/__init__.py" DESTINATION "${python_bindings_DIR}/pulse/study")
    file(COPY "${SRC_ROOT}/python/__init__.py" DESTINATION "${python_bindings_DIR}/pulse/cdm/bind")
    file(COPY "${SRC_ROOT}/python/__init__.py" DESTINATION "${python_bindings_DIR}/pulse/engine/bind")
    file(COPY "${SRC_ROOT}/python/__init__.py" DESTINATION "${python_bindings_DIR}/pulse/study/bind")
    message(STATUS "python bindings are here : ${python_bindings_DIR}" )
  endif()
endif()

file(TOUCH ${DST_ROOT}/schema_last_built)
message(STATUS "Touch file ${DST_ROOT}/schema_last_built")
