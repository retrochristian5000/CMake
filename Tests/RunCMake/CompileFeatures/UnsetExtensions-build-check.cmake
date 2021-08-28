set(flag "@flag@")

if(CMAKE_GENERATOR MATCHES "Visual Studio")
  string(REPLACE "-" "/" flag "${flag}")
elseif(CMAKE_GENERATOR STREQUAL "Xcode")
  string(REPLACE "=" "\\=" flag "${flag}")
endif()

string(REPLACE ";" " " flag "${flag}")
string(FIND "${actual_stdout}" "${flag}" flag)

if(flag EQUAL -1)
  set(RunCMake_TEST_FAILED "Compile flag \"${flag}\" for CMAKE_@lang@_EXTENSIONS=@extensions@ not found.")
endif()
