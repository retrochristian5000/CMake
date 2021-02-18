# We need a file whose timestamp we can compare against and where we know the
# timestamp is earlier than anything that FetchContent might set
set(cmpFile ${CMAKE_CURRENT_BINARY_DIR}/cmpTimeStamp.txt)
file(TOUCH ${cmpFile})

include(${CMAKE_CURRENT_LIST_DIR}/TimeStamps.cmake)
