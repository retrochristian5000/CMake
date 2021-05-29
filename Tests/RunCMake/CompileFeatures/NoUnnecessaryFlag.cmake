enable_language(@lang@)
cmake_policy(SET CMP0127 NEW)

set(CMAKE_@lang@_EXTENSIONS @extensions_default@)
set(CMAKE_@lang@_STANDARD @standard_default@)
add_library(foo "@RunCMake_SOURCE_DIR@/empty.@ext@")
