add_executable(SubDirA SubDirA.c)

target_link_libraries(TopDir PUBLIC SubDirAImported)

add_library(SubDirAImported IMPORTED INTERFACE)
target_compile_definitions(SubDirAImported INTERFACE DEF_SubDirAImported)
