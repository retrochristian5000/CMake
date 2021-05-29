enable_language(@lang@)
cmake_policy(SET CMP0127 OLD)
set(CMAKE_POLICY_WARNING_CMP0127 ON)

set(CMAKE_@lang@_EXTENSIONS @extensions_opposite@)
add_library(foo "@RunCMake_SOURCE_DIR@/empty.@ext@")
