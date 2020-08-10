macro(prepare_test CASE_NAME)

  if (${CASE_NAME} STREQUAL NameIsAlphaNumeric1)
    set(CASE_CMAKELISTS_NAME_1 [==[ abcdefghijklmnopqrstuvwxyz0123456789 ]==])
    set(CASE_CMAKELISTS_NAME_2 [==[ abcdefghijklmnopqrstuvwxyz0123456789 ]==])
  endif()

  if (${CASE_NAME} STREQUAL NameIsAlphaNumeric2)
    set(CASE_CMAKELISTS_NAME_1 [==["abcdefghijklmnopqrstuvwxyz0123456789"]==])
    set(CASE_CMAKELISTS_NAME_2 [==["abcdefghijklmnopqrstuvwxyz0123456789"]==])
  endif()

  if (${CASE_NAME} STREQUAL NameIsAlphaNumeric3)
    set(CASE_CMAKELISTS_NAME_1 [==["abcdefghijklmnopqrstuvwxyz0123456789"]==])
    set(CASE_CMAKELISTS_NAME_2 [==[ abcdefghijklmnopqrstuvwxyz0123456789 ]==])
  endif()

  if (${CASE_NAME} STREQUAL NameIsAlphaNumeric4)
    set(CASE_CMAKELISTS_NAME_1 [==[ abcdefghijklmnopqrstuvwxyz0123456789 ]==])
    set(CASE_CMAKELISTS_NAME_2 [==["abcdefghijklmnopqrstuvwxyz0123456789"]==])
  endif()


  if (${CASE_NAME} STREQUAL NameContainsValidSpecialChars1)
    set(CASE_CMAKELISTS_NAME_1 [==[ abc_.+-012 ]==])
    set(CASE_CMAKELISTS_NAME_2 [==[ abc_.+-012 ]==])
  endif()

  if (${CASE_NAME} STREQUAL NameContainsValidSpecialChars2)
    set(CASE_CMAKELISTS_NAME_1 [==["abc_.+-012"]==])
    set(CASE_CMAKELISTS_NAME_2 [==["abc_.+-012"]==])
  endif()

  if (${CASE_NAME} STREQUAL NameContainsValidSpecialChars3)
    set(CASE_CMAKELISTS_NAME_1 [==["abc_.+-012"]==])
    set(CASE_CMAKELISTS_NAME_2 [==[ abc_.+-012 ]==])
  endif()

  if (${CASE_NAME} STREQUAL NameContainsValidSpecialChars4)
    set(CASE_CMAKELISTS_NAME_1 [==[ abc_.+-012 ]==])
    set(CASE_CMAKELISTS_NAME_2 [==["abc_.+-012"]==])
  endif()


  if (${CASE_NAME} STREQUAL NameContainsFormerInvalidSpecialChars1)
    set(CASE_CMAKELISTS_NAME_1 [==[ $[] #;\t\n\"\\ ]==])
    set(CASE_CMAKELISTS_NAME_2 [==[ $[] #;\t\n\"\\ ]==])
  endif()

  if (${CASE_NAME} STREQUAL NameContainsFormerInvalidSpecialChars2)
    set(CASE_CMAKELISTS_NAME_1 [==["$[] #;\t\n\"\\"]==])
    set(CASE_CMAKELISTS_NAME_2 [==["$[] #;\t\n\"\\"]==])
  endif()

  if (${CASE_NAME} STREQUAL NameContainsFormerInvalidSpecialChars3)
    set(CASE_CMAKELISTS_NAME_1 [==["$[] #;\t\n\"\\"]==])
    set(CASE_CMAKELISTS_NAME_2 [==[ $[] #;\t\n\"\\ ]==])
  endif()

  if (${CASE_NAME} STREQUAL NameContainsFormerInvalidSpecialChars4)
    set(CASE_CMAKELISTS_NAME_1 [==[ $[] #;\t\n\"\\ ]==])
    set(CASE_CMAKELISTS_NAME_2 [==["$[] #;\t\n\"\\"]==])
  endif()


  if (${CASE_NAME} STREQUAL NameContainsOtherSpecialChars1)
    set(CASE_CMAKELISTS_NAME_1 [==[ !§%&/ü:*😤~ ]==])
    set(CASE_CMAKELISTS_NAME_2 [==[ !§%&/ü:*😤~ ]==])
  endif()

  if (${CASE_NAME} STREQUAL NameContainsOtherSpecialChars2)
    set(CASE_CMAKELISTS_NAME_1 [==["!§%&/ü:*😤~"]==])
    set(CASE_CMAKELISTS_NAME_2 [==["!§%&/ü:*😤~"]==])
  endif()

  if (${CASE_NAME} STREQUAL NameContainsOtherSpecialChars3)
    set(CASE_CMAKELISTS_NAME_1 [==["!§%&/ü:*😤~"]==])
    set(CASE_CMAKELISTS_NAME_2 [==[ !§%&/ü:*😤~ ]==])
  endif()

  if (${CASE_NAME} STREQUAL NameContainsOtherSpecialChars4)
    set(CASE_CMAKELISTS_NAME_1 [==[ !§%&/ü:*😤~ ]==])
    set(CASE_CMAKELISTS_NAME_2 [==["!§%&/ü:*😤~"]==])
  endif()


  if (${CASE_NAME} STREQUAL NameContainsEscapedSpecialChars1)
    set(CASE_CMAKELISTS_NAME_1 [==[ \(\)\ \# ]==])
    set(CASE_CMAKELISTS_NAME_2 [==[ \(\)\ \# ]==])
  endif()

  if (${CASE_NAME} STREQUAL NameContainsEscapedSpecialChars2)
    set(CASE_CMAKELISTS_NAME_1 [==["\(\)\ \#"]==])
    set(CASE_CMAKELISTS_NAME_2 [==["\(\)\ \#"]==])
  endif()

  if (${CASE_NAME} STREQUAL NameContainsEscapedSpecialChars3)
    set(CASE_CMAKELISTS_NAME_1 [==["\(\)\ \#"]==])
    set(CASE_CMAKELISTS_NAME_2 [==[ \(\)\ \# ]==])
  endif()

  if (${CASE_NAME} STREQUAL NameContainsEscapedSpecialChars4)
    set(CASE_CMAKELISTS_NAME_1 [==[ \(\)\ \# ]==])
    set(CASE_CMAKELISTS_NAME_2 [==["\(\)\ \#"]==])
  endif()


  if (${CASE_NAME} STREQUAL NameContainsEscapedLeadingAndTrailingWhitespace1)
    set(CASE_CMAKELISTS_NAME_1 [==[ \ SurroundedByWhitespace\  ]==])
    set(CASE_CMAKELISTS_NAME_2 [==[ \ SurroundedByWhitespace\  ]==])
  endif()

  if (${CASE_NAME} STREQUAL NameContainsEscapedLeadingAndTrailingWhitespace2)
    set(CASE_CMAKELISTS_NAME_1 [==["\ SurroundedByWhitespace\ "]==])
    set(CASE_CMAKELISTS_NAME_2 [==["\ SurroundedByWhitespace\ "]==])
  endif()

  if (${CASE_NAME} STREQUAL NameContainsEscapedLeadingAndTrailingWhitespace3)
    set(CASE_CMAKELISTS_NAME_1 [==["\ SurroundedByWhitespace\ "]==])
    set(CASE_CMAKELISTS_NAME_2 [==[ \ SurroundedByWhitespace\  ]==])
  endif()

  if (${CASE_NAME} STREQUAL NameContainsEscapedLeadingAndTrailingWhitespace4)
    set(CASE_CMAKELISTS_NAME_1 [==[ \ SurroundedByWhitespace\  ]==])
    set(CASE_CMAKELISTS_NAME_2 [==["\ SurroundedByWhitespace\ "]==])
  endif()


  if (${CASE_NAME} STREQUAL NameContainsGeneratorExpressionSyntax1)
    set(CASE_CMAKELISTS_NAME_1 [==[ $<BOOL:0> ]==])
    set(CASE_CMAKELISTS_NAME_2 [==[ $<BOOL:0> ]==])
  endif()

  if (${CASE_NAME} STREQUAL NameContainsGeneratorExpressionSyntax2)
    set(CASE_CMAKELISTS_NAME_1 [==["$<BOOL:0>"]==])
    set(CASE_CMAKELISTS_NAME_2 [==["$<BOOL:0>"]==])
  endif()

  if (${CASE_NAME} STREQUAL NameContainsGeneratorExpressionSyntax3)
    set(CASE_CMAKELISTS_NAME_1 [==["$<BOOL:0>"]==])
    set(CASE_CMAKELISTS_NAME_2 [==[ $<BOOL:0> ]==])
  endif()

  if (${CASE_NAME} STREQUAL NameContainsGeneratorExpressionSyntax4)
    set(CASE_CMAKELISTS_NAME_1 [==[ $<BOOL:0> ]==])
    set(CASE_CMAKELISTS_NAME_2 [==["$<BOOL:0>"]==])
  endif()


  if (${CASE_NAME} STREQUAL NameContainsSpaces1)
    set(CASE_CMAKELISTS_NAME_1 [==["PrefixTest SuffixTest"]==])
    set(CASE_CMAKELISTS_NAME_2 [==["PrefixTest SuffixTest"]==])
  endif()

  if (${CASE_NAME} STREQUAL NameContainsSpaces2)
    set(CASE_CMAKELISTS_NAME_1 [==[ PrefixTest SuffixTest ]==])
    set(CASE_CMAKELISTS_NAME_2 [==[ PrefixTest SuffixTest ]==])
  endif()

  if (${CASE_NAME} STREQUAL NameContainsSpaces3)
    set(CASE_CMAKELISTS_NAME_1 [==["PrefixTest SuffixTest"]==])
    set(CASE_CMAKELISTS_NAME_2 [==[ PrefixTest SuffixTest ]==])
  endif()

  if (${CASE_NAME} STREQUAL NameContainsSpaces4)
    set(CASE_CMAKELISTS_NAME_1 [==[ PrefixTest SuffixTest ]==])
    set(CASE_CMAKELISTS_NAME_2 [==["PrefixTest SuffixTest"]==])
  endif()


  if (${CASE_NAME} STREQUAL NameContainsSemicolon1)
    set(CASE_CMAKELISTS_NAME_1 [==["PrefixTest;SuffixTest"]==])
    set(CASE_CMAKELISTS_NAME_2 [==["PrefixTest;SuffixTest"]==])
  endif()

  if (${CASE_NAME} STREQUAL NameContainsSemicolon2)
    set(CASE_CMAKELISTS_NAME_1 [==[ PrefixTest;SuffixTest ]==])
    set(CASE_CMAKELISTS_NAME_2 [==[ PrefixTest;SuffixTest ]==])
  endif()

  if (${CASE_NAME} STREQUAL NameContainsSemicolon3)
    set(CASE_CMAKELISTS_NAME_1 [==["PrefixTest;SuffixTest"]==])
    set(CASE_CMAKELISTS_NAME_2 [==[ PrefixTest;SuffixTest ]==])
  endif()

  if (${CASE_NAME} STREQUAL NameContainsSemicolon4)
    set(CASE_CMAKELISTS_NAME_1 [==[ PrefixTest;SuffixTest ]==])
    set(CASE_CMAKELISTS_NAME_2 [==["PrefixTest;SuffixTest"]==])
  endif()


  if (${CASE_NAME} STREQUAL NameContainsEscapedQuote1)
    set(CASE_CMAKELISTS_NAME_1 [==["EscapedQuote\"\"SuffixTest"]==])
    set(CASE_CMAKELISTS_NAME_2 [==["EscapedQuote\"\"SuffixTest"]==])
  endif()

  if (${CASE_NAME} STREQUAL NameContainsEscapedQuote2)
    set(CASE_CMAKELISTS_NAME_1 [==[ EscapedQuote\"\"SuffixTest ]==])
    set(CASE_CMAKELISTS_NAME_2 [==[ EscapedQuote\"\"SuffixTest ]==])
  endif()

  if (${CASE_NAME} STREQUAL NameContainsEscapedQuote3)
    set(CASE_CMAKELISTS_NAME_1 [==["EscapedQuote\"\"SuffixTest"]==])
    set(CASE_CMAKELISTS_NAME_2 [==[ EscapedQuote\"\"SuffixTest ]==])
  endif()

  if (${CASE_NAME} STREQUAL NameContainsEscapedQuote4)
    set(CASE_CMAKELISTS_NAME_1 [==[ EscapedQuote\"\"SuffixTest ]==])
    set(CASE_CMAKELISTS_NAME_2 [==["EscapedQuote\"\"SuffixTest"]==])
  endif()


  if (${CASE_NAME} STREQUAL NameContainsBracketArgument1)
    set(CASE_CMAKELISTS_NAME_1 [==["[=[BracketArgument;SuffixTest]=]"]==])
    set(CASE_CMAKELISTS_NAME_2 [==["[=[BracketArgument;SuffixTest]=]"]==])
  endif()

  if (${CASE_NAME} STREQUAL NameContainsBracketArgument2)
    set(CASE_CMAKELISTS_NAME_1 [==[ [=[BracketArgument;SuffixTest]=] ]==])
    set(CASE_CMAKELISTS_NAME_2 [==[ [=[BracketArgument;SuffixTest]=] ]==])
  endif()

  if (${CASE_NAME} STREQUAL NameContainsBracketArgument3)
    set(CASE_CMAKELISTS_NAME_1 [==["[=[BracketArgument;SuffixTest]=]"]==])
    set(CASE_CMAKELISTS_NAME_2 [==[ [=[BracketArgument;SuffixTest]=] ]==])
  endif()

  if (${CASE_NAME} STREQUAL NameContainsBracketArgument4)
    set(CASE_CMAKELISTS_NAME_1 [==[ [=[BracketArgument;SuffixTest]=] ]==])
    set(CASE_CMAKELISTS_NAME_2 [==["[=[BracketArgument;SuffixTest]=]"]==])
  endif()

endmacro()
