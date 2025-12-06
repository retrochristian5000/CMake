set(OUTPUT_NAME "test.tar.zstd")

set(COMPRESSION_FLAGS cvf)
set(COMPRESSION_OPTIONS --format=pax --lzma --cmake-tar-compression-level=9)

set(DECOMPRESSION_FLAGS xvf)

include(${CMAKE_CURRENT_LIST_DIR}/roundtrip.cmake)

check_magic("5d00000004" LIMIT 5 HEX)
