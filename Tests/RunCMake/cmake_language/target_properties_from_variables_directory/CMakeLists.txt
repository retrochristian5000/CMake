cmake_language(TARGET_PROPERTIES_FROM_VARIABLES
  PREFIX Test_
  PROPERTIES PROP2 PROP3
  )

add_executable(sub_exe ../main.c)
assert_prop_eq(sub_exe PROP1 "Hello")
assert_prop_eq(sub_exe PROP2 "world")
assert_prop_eq(sub_exe PROP3 "!")
