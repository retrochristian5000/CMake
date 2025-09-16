cmake_minimum_required(VERSION 4.1)
project(${RunCMake_TEST} NONE)

enable_language("${RunCMake_TEST}")

if ("${RunCMake_TEST}" STREQUAL "C")
    set(extension "c")
elseif("${RunCMake_TEST}" STREQUAL "CXX")
    set(extension "cpp")
elseif("${RunCMake_TEST}" STREQUAL "CUDA")
    set(extension "cu")
else()
    message(FATAL_ERROR "Unknown test: ${RunCMake_TEST}")
endif()

add_executable(${RunCMake_TEST} checker.${extension})
target_compile_definitions(${RunCMake_TEST} PRIVATE "FUNCTION=main")

add_library(${RunCMake_TEST}_shared SHARED checker.${extension})
target_compile_definitions(${RunCMake_TEST}_shared PRIVATE "FUNCTION=test")

add_library(${RunCMake_TEST}_static STATIC checker.${extension})
target_compile_definitions(${RunCMake_TEST}_static PRIVATE "FUNCTION=test")

target_compile_definitions(${RunCMake_TEST}_shared PRIVATE "MUST_HAVE_DEFINE")
