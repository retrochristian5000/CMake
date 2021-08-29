enable_language(@lang@)
set(CMAKE_@lang@_EXTENSIONS @extensions@)
add_library(foo "@RunCMake_SOURCE_DIR@/empty.@ext@")
