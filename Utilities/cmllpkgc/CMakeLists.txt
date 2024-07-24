# Disable warnings to avoid changing 3rd party code.
if(CMAKE_CXX_COMPILER_ID MATCHES
  "^(GNU|LCC|Clang|AppleClang|IBMClang|XLClang|XL|VisualAge|SunPro|HP|Intel|IntelLLVM|NVHPC)$")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "PathScale")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -woffall")
endif()

add_library(cmllpkgc STATIC llpkgc.c llpkgc__internal.c)
