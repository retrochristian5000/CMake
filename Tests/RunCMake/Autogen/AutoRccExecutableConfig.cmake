enable_language(CXX)

find_package(Qt${with_qt_version} REQUIRED COMPONENTS Core Widgets Gui)

set(CMAKE_AUTORCC ON)

add_library(dummy STATIC empty.cpp)
target_link_libraries(dummy Qt${with_qt_version}::Core
                            Qt${with_qt_version}::Widgets
                            Qt${with_qt_version}::Gui)

add_executable(myrcc exe.cpp)
set_target_properties(dummy PROPERTIES AUTORCC_EXECUTABLE $<TARGET_FILE:myrcc>)
