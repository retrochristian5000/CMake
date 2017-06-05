# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

if(GHS_TARGET_PLATFORM MATCHES "integrity")
  set(CMAKE_CXX_COMPILER_ID_TEST_FLAGS_FIRST "-bsp=${GHS_BSP_NAME} -os_dir=${GHS_OS_DIR} -c"
    CACHE INTERNAL "Compiler Flags to use during Compiler ID check")
endif()
