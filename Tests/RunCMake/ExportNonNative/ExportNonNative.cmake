project(build
  VERSION 1.2.3
  DESCRIPTION "sample description"
  HOMEPAGE_URL "http://sample.org"
)
set(CMAKE_BUILD_TYPE Debug)
add_library(foo foo.cxx)
add_library(car foo.cxx)
add_library(bar bar.c)
add_library(dog foo.cxx)
target_link_libraries(foo PRIVATE car bar dog debug -lm)
add_library(foo2 foo.cxx)
target_link_options(foo2 INTERFACE -lm)

#PkgConfig exports

# let's pretend these dependencies have pkg-config packages
# and end up in Requires: property
set_target_properties(foo PROPERTIES PKG_CONFIG_NAME foo)
set_target_properties(car PROPERTIES PKG_CONFIG_NAME car-pkg)

# macro to set common test target properties
macro(setup_pkg_conf_test_target tgt)
  target_link_libraries(${tgt} PRIVATE foo bar dog PUBLIC car)
  target_compile_features(${tgt} PUBLIC cxx_std_20)
  target_link_libraries(${tgt} PUBLIC foo2)
  if (UNIX)
    # adding a couple of dummy include dirs.
    # the first one is implicit and has to be removed
    # by the export generator
    # Notice their order which has to be kept
    target_include_directories(${tgt} PUBLIC
      /usr/include /opt/include2 /opt/include1)
  endif()
  target_compile_features(${tgt} PUBLIC cxx_std_20)
endmacro()

# make targets to export as pkg-config .pc file
add_library(foo3Static STATIC foo.cxx)
setup_pkg_conf_test_target(foo3Static)
add_library(foo3Shared SHARED foo.cxx)
setup_pkg_conf_test_target(foo3Shared)
set_target_properties(foo3Static PROPERTIES INTERFACE_POSITION_INDEPENDENT_CODE ON)
set_target_properties(foo3Shared PROPERTIES VERSION "3.4.5")


export(TARGETS bar dog car foo foo2 ANDROID_MK
  ${build_BINARY_DIR}/Android.mk)
install(TARGETS bar dog car foo foo2 DESTINATION lib EXPORT myexp)
install(EXPORT_ANDROID_MK myexp DESTINATION share/ndk-modules)

install(TARGETS foo3Static DESTINATION lib EXPORT myStatic)
install(EXPORT myStatic
  ANDROID_MK DESTINATION share/ndk-modules3
  PKG_CONFIG NAMESPACE mynm- DESTINATION lib/pkg-config
)

install(TARGETS foo3Shared DESTINATION lib EXPORT myShared)
install(EXPORT myShared
  PKG_CONFIG NAMESPACE mynm- DESTINATION lib/pkg-config
)
