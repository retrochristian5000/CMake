cmake_minimum_required(VERSION 3.24...3.28)
project(cxx_modules_library NONE)

find_package(export_interface_wrapper REQUIRED)

if (NOT TARGET CXXModules::wrapper OR
    NOT TARGET CXXModules::export_wrapped_modules)
  message(FATAL_ERROR
    "Missing imported target")
endif ()
