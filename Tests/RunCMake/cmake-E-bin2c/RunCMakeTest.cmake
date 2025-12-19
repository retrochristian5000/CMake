include(RunCMake)

function(run_bin2c name contents_name)
  string(REPLACE ";" "\\;" ARGS "${ARGN}")
  run_cmake_command(${name}
    "${CMAKE_COMMAND}"
    "-DNAME=${name}"
    "-DINPUT_FILE=${RunCMake_TEST_BINARY_DIR}/${contents_name}.bin"
    "-DARGS=${ARGS}"
    -P "${RunCMake_SOURCE_DIR}/run_bin2c.cmake"
    )
endfunction()

function(run_bin2c_basic name)
  run_bin2c("${name}" basic ${ARGN})
endfunction()

set(RunCMake_TEST_BINARY_DIR "${RunCMake_BINARY_DIR}/generate_binaries-build")
run_cmake(generate_binaries)
set(RunCMake_TEST_NO_CLEAN 1)
run_cmake_command(generate_binaries-build "${CMAKE_COMMAND}" --build .)
run_bin2c(basic basic)
run_bin2c(empty empty)
run_bin2c(text_lf text_lf)
run_bin2c(text_crlf text_crlf)
run_bin2c(text_align text_align)
run_bin2c(long long)
run_bin2c_basic(array --array-name basic_contents)
run_bin2c(array_empty empty --array-name empty_contents)
run_bin2c_basic(static --array-name basic_contents --static)
run_bin2c_basic(const --array-name basic_contents --const)
run_bin2c_basic(static_const --array-name basic_contents --static --const)
run_bin2c_basic(namespace --array-name basic_contents --namespace ns)
run_bin2c_basic(namespace_nested --array-name basic_contents --namespace ns::nested)
run_bin2c_basic(namespace_anonymous --namespace "" --array-name basic_contents)
run_bin2c_basic(namespace_anonymous_no_arg --namespace --array-name basic_contents)
run_bin2c_basic(extern_c --array-name basic_contents --extern-c)
run_bin2c_basic(macro --array-name basic_contents --macro "MY_MACRO(true)")
run_bin2c_basic(array_type --array-name basic_contents --array-type std::uint8_t)
run_bin2c_basic(size --array-name basic_contents --size-name basic_size)
run_bin2c_basic(trailing_comma --trailing-comma)
run_bin2c_basic(trailing_comma_array --array-name basic_contents --trailing-comma)
run_bin2c(trailing_comma_empty empty --trailing-comma)
run_bin2c(trailing_comma_array_empty empty --array-name empty_contents --trailing-comma)
run_bin2c(trailing_comma_text_align text_align --trailing-comma)
run_bin2c_basic(include --array-name basic_contents --include "<brackets.h>" --include "\"quotes.h\"" --include "noquotes.h")
run_bin2c_basic(class_member --array-name "basic_class<true>::basic_contents" --size-name "basic_class<true>::basic_size")
run_bin2c_basic(everything
  --array-name basic_contents
  --size-name basic_size
  --namespace ns::nested
  --static
  --const
  --extern-c
  --macro "MY_MACRO(true)"
  --array-type std::uint8_t
  --include "<brackets.h>"
  --include "\"quotes.h\""
  --include "noquotes.h"
  --trailing-comma
  )
run_bin2c_basic(everything_array_override
  --array-name basic_contents
  --size-name basic_size
  --namespace ns::nested
  --macro GLOBAL_MACRO
  --array-namespace ns::array
  --array-static
  --array-const
  --array-extern-c
  --array-macro ARRAY_MACRO
  )
run_bin2c_basic(everything_array_only
  --array-name basic_contents
  --namespace ns::nested
  --macro GLOBAL_MACRO
  --array-namespace ns::array
  --array-static
  --array-const
  --array-extern-c
  --array-macro ARRAY_MACRO
  )
run_bin2c_basic(everything_size_override
  --array-name basic_contents
  --size-name basic_size
  --namespace ns::nested
  --macro GLOBAL_MACRO
  --size-namespace ns::size
  --size-static
  --size-const
  --size-extern-c
  --size-macro SIZE_MACRO
  )
