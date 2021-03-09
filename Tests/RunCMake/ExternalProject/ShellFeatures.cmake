include(ExternalProject)

add_custom_target(other)

set(outFile ${CMAKE_CURRENT_BINARY_DIR}/cmdout.txt)
ExternalProject_Add(customCommands
  DOWNLOAD_COMMAND  "${CMAKE_COMMAND}" -E echo "download 1" > ${outFile}
           COMMAND  "${CMAKE_COMMAND}" -E echo "download 2" >> ${outFile}
  UPDATE_COMMAND    "${CMAKE_COMMAND}" -E echo "update 1" >> ${outFile}
         COMMAND    "${CMAKE_COMMAND}" -E true &&
                    "${CMAKE_COMMAND}" -E echo "update 2" >> ${outFile}
  PATCH_COMMAND     "${CMAKE_COMMAND}" -E echo "patch 1" >> ${outFile}
        COMMAND     "${CMAKE_COMMAND}" -E echo "patch 2" >> ${outFile}
  CONFIGURE_COMMAND "${CMAKE_COMMAND}" -E echo "configure 1" >> ${outFile}
            COMMAND "${CMAKE_COMMAND}" -E false ||
                    "${CMAKE_COMMAND}" -E echo "configure 2" >> ${outFile}
  BUILD_COMMAND     "${CMAKE_COMMAND}" -E echo "build 1" >> ${outFile}
        COMMAND     "${CMAKE_COMMAND}" -E echo "build genex: $<TARGET_PROPERTY:other,TYPE>" >> ${outFile}
  TEST_COMMAND      "${CMAKE_COMMAND}" -E echo "test 1" >> ${outFile}
       COMMAND      "${CMAKE_COMMAND}" -E echo "test 2" >> ${outFile}
  INSTALL_COMMAND   "${CMAKE_COMMAND}" -E echo "install 1" >> ${outFile}
          COMMAND   "${CMAKE_COMMAND}" -E echo "install 2" >> ${outFile}
)

# Force all steps to be re-run by removing timestamps from any previous run
ExternalProject_Get_Property(customCommands STAMP_DIR)
file(REMOVE_RECURSE "${STAMP_DIR}")
file(MAKE_DIRECTORY "${STAMP_DIR}")
