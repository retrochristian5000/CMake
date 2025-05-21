cmake_path(GET CMAKE_BINARY_DIR PARENT_PATH out_dir)
set(CMAKE_PREFIX_PATH "${out_dir}/install")
set(CMAKE_INSTALL_PREFIX "${out_dir}/install")

project(sbom LANGUAGES C)

add_library(sbom SHARED
    foo.c
)

install(
    TARGETS sbom
    EXPORT SbomTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(
    EXPORT SbomTargets
    FILE SbomTargets.cmake
    NAMESPACE Sbom::
    DESTINATION lib/cmake/Sbom
)
install(SBOM FORMAT SPDX)
