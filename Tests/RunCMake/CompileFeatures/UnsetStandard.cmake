enable_language(@lang@)
cmake_policy(SET CMP0127 NEW)

set(CMAKE_@lang@_EXTENSIONS @extensions_opposite@)
add_library(foo "@RunCMake_SOURCE_DIR@/empty.@ext@")
