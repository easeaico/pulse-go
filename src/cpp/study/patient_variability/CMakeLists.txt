add_library_ex(PatientVariabilityEngine STATIC
  H_FILES
    ActionIteration.h
    PatientIteration.h
    ScenarioIteration.h
    TCCCIteration.h
    ValidationIteration.h
  CPP_FILES
    ActionIteration.cpp
    PatientIteration.cpp
    ScenarioIteration.cpp
    TCCCIteration.cpp
    ValidationIteration.cpp
  PUBLIC_DEPENDS
    Pulse
  INSTALL_HEADER_DIR
    study/patient_variability
  NO_INSTALL
  )
set_target_properties(PatientVariabilityEngine PROPERTIES FOLDER "Studies//PatientVariability")

add_executable_ex(PatientVariabilityDriver main.cpp)
set_target_properties(PatientVariabilityDriver PROPERTIES FOLDER "Studies//PatientVariability")
target_link_libraries(PatientVariabilityDriver PatientVariabilityEngine protobuf::libprotobuf)
