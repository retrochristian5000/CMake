project(unitybuild_anon_ns CXX)

set(srcs "")
foreach(s RANGE 1 8)
  set(src "${CMAKE_CURRENT_BINARY_DIR}/s${s}.cpp")
  file(WRITE "${src}" "
#undef CAT
#undef CAT2
#define CAT2(ARG1, ARG2) ARG1 ## ARG2
#define CAT(ARG1, ARG2) CAT2(ARG1, ARG2)
int CAT(unique_name_, MY_ANON_ID)() {
    return ${s};
}
")
  list(APPEND srcs "${src}")
endforeach()

add_library(tgt SHARED ${srcs})

set_target_properties(tgt PROPERTIES UNITY_BUILD ON)

set_target_properties(tgt PROPERTIES UNITY_BUILD_UNIQUE_ID MY_ANON_ID)
