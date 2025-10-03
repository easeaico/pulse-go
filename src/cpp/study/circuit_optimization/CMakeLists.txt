add_library_ex(CircuitOptimizationEngine STATIC
  H_FILES
    CircuitOptimizer.h
  CPP_FILES
    CircuitOptimizer.cpp
  PUBLIC_DEPENDS
    Pulse
  INSTALL_HEADER_DIR
    study/circuit_optimization
  NO_INSTALL
  PRIVATE_DEPENDS
    Eigen3::Eigen
  )
set_target_properties(CircuitOptimizationEngine PROPERTIES FOLDER "Studies//Circuit Optimization")

add_executable_ex(CircuitOptimizationDriver main.cpp)
set_target_properties(CircuitOptimizationDriver PROPERTIES FOLDER "Studies//Circuit Optimization")
target_link_libraries(CircuitOptimizationDriver CircuitOptimizationEngine protobuf::libprotobuf)