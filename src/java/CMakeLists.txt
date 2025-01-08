if(NOT Pulse_JAVA_API)
  return()
endif()

find_package(Java COMPONENTS Development)
include(UseJava)
find_package(JNI REQUIRED)

add_library_ex(PulseJNI SHARED
  H_FILES
    jni/PulseEngineJNI.h
  CPP_FILES
    jni/CommonDataModelJNI.cpp
    jni/PulseEngineJNI.cpp
  PUBLIC_DEPENDS
    PulseTests
  LIB_INSTALL_ONLY
)
target_include_directories(PulseJNI PRIVATE ${JNI_INCLUDE_DIRS})
set_target_properties (PulseJNI PROPERTIES FOLDER ${PROJECT_NAME})
include(PulseFiles.cmake)
# Still going to glob google protobuf java files
file(GLOB_RECURSE GOOGLE_JAVA_FILES "${DST_ROOT}/java/com/google/*.java")
add_jar(PulseJava ${PULSE_JAVA_FILES}
                  ${PULSE_BIND_JAVA_FILES}
                  ${GOOGLE_JAVA_FILES}
    INCLUDE_JARS 
      "jar/commons-collections4-4.4.jar"
      "jar/commons-compress-1.21.jar"
      "jar/error_prone_annotations-2.11.0.jar" # needed for protobufs
      "jar/gson-2.10.1.jar" # needed for protobufs
      "jar/guava-31.1-jre.jar" # needed for protobufs
      "jar/j2objc-annotations-1.3.jar" # needed for protobufs
      "jar/java-ordered-properties-1.0.4.jar"
      "jar/jfreechart-1.5.4.jar"
      "jar/jsr305-3.0.2.jar" # needed for protobufs
      "jar/poi-5.2.3.jar"
      "jar/poi-ooxml-5.2.3.jar"
      "jar/poi-ooxml-lite-5.2.3.jar"
      "jar/reflections-0.10.2.jar"
      "jar/slf4j-api-2.0.7.jar"
      "jar/xmlbeans-5.1.1.jar"
    OUTPUT_NAME Pulse
    MANIFEST ${CMAKE_CURRENT_SOURCE_DIR}/MANIFEST.MF)
get_target_property(_jarFile PulseJava JAR_FILE)
add_custom_command(TARGET PulseJava POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${_jarFile} ${CMAKE_INSTALL_PREFIX}/bin)
install_jar(PulseJava ${CMAKE_INSTALL_PREFIX}/bin)
set_target_properties (PulseJava PROPERTIES FOLDER ${PROJECT_NAME})
