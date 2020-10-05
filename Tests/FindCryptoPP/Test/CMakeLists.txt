cmake_minimum_required(VERSION 3.0)
project(FindCryptoPP CXX)
include(CTest)

find_package(CryptoPP REQUIRED)

add_executable(test_cryptopp_tgt main.cpp)
target_link_libraries(test_cryptopp_tgt CryptoPP::CryptoPP)
add_test(NAME test_cryptopp_tgt COMMAND test_cryptopp_tgt)

add_executable(test_cryptopp_var main.cpp)
target_include_directories(test_cryptopp_var PRIVATE ${CryptoPP_INCLUDE_DIRS})
target_link_libraries(test_cryptopp_var PRIVATE ${CryptoPP_LIBRARIES})
add_test(NAME test_cryptopp_var COMMAND test_cryptopp_var)
