cmake_minimum_required(VERSION 3.28)
project(scantest CXX)

# CMP0155 is NEW, so a C++20 target is scanned for module dependencies when the
# toolchain provides a scanner.
add_library(scantest STATIC scantest.cpp)
target_compile_features(scantest PRIVATE cxx_std_20)
