enable_language(@lang@)
set(CMAKE_@lang@_EXTENSIONS @extensions_default@)
set(CMAKE_@lang@_STANDARD @standard_default@)
add_library(foo @RunCMake_SOURCE_DIR@/empty.@ext@)
