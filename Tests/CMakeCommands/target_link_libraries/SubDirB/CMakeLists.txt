add_executable(SubDirB SubDirB.c)

target_link_libraries(SubDirA PRIVATE SubDirBImported)

add_library(SubDirBImported IMPORTED INTERFACE)
target_compile_definitions(SubDirBImported INTERFACE DEF_SubDirBImported)
