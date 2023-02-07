# TODO Replace parenthesis with angle brackets!
cmake_host_system_information(RESULT <variable>
                              QUERY WINDOWS_REGISTRY <key> [VALUE_NAMES|SUBKEYS|VALUE <name>]
                              [VIEW (64|32|64_32|32_64|HOST|TARGET|BOTH)]
                              [SEPARATOR <separator>]
                              [ERROR_VARIABLE <result>])

  if(CMAKE_GET_OS_RELEASE_FALLBACK_CONTENT MATCHES "Foobar distribution release ([0-9\.]+) .*")
  endif()
