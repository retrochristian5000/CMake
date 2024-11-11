
set(cfg_dir)
get_property(_isMultiConfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(_isMultiConfig)
  set(cfg_dir /Debug)
endif()
set(DUMP_EXE "${CMAKE_CURRENT_BINARY_DIR}${cfg_dir}/dump${CMAKE_EXECUTABLE_SUFFIX}")

add_executable(dump dump.c)

# ensure no temp file nor response file will be used
set(CMAKE_C_USE_RESPONSE_FILE_FOR_LIBRARIES 0)
string(REPLACE "${CMAKE_START_TEMP_FILE}" "" CMAKE_C_CREATE_SHARED_LIBRARY "${CMAKE_C_CREATE_SHARED_LIBRARY}")
string(REPLACE "${CMAKE_END_TEMP_FILE}" "" CMAKE_C_CREATE_SHARED_LIBRARY "${CMAKE_C_CREATE_SHARED_LIBRARY}")

add_executable(main main.c)
if (NOT DEFINED CMAKE_LINK_WARNING_AS_ERROR)
  set_target_properties(main PROPERTIES LINK_WARNING_AS_ERROR ${link_warning_as_error})
endif()
# use LAUNCH facility to dump linker command
set_property(TARGET main PROPERTY RULE_LAUNCH_LINK "\"${DUMP_EXE}\"")

add_dependencies(main dump)


# generate reference for WARNING_AS_ERROR flag
string(REPLACE "LINKER:" "" linker_WarnError "${CMAKE_C_LINK_OPTIONS_WARNING_AS_ERROR}")
if (CMAKE_C_LINKER_WRAPPER_FLAG)
  set(linker_flag ${CMAKE_C_LINKER_WRAPPER_FLAG})
  list(GET linker_flag -1 linker_space)
  if (linker_space STREQUAL " ")
    list(REMOVE_AT linker_flag -1)
  else()
    set(linker_space)
  endif()
  if (CMAKE_C_LINKER_WRAPPER_FLAG_SEP)
    string(REPLACE "," "${CMAKE_C_LINKER_WRAPPER_FLAG_SEP}" linker_WarnError "${linker_WarnError}")
    set(linker_WarnError "${linker_flag}${linker_space}${linker_WarnError}")
  else()
    set(linker_prefix "${linker_flag}${linker_space}")
    string(REPLACE "," ";" linker_WarnError "${linker_WarnError}")
    list(TRANSFORM linker_WarnError PREPEND "${linker_prefix}")
    list(JOIN linker_WarnError " " linker_WarnError)
  endif()
else()
  string(REPLACE "," " " linker_WarnError "${linker_WarnError}")
endif()
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/WARNING_AS_ERROR.txt" "${linker_WarnError}")
