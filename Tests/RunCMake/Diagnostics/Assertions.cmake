function(expect CATEGORY EXPECTED)
  cmake_diagnostic(GET ${CATEGORY} actual)
  if(NOT "${actual}" STREQUAL "${EXPECTED}")
    message(SEND_ERROR
      "wrong action for diagnostic ${CATEGORY}"
      " (expected '${EXPECTED}', actual '${actual}')")
  endif()
endfunction()
