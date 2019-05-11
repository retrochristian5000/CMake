ADDITIONAL_NINJA_CLEAN_FILES
----------------------------

Additional files to remove during the Ninja clean stage.

A :ref:`;-list <CMake Language Lists>` of files that will be removed
as a part of the ``ninja clean`` target.

Relative paths are allowed and are interpreted relative to the
current binary directory.

Arguments to :prop_dir:`ADDITIONAL_NINJA_CLEAN_FILES` may use
:manual:`generator expressions <cmake-generator-expressions(7)>`.

This property only works for the :generator:`Ninja` generator.
It is ignored on other generators.
