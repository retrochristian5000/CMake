cmake_policy(SET CMP0205 OLD)
include(CMP0205-common.cmake)

if(allFilesDst)
  message(SEND_ERROR "Directory is not empty: '${allFilesDst}'")
endif()
