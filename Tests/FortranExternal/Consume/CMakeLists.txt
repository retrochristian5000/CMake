cmake_minimum_required(VERSION 3.21)
cmake_policy(SET CMP0127 NEW)
project(FortranExternalConsume C)
find_package(FortranStatic REQUIRED)

add_executable(FortranExternal FortranExternal.c)
target_link_libraries(FortranExternal PRIVATE FortranStatic)
