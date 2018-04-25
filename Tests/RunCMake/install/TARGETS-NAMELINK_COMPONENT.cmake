enable_language(C)

macro(add_versioned_library NAME)
  add_library(${NAME} SHARED empty.c)
  set_target_properties(${NAME} PROPERTIES
    VERSION 1.0
    SOVERSION 1
  )
endmacro()

add_versioned_library(namelink-sep)
add_versioned_library(namelink-same)
add_versioned_library(namelink-uns)
add_versioned_library(namelink-uns-dev)
add_versioned_library(namelink-only)
add_versioned_library(namelink-skip)
add_library(namelink-none SHARED empty.c)

install(TARGETS namelink-sep namelink-none
  LIBRARY
    DESTINATION lib
    COMPONENT lib
    NAMELINK_COMPONENT dev
)
install(TARGETS namelink-same
  LIBRARY
    DESTINATION lib
    COMPONENT lib
)
install(TARGETS namelink-uns
  LIBRARY
    DESTINATION lib
)
install(TARGETS namelink-uns-dev
  LIBRARY
    DESTINATION lib
    NAMELINK_COMPONENT dev
)
install(TARGETS namelink-only
  LIBRARY
    DESTINATION lib
    COMPONENT lib
    NAMELINK_COMPONENT dev
    NAMELINK_ONLY
)
install(TARGETS namelink-skip
  LIBRARY
    DESTINATION lib
    COMPONENT lib
    NAMELINK_COMPONENT dev
    NAMELINK_SKIP
)
