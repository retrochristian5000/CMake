project(MyThirdPartyDependency)
cmake_minimum_required(VERSION 3.8)

include(CTest)

add_executable(thirdparty thirdparty.cxx)
add_test(NAME thirdparty COMMAND thirdparty)

set_property(TARGET thirdparty PROPERTY LABELS NotASubproject)
set_property(TEST thirdparty PROPERTY LABELS NotASubproject)
