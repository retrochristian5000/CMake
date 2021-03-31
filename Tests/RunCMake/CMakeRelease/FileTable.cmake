foreach(query
    ".files[].name"
    ".files[] | select(.os[] | . == \"source\") | .name"
    ".files[] | select((.os[] | . == \"macOS\") and (.class == \"volume\")) | .name"
    ".files[] | select((.os[] | . == \"windows\") and (.architecture[] | . == \"i386\") and (.class == \"installer\")) | .name"
    ".files[] | select(.architecture[] | . == \"x86_64\") | .name"
    ".files[] | select([.macOSmin] | inside([\"10.10\", \"10.11\", \"10.12\"])) | .name"
    ".hashFiles[] | select(.algorithm[] | . == \"SHA-256\") | .name"
    )
  message(STATUS "query: ${query}")
  execute_process(
    COMMAND ${JQ} "${query}" files-v1.json.in
    WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/../../../Utilities/Release"
    )
endforeach()
