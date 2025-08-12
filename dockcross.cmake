if (Pulse_JAVA_API)
  if (INSTALLER STREQUAL "YUM")
    message(STATUS "Installing Java via YUM")
    execute_process(COMMAND sudo rpm --import https://yum.corretto.aws/corretto.key)
    execute_process(COMMAND sudo curl -L -o /etc/yum.repos.d/corretto.repo https://yum.corretto.aws/corretto.repo)
    execute_process(COMMAND sudo yum install -y java-1.8.0-amazon-corretto-devel)
  else() # I assume if you ain't yum, you are apt
    message(STATUS "Installing Java via APT")
    execute_process(COMMAND sudo apt-get -y install java-common)
    execute_process(COMMAND sudo wget https://d3pxv6yz143wms.cloudfront.net/8.232.09.1/java-1.8.0-amazon-corretto-jdk_8.232.09-1_amd64.deb)
    execute_process(COMMAND sudo dpkg --install java-1.8.0-amazon-corretto-jdk_8.232.09-1_amd64.deb)
  endif()
  execute_process(COMMAND export JAVA_HOME=/usr/lib/jvm/java-1.8.0-amazon-corretto)
else()
  message(STATUS "Not installing Java")
endif()
message(STATUS "Configuring cmake via container in ${CMAKE_CURRENT_SOURCE_DIR}")
execute_process(COMMAND cmake -DPulse_JAVA_API:BOOL=${Pulse_JAVA_API}
                              -DPulse_PYTHON_API:BOOL=OFF
                              -DPulse_C_AS_STATIC:BOOL=${Pulse_C_AS_STATIC}
                              -DPulse_NATIVE_BUILD_DIR:PATH=${Pulse_NATIVE_BUILD_DIR}
                              -B${BUILD_DIR}
                              -H${SRC_DIR}
                              -GNinja)
message(STATUS "Building Pulse...")
execute_process(COMMAND ninja -C${BUILD_DIR})
