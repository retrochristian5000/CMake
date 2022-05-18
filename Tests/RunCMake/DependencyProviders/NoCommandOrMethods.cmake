# Force the provider to be invoked
find_package(NoExist QUIET)
message(STATUS "NoExist_FOUND = ${NoExist_FOUND}")
