file(READ ${RunCMake_TEST_BINARY_DIR}/cmake_install.cmake install_script)
# message(STATUS ${install_script})

# exe
set(regex [[NEW_RPATH "\\\${ORIGIN}/../lib"]])
if(NOT install_script MATCHES "${regex}")
  string(CONCAT RunCMake_TEST_FAILED
    "cmake_install.cmake does not contain the correctly generated RPATH entries. "
    "It does not match ${regex}")
  return()
endif()

# exe2
set(regex [[NEW_RPATH "/opt/foobar/lib"]])
if(NOT install_script MATCHES "${regex}")
  string(CONCAT RunCMake_TEST_FAILED
    "cmake_install.cmake does not contain the correctly generated RPATH entries. "
    "It does not match ${regex}")
  return()
endif()

# exe3
set(regex [[NEW_RPATH "\\?\$ORIGIN/../lib"]])
if(NOT install_script MATCHES "${regex}")
  string(CONCAT RunCMake_TEST_FAILED
    "cmake_install.cmake does not contain the correctly generated RPATH entries. "
    "It does not match ${regex}")
  return()
endif()

# exe4
set(regex [[NEW_RPATH "\\\${ORIGIN}/../foolib"]])
if(NOT install_script MATCHES "${regex}")
  string(CONCAT RunCMake_TEST_FAILED
    "cmake_install.cmake does not contain the correctly generated RPATH entries. "
    "It does not match ${regex}")
  return()
endif()
set(regex [[OLD_RPATH "\\?\$ORIGIN:::]])
if(NOT install_script MATCHES "${regex}")
  string(CONCAT RunCMake_TEST_FAILED
    "cmake_install.cmake does not contain the correctly generated RPATH entries. "
    "It does not match ${regex}")
  return()
endif()
