cmake_minimum_required(VERSION 3.15)
project(MyExe)

# Add executable
add_executable(myexe main.cxx)

# install executable
install(TARGETS myexe)
