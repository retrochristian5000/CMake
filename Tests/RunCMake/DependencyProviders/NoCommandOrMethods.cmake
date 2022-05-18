# Force the provider to be invoked
find_package(SomethingThatDoesNotExist QUIET)
message(STATUS "SomethingThatDoesNotExist_FOUND = ${SomethingThatDoesNotExist_FOUND}")
