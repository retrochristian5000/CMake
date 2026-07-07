enable_language(CXX)

add_executable(main empty.cpp)
add_executable(Alias::Main ALIAS main)
target_compile_features(Alias::Main PRIVATE cxx_std_98)

get_target_property(val main COMPILE_FEATURES)
if(NOT "cxx_std_98" IN_LIST val)
  message(SEND_ERROR "target_compile_features(): Target property 'COMPILE_FEATURES' is missing value added to ALIAS 'Alias::Main' on aliased target 'main': 'cxx_std_98' not in '${val}'.")
endif()
