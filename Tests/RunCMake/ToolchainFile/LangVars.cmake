foreach(test_language C CXX)
  enable_language(${test_language})
  if(CMAKE_${test_language}_COMPILER_ID MATCHES "GNU|Clang|MSVC|Intel"
      AND NOT CMAKE_${test_language}_COMPILE_FEATURES)
    message(FATAL_ERROR "Compile features not found for ${test_language}")
  endif()
endforeach()
