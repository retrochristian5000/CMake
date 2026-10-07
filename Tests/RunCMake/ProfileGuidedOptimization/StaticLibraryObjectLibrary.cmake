enable_language(C)
# Only the object library is instrumented, but archiving its objects into
# the static library, e.g. with MSVC, and linking the executable using them
# needs the profile guided optimization options.
add_library(foo OBJECT foo.c)
set_property(TARGET foo PROPERTY PROFILE_GUIDED_OPTIMIZATION GENERATE)
if(CMAKE_GENERATOR MATCHES "Xcode")
  # Xcode does not seem to support targets without sources.
  set(dummy dummy.c)
endif()
add_library(bar STATIC ${dummy} $<TARGET_OBJECTS:foo>)
add_executable(main main.c)
target_link_libraries(main PRIVATE bar)

include(PGORuntime.cmake)
