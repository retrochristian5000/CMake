set(CMAKE_INTERMEDIATE_DIR_STRATEGY FULL CACHE STRING "" FORCE)

enable_language(C)
enable_testing()

include(CheckCCompilerFlag)
include(CheckCSourceCompiles)

if(CMAKE_C_COMPILE_OPTIONS_USE_PCH)
  add_definitions(-DHAVE_PCH_SUPPORT)
endif()

# Compilers that reject a mismatching precompiled header outright, e.g. Clang,
# fail the build on their own.  Compilers that only warn, e.g. GCC through
# "-Winvalid-pch", silently compile the headers again instead, which would
# hide a regression here, so promote that warning to an error.
check_c_compiler_flag(-Werror=invalid-pch HAVE_WERROR_INVALID_PCH)
if(HAVE_WERROR_INVALID_PCH)
  add_compile_options(-Werror=invalid-pch)
endif()

# Telling the PIE flavor of the position independent code flags from the PIC
# flavor at compile time needs __PIE__, which not every compiler defines,
# e.g. Apple Clang does not.  Note that __PIC__ alone cannot tell them apart,
# because both flavors define it.
string(REPLACE ";" " " maybe_pie_flags "${CMAKE_C_COMPILE_OPTIONS_PIE}")
set(CMAKE_REQUIRED_FLAGS "${maybe_pie_flags}")
check_c_source_compiles([[
#ifndef __PIE__
#  error "__PIE__ is not defined"
#endif
int main(void) { return 0; }
]] HAVE_PIE_MACRO)
unset(CMAKE_REQUIRED_FLAGS)

file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/skipped.c" [[
#if defined(EXPECT_PIC) && !defined(__PIC__)
#  error "position independent code flags are missing"
#endif

#if defined(EXPECT_PIE) && !defined(__PIE__)
#  error "position independent executable flags are missing"
#endif

int main(void)
{
  return 0;
}
]])

# The precompiled header is created while compiling a library, which uses
# the PIC flavor of the position independent code flags.
add_library(empty empty.c)
set_property(TARGET empty PROPERTY POSITION_INDEPENDENT_CODE ON)
target_precompile_headers(empty PRIVATE
  <stdio.h>
  <string.h>
)
target_include_directories(empty PUBLIC include)

add_library(foo foo.c)
set_property(TARGET foo PROPERTY POSITION_INDEPENDENT_CODE ON)
target_include_directories(foo PUBLIC include)
target_precompile_headers(foo REUSE_FROM empty)

# Executables would use the PIE flavor of the position independent code
# flags, which does not match the precompiled header.
add_executable(foobar_pie foobar.c)
set_property(TARGET foobar_pie PROPERTY POSITION_INDEPENDENT_CODE ON)
target_link_libraries(foobar_pie PRIVATE foo)
target_precompile_headers(foobar_pie REUSE_FROM empty)
add_test(NAME foobar_pie COMMAND foobar_pie)

# This target would use no position independent code flags at all,
# which does not match the precompiled header either.
add_executable(foobar_nopie foobar.c)
set_property(TARGET foobar_nopie PROPERTY POSITION_INDEPENDENT_CODE OFF)
target_link_libraries(foobar_nopie PRIVATE foo)
target_precompile_headers(foobar_nopie REUSE_FROM empty)
add_test(NAME foobar_nopie COMMAND foobar_nopie)

# The precompiled header below is created without position independent code
# flags, while one of the targets reusing it needs them.  The flags must be
# enabled for the whole group so that no target loses position independence.
add_library(empty_nopic empty.c)
set_property(TARGET empty_nopic PROPERTY POSITION_INDEPENDENT_CODE OFF)
target_precompile_headers(empty_nopic PRIVATE
  <stdio.h>
  <string.h>
)
target_include_directories(empty_nopic PUBLIC include)

add_library(foo_shared SHARED foo.c)
set_property(TARGET foo_shared PROPERTY WINDOWS_EXPORT_ALL_SYMBOLS ON)
target_include_directories(foo_shared PUBLIC include)
target_precompile_headers(foo_shared REUSE_FROM empty_nopic)

add_executable(foobar_nopic foobar.c)
set_property(TARGET foobar_nopic PROPERTY POSITION_INDEPENDENT_CODE OFF)
target_link_libraries(foobar_nopic PRIVATE foo_shared)
target_precompile_headers(foobar_nopic REUSE_FROM empty_nopic)
add_test(NAME foobar_nopic COMMAND foobar_nopic)

