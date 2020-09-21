set(subdir_message "This should not be printed because variable evaluation is in deferred scope.")
cmake_language(DEFER DIRECTORY .. CALL message STATUS "${subdir_message}")
cmake_language(DEFER DIRECTORY .. GET_CALL_IDS ids)
message(STATUS "Immediate Message From Subdirectory: ids='${ids}'")
