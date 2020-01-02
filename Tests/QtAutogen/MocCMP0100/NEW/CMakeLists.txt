cmake_minimum_required(VERSION 3.10)
cmake_policy(SET CMP0100 NEW)

add_executable(mocCMP0100New
  ${CSD}/main.cpp
  ${CSD}/Obj.hh ${CSD}/Obj.cpp # Include Obj.hh
  ${CSD}/Obj2.cpp              # Don't include Obj2.hh!
)
target_link_libraries(mocCMP0100New ${QT_LIBRARIES})