# Every target of the group below is an executable, so the group must keep
# the PIE flavor of the position independent code flags instead of falling
# back to the PIC flavor that shared libraries need.  The last target reuses
# the precompiled header through another target that reuses it, to cover the
# resolution of such a chain.
add_executable(foobar_pie_pch foobar.c)
set_property(TARGET foobar_pie_pch PROPERTY POSITION_INDEPENDENT_CODE ON)
target_link_libraries(foobar_pie_pch PRIVATE foo)
target_precompile_headers(foobar_pie_pch PRIVATE
  <stdio.h>
  <string.h>
)
add_test(NAME foobar_pie_pch COMMAND foobar_pie_pch)

add_executable(foobar_pie_reuse foobar.c)
set_property(TARGET foobar_pie_reuse PROPERTY POSITION_INDEPENDENT_CODE ON)
target_link_libraries(foobar_pie_reuse PRIVATE foo)
target_precompile_headers(foobar_pie_reuse REUSE_FROM foobar_pie_pch)
add_test(NAME foobar_pie_reuse COMMAND foobar_pie_reuse)

add_executable(foobar_pie_chain foobar.c)
set_property(TARGET foobar_pie_chain PROPERTY POSITION_INDEPENDENT_CODE ON)
target_link_libraries(foobar_pie_chain PRIVATE foo)
target_precompile_headers(foobar_pie_chain REUSE_FROM foobar_pie_reuse)
add_test(NAME foobar_pie_chain COMMAND foobar_pie_chain)

# No target shares this precompiled header, so this executable must keep the
# PIE flavor of the flags that it would use without one.
add_executable(foobar_own_pch foobar.c)
set_property(TARGET foobar_own_pch PROPERTY POSITION_INDEPENDENT_CODE ON)
target_link_libraries(foobar_own_pch PRIVATE foo)
target_precompile_headers(foobar_own_pch PRIVATE
  <stdio.h>
  <string.h>
)
add_test(NAME foobar_own_pch COMMAND foobar_own_pch)

# The executable below names the library in "PRECOMPILE_HEADERS_REUSE_FROM"
# but skips the precompiled header on all of its sources, so it never
# compiles against it.  Neither target may take the flags of the other.
add_library(empty_skip empty.c)
set_property(TARGET empty_skip PROPERTY POSITION_INDEPENDENT_CODE OFF)
target_precompile_headers(empty_skip PRIVATE
  <stdio.h>
  <string.h>
)

add_executable(foobar_skip "${CMAKE_CURRENT_BINARY_DIR}/skipped.c")
set_property(TARGET foobar_skip PROPERTY POSITION_INDEPENDENT_CODE ON)
set_source_files_properties("${CMAKE_CURRENT_BINARY_DIR}/skipped.c"
  PROPERTIES SKIP_PRECOMPILE_HEADERS ON)
target_precompile_headers(foobar_skip REUSE_FROM empty_skip)
add_test(NAME foobar_skip COMMAND foobar_skip)

# The flags expected for each target.  Verify them in two ways, because
# neither way works everywhere: the predefined macros are checked while
# compiling the sources, but toolchains that are position independent by
# default define them no matter which flags are used, and the build check
# script scans the compile lines, which only some generators print.
set(pic_targets empty foo foobar_pie foobar_nopie
                empty_nopic foo_shared foobar_nopic)
set(pie_targets foobar_pie_pch foobar_pie_reuse foobar_pie_chain
                foobar_own_pch foobar_skip)
set(nopic_targets empty_skip)

# Compilers define the standard __PIC__ macro for the "-fPIC" flag.  Note
# that targets of a group have to agree on their compile definitions too,
# otherwise the precompiled header does not match.
if(CMAKE_C_COMPILE_OPTIONS_PIC STREQUAL "-fPIC")
  foreach(target IN LISTS pic_targets)
    target_compile_definitions(${target} PRIVATE EXPECT_PIC)
  endforeach()
endif()

if(HAVE_PIE_MACRO)
  foreach(target IN LISTS pie_targets)
    target_compile_definitions(${target} PRIVATE EXPECT_PIE)
  endforeach()
endif()

string(REPLACE ";" " " expect_pic_flags "${CMAKE_C_COMPILE_OPTIONS_PIC}")
string(REPLACE ";" " " expect_pie_flags "${CMAKE_C_COMPILE_OPTIONS_PIE}")
if(expect_pie_flags STREQUAL "")
  # CMake uses the PIC flavor when the compiler has no PIE flavor.
  set(expect_pie_flags "${expect_pic_flags}")
endif()

file(WRITE "${CMAKE_BINARY_DIR}/expected-flags.cmake"
"set(expect_pic_flags \"${expect_pic_flags}\")
set(expect_pie_flags \"${expect_pie_flags}\")
set(expect_pic_targets \"${pic_targets}\")
set(expect_pie_targets \"${pie_targets}\")
set(expect_nopic_targets \"${nopic_targets}\")
")
