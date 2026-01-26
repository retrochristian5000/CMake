cmake_policy(SET CMP0210 NEW)

set(CMAKE_C_LINK_FLAGS ${pre}BADFLAG_C${obj})
set(CMAKE_CXX_LINK_FLAGS ${pre}BADFLAG_CXX${obj})

add_library(LinkFlags_shared_C SHARED LinkFlagsLib.c)
add_library(LinkFlags_mod_C MODULE LinkFlagsLib.c)
add_executable(LinkFlags_exe_C LinkFlagsExe.c)

add_library(LinkFlags_shared_CXX SHARED LinkFlagsLib.cxx)
add_library(LinkFlags_mod_CXX MODULE LinkFlagsLib.cxx)
add_executable(LinkFlags_exe_CXX LinkFlagsExe.cxx)
