cmake_policy(SET CMP0157 NEW)
cmake_policy(SET CMP0195 NEW)

enable_language(Swift CXX)

## Verify generated header is visible internally but not visible beyond

add_library(L1 interop-lib.swift interop-use.cpp)
target_sources(L1 PRIVATE GENERATE_INTEROP_HEADER Swift L1/include/swift/answer.h BASE_DIR L1/include)

add_library(C1 interop-no-use.cpp)
target_link_libraries(C1 PRIVATE L1)

## Verify generated header is visible to dependencies but not visible internally

add_library(L2 interop-lib.swift interop-no-use.cpp)
target_sources(L2 INTERFACE GENERATE_INTEROP_HEADER Swift L2/swift/answer.h BASE_DIR L2)

add_library(C2 interop-use.cpp)
target_link_libraries(C2 PRIVATE L2)

## Verify generated header is visible internally and to public dependencies

add_library(L3 interop-lib.swift interop-use.cpp)
target_sources(L3 PUBLIC GENERATE_INTEROP_HEADER Swift L3/swift/answer.h BASE_DIR L3)

add_library(C3 interop-use.cpp)
target_link_libraries(C3 PUBLIC L3)

add_library(C4 interop-use.cpp)
target_link_libraries(C4 PRIVATE C3)

add_library(C5 interop-no-use.cpp)
target_link_libraries(C5 PUBLIC C4)
