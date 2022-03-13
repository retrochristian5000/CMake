cmake_minimum_required (VERSION 3.23)
project (TestMinimal)

find_package (JNI REQUIRED OPTIONAL_COMPONENTS AWT JVM)

add_library (minimal MODULE main.cxx)

target_link_libraries (minimal PRIVATE JNI::JNI)

if (JNI_AWT_FOUND)
  target_link_libraries (minimal PRIVATE JNI::AWT)
endif (JNI_AWT_FOUND)

if (JNI_JVM_FOUND)
  target_link_libraries (minimal PRIVATE JNI::JVM)
endif (JNI_JVM_FOUND)
