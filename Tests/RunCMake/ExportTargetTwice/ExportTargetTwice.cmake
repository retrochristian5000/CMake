cmake_minimum_required(VERSION 3.19)
project(cmake-install-include-dir-repro LANGUAGES C)

add_library(foo foo.c)

install(
    EXPORT      pkg1
    FILE        pkg1Config.cmake
    DESTINATION pkg1
    NAMESPACE   pkg1::
)
install(
    TARGETS foo EXPORT pkg1
    ARCHIVE DESTINATION pkg1/lib
    INCLUDES DESTINATION pkg1/inc
)


install(
    EXPORT      pkg2
    FILE        pkg2Config.cmake
    DESTINATION pkg2
    NAMESPACE   pkg2::
)
install(
    TARGETS foo EXPORT pkg2
    ARCHIVE DESTINATION pkg2/lib
    INCLUDES DESTINATION pkg2/inc
)
