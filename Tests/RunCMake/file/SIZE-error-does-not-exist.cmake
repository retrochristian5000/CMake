set(file "${CMAKE_CURRENT_BINAR_DIR}/a-file-that-does-not-exist")

file(SIZE "${file}" CALCULATED_SIZE)
