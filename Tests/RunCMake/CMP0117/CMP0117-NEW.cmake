message(WARNING "First message")
cmake_policy(SET CMP0117 NEW)
message(WARNING "Second message")

foreach(i 1 2)
  message(WARNING "${i}")
  if(i)
  endforeach()
endif()
endif()

message(WARNING "Third message")
