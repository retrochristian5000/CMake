cmake_minimum_required(VERSION 3.21)
project(FortranExternalProduce C Fortran)

include(FortranCInterface)
FortranCInterface_VERIFY()

FortranCInterface_HEADER(FortranStatic.h
  MACRO_NAMESPACE "FC_"
  SYMBOL_NAMESPACE "F_"
  SYMBOLS FortranStatic
  )

add_library(FortranStaticObj STATIC FortranStatic.f)
add_library(FortranStatic STATIC FortranStatic.c "$<TARGET_OBJECTS:FortranStaticObj>")
target_include_directories(FortranStatic PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")

install(TARGETS FortranStatic DESTINATION lib EXPORT FortranStatic)
install(EXPORT FortranStatic DESTINATION lib/cmake/FortranStatic FILE FortranStaticConfig.cmake)
