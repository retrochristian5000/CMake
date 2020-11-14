include(Platform/Linux-Intel)
__linux_compiler_intel(CXX)
set(CMAKE_INCLUDE_SYSTEM_FLAG_CXX "-isystem ")

if((NOT DEFINED CMAKE_DEPENDS_USE_COMPILER OR CMAKE_DEPENDS_USE_COMPILER)
    AND CMAKE_GENERATOR MATCHES "Makefiles"
    AND CMAKE_DEPFILE_FLAGS_CXX)
  # dependencies are computed by the compiler itself
  string(PREPEND CMAKE_DEPFILE_FLAGS_CXX "-MP ")
  set(CMAKE_CXX_DEPFILE_FORMAT make)
  set(CMAKE_CXX_DEPENDS_USE_COMPILER TRUE)
endif()
