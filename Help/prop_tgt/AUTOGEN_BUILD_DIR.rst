AUTOGEN_BUILD_DIR
-----------------

Directory where :prop_tgt:`AUTOMOC`, :prop_tgt:`AUTOUIC` and :prop_tgt:`AUTORCC`
generate files for the target.

By default :prop_tgt:`AUTOGEN_BUILD_DIR` is unset.

When unset or empty the directory
:variable:`CMAKE_CURRENT_BINARY_DIR`/<TARGET_NAME>_autogen is used.

See the :manual:`cmake-qt(7)` manual for more information on using CMake
with Qt.
