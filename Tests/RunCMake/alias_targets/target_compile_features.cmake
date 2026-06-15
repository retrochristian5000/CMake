
enable_language(CXX)

add_library(foo empty.cpp)

add_library(alias ALIAS foo)

target_compile_features(alias PRIVATE cxx_std_98)
get_target_property(val foo COMPILE_FEATURES)
if(NOT "cxx_std_98" IN_LIST val)
  message(SEND_ERROR "target_compile_features(): Target property 'COMPILE_FEATURES' set on ALIAS 'alias' has wrong value on aliased target 'foo': '${val}' instead of 'cxx_std_98'.")
endif()
