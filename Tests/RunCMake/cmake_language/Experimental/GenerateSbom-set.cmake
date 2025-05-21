set(CMAKE_EXPERIMENTAL_GENERATE_SBOM
  "ca494ed3-b261-4205-a01f-603c95e4cae0")

cmake_language(GET_EXPERIMENTAL_FEATURE_ENABLED
  "GenerateSbom"
  feature_present)

if (NOT feature_present STREQUAL "TRUE")
  message(FATAL_ERROR
    "Expected the `GenerateSbom` feature to be enabled.")
endif ()
