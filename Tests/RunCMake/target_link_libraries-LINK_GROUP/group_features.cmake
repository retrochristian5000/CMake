
enable_language(C)

# Feature RESCAN
add_library(static1-rescan STATIC func1.c func3.c)
add_library(static2-rescan STATIC func2.c)

add_executable(main-rescan main.c)
target_link_libraries(main-rescan PRIVATE "$<LINK_GROUP:RESCAN,static1-rescan,static2-rescan>")
