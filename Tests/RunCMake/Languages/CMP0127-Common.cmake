enable_language(C)

add_library(NeedsFortran STATIC IMPORTED)
set_target_properties(NeedsFortran PROPERTIES
  IMPORTED_LOCATION "${CMAKE_CURRENT_BINARY_DIR}/libNeedsFortran.a"
  IMPORTED_LINK_INTERFACE_LANGUAGES Fortran
  )

add_executable(main1 main.c)
target_link_libraries(main1 PRIVATE NeedsFortran)

add_executable(main2 main.c)
target_link_libraries(main2 PRIVATE NeedsFortran)
