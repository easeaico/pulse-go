include(bind/files.cmake)
include(cdm/files.cmake)
include(engine/files.cmake)

set(_filesystem)# Support for std::filesystem::experimental
if (CMAKE_COMPILER_IS_GNUCC AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9)
  set(_filesystem stdc++fs)
endif()

add_library_ex(Pulse ${${PROJECT_NAME}_LIB_TYPE}
  H_FILES
    ${CDM_H_FILES}
    ${ENGINE_H_FILES}
  CPP_FILES
    ${CDM_CPP_FILES}
    ${ENGINE_CPP_FILES}
  BIND_H_FILES
    ${BIND_H_FILES}
  BIND_CPP_FILES
    ${BIND_CPP_FILES}
  BIND_ROOT
    ${DST_ROOT}/cpp/pulse
  CONFIG_CPP_FILES
    PulseBuildInformation.cpp
  PUBLIC_DEPENDS
    protobuf::libprotobuf
  PRIVATE_DEPENDS
    ${_filesystem}
)
target_include_directories(Pulse PRIVATE ${EIGEN3_INCLUDE_DIR})

add_library_ex(PulseTests STATIC
  H_FILES
    ${CDM_TEST_H_FILES}
    ${ENGINE_TEST_H_FILES}
  CPP_FILES
    ${CDM_TEST_CPP_FILES}
    ${ENGINE_TEST_CPP_FILES}
  PUBLIC_DEPENDS
    Pulse
  LIB_INSTALL_ONLY

)
target_include_directories(PulseTests PRIVATE ${EIGEN3_INCLUDE_DIR})


if(${PROJECT_NAME}_C_AS_STATIC)
  return()
endif()

add_library_ex(Pulse ${${PROJECT_NAME}_LIB_TYPE}
  H_FILES
    ${CDM_H_FILES}
    ${ENGINE_H_FILES}
  CPP_FILES
    ${CDM_CPP_FILES}
    ${ENGINE_CPP_FILES}
  BIND_H_FILES
    ${BIND_H_FILES}
  BIND_CPP_FILES
    ${BIND_CPP_FILES}
  BIND_ROOT
    ${DST_ROOT}/cpp/pulse
  CONFIG_CPP_FILES
    PulseBuildInformation.cpp
  PUBLIC_DEPENDS
    protobuf::libprotobuf
  PRIVATE_DEPENDS
    ${_filesystem}
)
target_include_directories(Pulse PRIVATE ${EIGEN3_INCLUDE_DIR})

add_library_ex(PulseTests STATIC
  H_FILES
    ${CDM_TEST_H_FILES}
    ${ENGINE_TEST_H_FILES}
  CPP_FILES
    ${CDM_TEST_CPP_FILES}
    ${ENGINE_TEST_CPP_FILES}
  PUBLIC_DEPENDS
    Pulse
  LIB_INSTALL_ONLY

)
target_include_directories(PulseTests PRIVATE ${EIGEN3_INCLUDE_DIR})

add_subdirectory(howto)
add_subdirectory(scenario_driver)
add_subdirectory(test_driver)

if(${PROJECT_NAME}_AS_SHARED)
  return()
endif()
# Study libraries will not link since the protobuf bind files are not exporting
# Could probably make this happen if this is a needed build combination

add_subdirectory(study/circuit_optimization)
add_subdirectory(study/hydrocephalus)
add_subdirectory(study/multiplex_ventilation)
add_subdirectory(study/patient_variability)
add_subdirectory(study/sensitivity_analysis)