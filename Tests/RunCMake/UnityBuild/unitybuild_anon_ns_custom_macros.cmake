project(unitybuild_anon_ns_custom_macros CXX)

set(srcs "")
foreach(s RANGE 1 8)
  set(src "${CMAKE_CURRENT_BINARY_DIR}/s${s}.cpp")
  file(WRITE "${src}" "

namespace MY_ANON_NS {
    int i = ${s};
}
int func_like_macro_${s}() {
    return MY_LOCAL(i);
}
int using_macro_${s}() {
    MY_LOCAL_USING_NS;
    return i;
}
")
  list(APPEND srcs "${src}")
endforeach()

add_library(tgt SHARED ${srcs})

set_property(TARGET tgt PROPERTY UNITY_BUILD ON)

set_property(TARGET tgt PROPERTY UNITY_BUILD_ANON_PREFIX MY_ANON)

set_property(TARGET tgt PROPERTY
    UNITY_BUILD_CODE_BEFORE_INCLUDE "
#ifndef MY_LOCAL
#define MY_LOCAL(ARG) MY_ANON(ARG)
#endif
#ifndef MY_ANON_NS
#define MY_ANON_NS MY_ANON_NAMESPACE
#endif
#ifndef MY_LOCAL_USING_NS
#define MY_LOCAL_USING_NS MY_ANON_USING_NAMESPACE
#endif
"
)
