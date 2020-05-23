project(unitybuild_anon_ns CXX)

set(srcs "")
foreach(s RANGE 1 8)
  set(src "${CMAKE_CURRENT_BINARY_DIR}/s${s}.cpp")
  file(WRITE "${src}" "
namespace MY_ANON_NAMESPACE {
    int i = ${s};
}
int func_like_macro_${s}() {
    return MY_ANON(i);
}
int using_macro_${s}() {
    MY_ANON_USING_NAMESPACE;
    return i;
}
")
  list(APPEND srcs "${src}")
endforeach()

add_library(tgt SHARED ${srcs})

set_target_properties(tgt PROPERTIES UNITY_BUILD ON)

set_target_properties(tgt PROPERTIES UNITY_BUILD_ANON_PREFIX MY_ANON)
