set(manifest ${CMAKE_ARGV3})
set(count ${CMAKE_ARGV4})
file(STRINGS ${manifest} lines)

list(LENGTH lines len)

if (NOT EXISTS ${manifest})
  message(FATAL_ERROR "Install manifest not generated")
endif()

if (NOT len EQUAL count)
  message(FATAL_ERROR "Install manifest missing content: ${len}/${count}")
endif()
