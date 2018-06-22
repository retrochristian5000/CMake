cmake_minimum_required(VERSION 2.6)
project(MathTest)


set (CMAKE_CXX_STANDARD 11)

# Expression test

set(expressions
        "5 * ( 3 + 4)"
        "(1 | 2 | 4 | 8) & 16"
        "1 +(3*4) + 10 >> 2"
        "10000 / 20 / 4"
        "10000 / (20 / 4)"
        "-1 + +1"
        "+1 - -1"
        "+1 - - + + -(-3 + - - +1)"
        "0"
        "0x12 * 0x12"
        "0x7FFFFFFFFFFFFFFF"
        "-12*5"
        "12*-5"
        "-12*-5"
        )

set(FILE_EXPRESSIONS "extern void test_expression(int64_t x, int64_t y, const char * text);\n")

macro(add_math_tests)
    foreach (expression ${expressions})
        math(EXPR result "${expression}" ${ARGV0} ${ARGV1})
        set(CODE "test_expression(${expression}, ${result}, \"${expression}\");")
        string(APPEND FILE_EXPRESSIONS "${CODE}\n")
    endforeach ()
endmacro()

add_math_tests()
add_math_tests("OUTPUT_FORMAT" "DECIMAL")
add_math_tests("OUTPUT_FORMAT" "HEXADECIMAL")


configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/MathTestTests.h.in"
        "${CMAKE_CURRENT_BINARY_DIR}/MathTestTests.h"
        @ONLY)

include_directories("${CMAKE_CURRENT_BINARY_DIR}")
add_executable(MathTest MathTestExec.cxx)

