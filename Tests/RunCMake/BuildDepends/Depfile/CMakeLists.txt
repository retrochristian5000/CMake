add_custom_command(
  OUTPUT cc.c
  DEPFILE cc.c.d
  COMMAND ${CMAKE_COMMAND} -DOUTFILE=cc.c -DINFILE=ccdep.txt -DDEPFILE=cc.c.d -DSTAMPFILE=ccstamp.txt -P ${CMAKE_CURRENT_LIST_DIR}/../WriteDepfile.cmake
  )
add_custom_target(cc ALL DEPENDS cc.c)

add_custom_command(
  OUTPUT exe.c
  DEPFILE exe.c.d
  COMMAND ${CMAKE_COMMAND} -DOUTFILE=exe.c -DINFILE=exedep.txt -DDEPFILE=exe.c.d -DSTAMPFILE=exestamp.txt -P ${CMAKE_CURRENT_LIST_DIR}/../WriteDepfile.cmake
  )
add_executable(exe exe.c)

add_custom_command(
  OUTPUT lib.c
  DEPFILE lib.c.d
  COMMAND ${CMAKE_COMMAND} -DOUTFILE=lib.c -DINFILE=libdep.txt -DDEPFILE=lib.c.d -DSTAMPFILE=libstamp.txt -P ${CMAKE_CURRENT_LIST_DIR}/../WriteDepfile.cmake
  )
add_library(lib STATIC lib.c)