run_bin2c_basic(no_auto_gen_comment --no-auto-gen-comment)
run_bin2c_basic(no_auto_gen_comment_array --array-name basic_contents --no-auto-gen-comment)
run_bin2c_basic(no_auto_gen_comment_namespace --array-name basic_contents --namespace ns --no-auto-gen-comment)
run_bin2c_basic(no_auto_gen_comment_extern_c --array-name basic_contents --extern-c --no-auto-gen-comment)
run_bin2c_basic(no_auto_gen_comment_include --array-name basic_contents --include header.h --no-auto-gen-comment)
run_bin2c(no_auto_gen_comment_empty empty --no-auto-gen-comment)

run_cmake_command(too_many_files ${CMAKE_COMMAND} -E bin2c a a a)
run_cmake_command(input_noexist ${CMAKE_COMMAND} -E bin2c noexist.bin)
run_cmake_command(size_no_array ${CMAKE_COMMAND} -E bin2c --size-name size)
run_cmake_command(include_no_array ${CMAKE_COMMAND} -E bin2c --include header.h)
run_cmake_command(static_no_array ${CMAKE_COMMAND} -E bin2c --static)
run_cmake_command(const_no_array ${CMAKE_COMMAND} -E bin2c --const)
run_cmake_command(namespace_no_array ${CMAKE_COMMAND} -E bin2c --namespace ns)
run_cmake_command(extern_c_no_array ${CMAKE_COMMAND} -E bin2c --extern-c)
run_cmake_command(macro_no_array ${CMAKE_COMMAND} -E bin2c --macro MY_MACRO)
run_cmake_command(array_static_no_array ${CMAKE_COMMAND} -E bin2c --array-static)
run_cmake_command(array_const_no_array ${CMAKE_COMMAND} -E bin2c --array-const)
run_cmake_command(array_namespace_no_array ${CMAKE_COMMAND} -E bin2c --array-namespace ns)
run_cmake_command(array_extern_c_no_array ${CMAKE_COMMAND} -E bin2c --array-extern-c)
run_cmake_command(array_macro_no_array ${CMAKE_COMMAND} -E bin2c --array-macro MY_MACRO)
run_cmake_command(array_type_no_array ${CMAKE_COMMAND} -E bin2c --array-type std::uint8_t)
run_cmake_command(size_static_no_size ${CMAKE_COMMAND} -E bin2c --array-name arr --size-static)
run_cmake_command(size_const_no_size ${CMAKE_COMMAND} -E bin2c --array-name arr --size-const)
run_cmake_command(size_namespace_no_size ${CMAKE_COMMAND} -E bin2c --array-name arr --size-namespace ns)
run_cmake_command(size_extern_c_no_size ${CMAKE_COMMAND} -E bin2c --array-name arr --size-extern-c)
run_cmake_command(size_macro_no_size ${CMAKE_COMMAND} -E bin2c --array-name arr --size-macro MY_MACRO)
run_cmake_command(array_namespace_invalid ${CMAKE_COMMAND} -E bin2c --array-name arr --array-namespace 0)
run_cmake_command(array_static_class_member ${CMAKE_COMMAND} -E bin2c --array-name cls::arr --array-static)
run_cmake_command(array_extern_c_class_member ${CMAKE_COMMAND} -E bin2c --array-name cls::arr --array-extern-c)
run_cmake_command(size_namespace_invalid ${CMAKE_COMMAND} -E bin2c --array-name arr --size-name size --size-namespace 0)
run_cmake_command(size_static_class_member ${CMAKE_COMMAND} -E bin2c --array-name arr --size-name cls::size --size-static)
run_cmake_command(size_extern_c_class_member ${CMAKE_COMMAND} -E bin2c --array-name arr --size-name cls::size --size-extern-c)

if(CMake_TEST_BIN2C_LARGE_FILE)
  run_cmake_command(generate_binaries-verify_very_long_hash
    "${CMAKE_COMMAND}"
    --build .
    --target verify_very_long_hash
    )
  run_cmake_command(very_long
    "${CMAKE_COMMAND}"
    -P "${RunCMake_SOURCE_DIR}/very_long.cmake"
    )
endif()
