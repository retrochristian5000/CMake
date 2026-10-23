# Editions of Visual Studio without profile guided optimization, e.g. the
# Build Tools, ignore '/GENPROFILE' with warning LNK4256.
if(CMAKE_C_COMPILER_ID STREQUAL "MSVC" AND actual_stdout MATCHES "LNK4256")
  file(WRITE "${RunCMake_TEST_BINARY_DIR}/pgo-not-available" "")
endif()
