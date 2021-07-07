cmake_minimum_required(VERSION 3.21)
project(FindPackageForceREquiredTest)

# Protect tests from running inside the default install prefix.
set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/NotDefaultPrefix")

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})


############################################################################
## Test find_package CMAKE_REQUIRE_FIND_PACKAGE_ with unknown package
## The find_package call should fail

set(CMAKE_REQUIRE_FIND_PACKAGE_DoesNotExist ON)
find_package(DoesNotExist MODULE)

