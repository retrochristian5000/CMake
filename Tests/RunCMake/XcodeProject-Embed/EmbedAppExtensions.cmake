list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
include(ExternalAppExtension)

add_executable(app MACOSX_BUNDLE main.m)
add_dependencies(app ${EXTERNAL_APPEXT})
set_target_properties(app PROPERTIES XCODE_EMBED_APP_EXTENSIONS ${EXTERNAL_APPEXT})
