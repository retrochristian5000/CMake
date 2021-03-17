include(${CMAKE_CURRENT_LIST_DIR}/../macros.cmake)

ASSERT_FILE_CONTENT_MATCHES(
  ${CMAKE_CURRENT_BINARY_DIR}/Executable.uvprojx
  "<vShortEn>1</vShortEn>"          # -fshort-enums flag must be inherited from the linked library
  "<Define>USE_LIBRARY</Define>"    # USE_LIBRARY interface definition of the linked library must be set
)
