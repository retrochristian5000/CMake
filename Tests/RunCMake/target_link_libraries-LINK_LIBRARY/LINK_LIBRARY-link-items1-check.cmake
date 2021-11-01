
if (NOT actual_stdout MATCHES "[/-]-PREFIXGROUP +.*${CMAKE_SHARED_LIBRARY_PREFIX}base1${CMAKE_SHARED_LIBRARY_SUFFIX} +${CMAKE_LINK_LIBRARY_FLAG}other +[/-]-SUFFIXGROUP")
  set (RunCMake_TEST_FAILED "Not found expected '--PREFIXGROUP <base1> <other> --SUFFIXGROUP'.")
endif()
