project(build)
set(CMAKE_BUILD_TYPE Debug)
add_library(foo foo.cxx)
set_target_properties(foo PROPERTIES PKG_CONFIG_NAME foo)
add_library(car foo.cxx)
set_target_properties(car PROPERTIES PKG_CONFIG_NAME car-pkg)
add_library(bar bar.c)
add_library(dog foo.cxx)
target_link_libraries(foo PRIVATE car bar dog debug -lm)
add_library(foo2 foo.cxx)
target_link_options(foo2 INTERFACE -lm)
add_library(foo3 foo.cxx)
target_link_libraries(foo3 PRIVATE car bar dog)
export(TARGETS bar dog car foo foo2 ANDROID_MK
  ${build_BINARY_DIR}/Android.mk)
install(TARGETS bar dog car foo foo2 DESTINATION lib EXPORT myexp)
install(EXPORT_ANDROID_MK myexp DESTINATION share/ndk-modules)
install(TARGETS foo3 DESTINATION lib EXPORT mypkgcfgexport)
install(EXPORT mypkgcfgexport
  ANDROID_MK DESTINATION share/ndk-modules3
  PKG_CONFIG NAMESPACE mynm- DESTINATION lib/pkg-config
)
