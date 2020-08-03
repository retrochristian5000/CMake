cmake_minimum_required(VERSION 3.15)
project(Downstream)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# find MathFunctions
find_package(MathFunctions 3.4 COMPONENTS Addition SquareRoot)

# create executable
add_executable(myexe main.cc)

# use MathFunctions library
target_link_libraries(myexe MathFunctions::Addition MathFunctions::SquareRoot)
