# Always Install pulse python files, there is pure python functionality we use in development
install(DIRECTORY ${CMAKE_SOURCE_DIR}/src/python/pulse DESTINATION ${CMAKE_INSTALL_PREFIX}/python)

if(NOT Pulse_PYTHON_API)
  return()
endif()
find_package(pybind11 CONFIG REQUIRED)

set(_pypulse_dependencies MultiplexVentilationEngine)
set(_study_support_cpp pybind/MultiplexVentilation.cpp)
if(Pulse_AS_SHARED)
  message(WARNING "Python API will not include study classes since Pulse is a shared library")
  # Since Protobuf bind files are not exported from the dll and used directly in study classes
  set(_pypulse_dependencies Pulse)
  set(_study_support_cpp)
endif()

pybind11_add_module(PyPulse
  pybind/PyPulse.cpp
  pybind/PulseEngines.cpp
  ${_study_support_cpp}
  )
target_compile_definitions(PyPulse PRIVATE EXCLUDE_STUDIES)
target_include_directories(PyPulse PUBLIC ${CMAKE_SOURCE_DIR}/src/cpp)
target_include_directories(PyPulse PRIVATE ${pybind11_INCLUDE_DIRS} ${DST_ROOT}/cpp)

if(APPLE)
    set_target_properties(PyPulse PROPERTIES MACOSX_RPATH ON)
endif()

target_link_libraries(PyPulse
                      PUBLIC ${_pypulse_dependencies} pybind11::module
)

set_target_properties(PyPulse PROPERTIES
    DEBUG_POSTFIX "${PULSE_DEBUG_POSTFIX}"
    RELWITHDEBINFO_POSTFIX "${PULSE_RELWITHDEBINFO_POSTFIX}")

add_custom_command(TARGET PyPulse POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:PyPulse> ${CMAKE_INSTALL_PREFIX}/bin)

install(TARGETS PyPulse
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

set_target_properties (PyPulse PROPERTIES FOLDER Pulse)

# Install protobuf python support, we don't get this unless python api is enabled
install(DIRECTORY ${CMAKE_BINARY_DIR}/src/python/pulse DESTINATION ${CMAKE_INSTALL_PREFIX}/python)
install(DIRECTORY ${CMAKE_BINARY_DIR}/src/python/google DESTINATION ${CMAKE_INSTALL_PREFIX}/python)
