
set(languages C ${LANG})
list(REMOVE_DUPLICATES languages)

enable_language(${languages})

include(CTest)

add_executable(COMPILER_LINKER compiler_linker.c)
target_compile_definitions(COMPILER_LINKER PRIVATE "VAR=${CMAKE_${LANG}_COMPILER_LINKER_${TYPE}}"
                                                   "GENEX=$<${LANG}_COMPILER_LINKER_${TYPE}>")

add_test(NAME COMPILER_LINKER.${LANG}.${TYPE}
         COMMAND COMPILER_LINKER)
