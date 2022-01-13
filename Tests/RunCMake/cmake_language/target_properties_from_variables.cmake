enable_language(C)

function(assert_prop_eq tgt name value)
  get_property(actual_value TARGET ${tgt} PROPERTY ${name})
  if(NOT actual_value STREQUAL value)
    message(SEND_ERROR "Expected value of ${name}:\n  ${value}\nActual value:\n  ${actual_value}")
  endif()
endfunction()

function(assert_prop_undef tgt name)
  get_property(actual_value TARGET ${tgt} PROPERTY ${name})
  if(DEFINED actual_value)
    message(SEND_ERROR "Expected ${name} to be undefined, actual value:\n  ${actual_value}")
  endif()
endfunction()

set(Test_PROP1 "Hello")
set(Test_PROP2 "world")
set(Test_PROP3 "!")
cmake_language(TARGET_PROPERTIES_FROM_VARIABLES
  PREFIX Test_
  PROPERTIES PROP1
  )

add_subdirectory(target_properties_from_variables_directory)

add_executable(top_exe main.c)
assert_prop_eq(top_exe PROP1 "Hello")
assert_prop_undef(top_exe PROP2)
assert_prop_undef(top_exe PROP3)
