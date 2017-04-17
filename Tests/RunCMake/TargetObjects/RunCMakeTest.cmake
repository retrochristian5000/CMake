include(RunCMake)

if(RunCMake_GENERATOR STREQUAL "Xcode" AND "$ENV{CMAKE_OSX_ARCHITECTURES}" MATCHES "[;$]")
  run_cmake(BadContext)
else()
  run_cmake(NoTarget)
  run_cmake(NotObjlibTarget)
endif()
