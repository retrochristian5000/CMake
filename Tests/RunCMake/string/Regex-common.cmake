function(assert_matchall regex input expected)
  string(REGEX MATCHALL "${regex}" actual "${input}")
  if(NOT actual STREQUAL expected)
    message(
"REGEX MATCHALL \"${regex}\"
Input: ${input}
Output: ${actual}
Expected: ${expected}
"
    )
  endif()
endfunction()

function(assert_replace regex replacement input expected)
  string(REGEX REPLACE "${regex}" "${replacement}" actual "${input}")
  if(NOT actual STREQUAL expected)
    message(
"REGEX REPLACE \"${regex}\"
Input: ${input}
Output: ${actual}
Expected: ${expected}
"
    )
  endif()
endfunction()
