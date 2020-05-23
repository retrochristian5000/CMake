project(unitybuild_anon_no_ns_no_error CXX)

set(srcs "")
foreach(s RANGE 1 8)
  set(src "${CMAKE_CURRENT_BINARY_DIR}/s${s}.cpp")
  file(WRITE "${src}" "
// The unity.cxx file opens the namespace so that left over
// `using namespace` expanded from `MY_ANON_USING_NAMESPACE`
// are not an error in a unity build.
int leftover_using_macro_${s}() {
    MY_ANON_USING_NAMESPACE;
    return 42;
}
")
  list(APPEND srcs "${src}")
endforeach()

add_library(tgt SHARED ${srcs})

set_target_properties(tgt PROPERTIES UNITY_BUILD ON)

set_target_properties(tgt PROPERTIES UNITY_BUILD_ANON_PREFIX MY_ANON)
