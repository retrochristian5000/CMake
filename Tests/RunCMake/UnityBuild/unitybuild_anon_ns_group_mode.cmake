project(unitybuild_anon_ns_group_mode CXX)

set(srcs "")
foreach(s RANGE 1 5)
  set(src "${CMAKE_CURRENT_BINARY_DIR}/s${s}.cpp")
  file(WRITE "${src}" "
#ifndef CONFIG_H
#define CONFIG_H
#define MY_ANON_NAMESPACE MY_ANON_ID
#define MY_ANON(Name) MY_ANON_NAMESPACE::Name
#define MY_ANON_USING_NAMESPACE using namespace MY_ANON_NAMESPACE
#endif

namespace { namespace MY_ANON_NAMESPACE {
    int i = ${s};
}}
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

set_target_properties(tgt PROPERTIES UNITY_BUILD ON
                                     UNITY_BUILD_MODE GROUP)

set_target_properties(tgt PROPERTIES UNITY_BUILD_UNIQUE_ID MY_ANON_ID)

set_source_files_properties(s1.cpp s2.cpp s5.cpp
                            PROPERTIES UNITY_GROUP "a"
                            )
set_source_files_properties(s3.cpp s4.cpp
                            PROPERTIES UNITY_GROUP "b"
                            )
