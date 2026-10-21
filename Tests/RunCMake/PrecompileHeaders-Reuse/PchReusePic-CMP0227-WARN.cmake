include(PchReusePic-common.cmake)

# The policy warning stays silent on compilers that produce the same compile
# line either way, e.g. MSVC, which has no position-independent code flags at
# all, or IBM Clang, whose PIE flavor of them is the PIC flavor.  Give the
# compiler flags that tell the flavors apart so that the warning is checked
# everywhere.  CMake resolves the flags while generating the build system, so
# setting them after creating the targets is enough, and this test configures
# the project without compiling it.
set(CMAKE_C_COMPILE_OPTIONS_PIC "-fPIC")
set(CMAKE_C_COMPILE_OPTIONS_PIE "-fPIE")
