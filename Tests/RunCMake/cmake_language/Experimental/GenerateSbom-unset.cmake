cmake_language(GET_EXPERIMENTAL_FEATURE_ENABLED
  "GenerateSbom"
  feature_present)

if (NOT feature_present STREQUAL "FALSE")
  message(FATAL_ERROR
    "Expected the `GenerateSbom` feature to be disabled.")
endif ()
