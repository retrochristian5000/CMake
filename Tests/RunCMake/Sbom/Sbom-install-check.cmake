include(${CMAKE_CURRENT_LIST_DIR}/../InstallPackageInfo/Assertions.cmake)

file(READ "${RunCMake_BINARY_DIR}/install/sbomSbom-Release.spdx.json" content)

expect_value("${content}" "https://spdx.org/rdf/3.0.1/spdx-context.jsonld" "@context")
expect_value("${content}" "CMake" "@graph" 1 "name")
