
cmake_minimum_required(VERSION 3.18)
project(ISPCTryCompile ISPC CXX)

#Verify we can use try_compile with ISPC
try_compile(result "${CMAKE_CURRENT_BINARY_DIR}"
        SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/simple.ispc"
        COPY_FILE "${CMAKE_CURRENT_BINARY_DIR}/result.o")

add_executable(ISPCTryCompile main.cxx )
target_link_libraries(ISPCTryCompile "${CMAKE_CURRENT_BINARY_DIR}/result.o")
