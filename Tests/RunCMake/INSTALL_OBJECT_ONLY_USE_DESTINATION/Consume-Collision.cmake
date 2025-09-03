include("${CMAKE_CURRENT_LIST_DIR}/Consume.cmake")
# The collision overwrites the expected object with a `return 1;`, so the test
# will fail.
set_property(TEST run PROPERTY WILL_FAIL 1)
