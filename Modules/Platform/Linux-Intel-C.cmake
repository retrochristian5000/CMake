include(Platform/Linux-Intel)
__linux_compiler_intel(C)
set(CMAKE_INCLUDE_SYSTEM_FLAG_C "-isystem ")

if((NOT DEFINED CMAKE_DEPENDS_USE_COMPILER OR CMAKE_DEPENDS_USE_COMPILER)
    AND CMAKE_GENERATOR MATCHES "Makefiles"
    AND CMAKE_DEPFILE_FLAGS_C)
  # dependencies are computed by the compiler itself
  string(PREPEND CMAKE_DEPFILE_FLAGS_C "-MP ")
  set(CMAKE_C_DEPFILE_FORMAT make)
  set(CMAKE_C_DEPENDS_USE_COMPILER TRUE)
endif()
