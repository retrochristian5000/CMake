include(FetchContent)

set(specialChars [==[!@#$%^& *()\\\"':[]{}|`~?<>,.]==])

set(outFile ${CMAKE_CURRENT_BINARY_DIR}/cmdout.txt)
FetchContent_Declare(customCommands
  DOWNLOAD_COMMAND  "${CMAKE_COMMAND}" -E echo "download 1" > ${outFile}
           COMMAND  "${CMAKE_COMMAND}" -E echo "download 2: ${specialChars}" >> ${outFile}
  UPDATE_COMMAND    "${CMAKE_COMMAND}" -E echo "update 1" >> ${outFile}
         COMMAND    "${CMAKE_COMMAND}" -E true &&
                    "${CMAKE_COMMAND}" -E echo "update 2" >> ${outFile}
  PATCH_COMMAND     "${CMAKE_COMMAND}" -E echo "patch 1" >> ${outFile}
        COMMAND     "${CMAKE_COMMAND}" -E false ||
                    "${CMAKE_COMMAND}" -E echo "patch 2" >> ${outFile}
)

# Force all steps to be re-run by removing timestamps, scripts, etc. from any
# previous run
file(REMOVE_RECURSE "${FETCHCONTENT_BASE_DIR}/customCommands-subbuild")

set(FETCHCONTENT_QUIET FALSE)
FetchContent_MakeAvailable(customCommands)
