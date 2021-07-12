enable_language(C)

add_library(MissingLanguage STATIC IMPORTED)
set_target_properties(MissingLanguage PROPERTIES
  IMPORTED_LOCATION "${CMAKE_CURRENT_BINARY_DIR}/libMissingLanguage.a"
  IMPORTED_LINK_INTERFACE_LANGUAGES NotALang
  )

add_executable(main1 main.c)
target_link_libraries(main1 PRIVATE MissingLanguage)

add_executable(main2 main.c)
target_link_libraries(main2 PRIVATE MissingLanguage)
