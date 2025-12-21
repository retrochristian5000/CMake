enable_language(CXX)

add_executable(generate_binary generate_binary.cpp)

include("${CMAKE_CURRENT_SOURCE_DIR}/hashes.cmake")

function(generate_binary name contents_name)
  add_custom_command(
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${name}.bin"
    COMMAND generate_binary "${contents_name}" "${CMAKE_CURRENT_BINARY_DIR}/${name}.bin"
    DEPENDS generate_binary
    )
  add_custom_target(
    verify_${name} ALL
    COMMAND "${CMAKE_COMMAND}"
      "-DFILENAME=${CMAKE_CURRENT_BINARY_DIR}/${name}.bin"
      "-DSHA256SUM=${${contents_name}_hash}"
      -P "${CMAKE_CURRENT_SOURCE_DIR}/verify_binary.cmake"
    DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/${name}.bin"
    )
endfunction()

foreach(name IN ITEMS basic st static c const static_const n name)
  generate_binary("${name}" basic)
endforeach()
foreach(name IN ITEMS empty text_lf text_crlf text_align very_long)
  generate_binary("${name}" "${name}")
endforeach()
