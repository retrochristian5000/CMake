cmake_minimum_required(VERSION 3.15)
project(objctest LANGUAGES CXX OBJCXX)

add_compile_options(-fobjc-arc)

add_library(myfuncs STATIC myfuncs.mm)
