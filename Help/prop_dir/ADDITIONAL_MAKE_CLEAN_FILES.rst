ADDITIONAL_MAKE_CLEAN_FILES
---------------------------

Additional files to remove during the make clean stage.

A :ref:`;-list <CMake Language Lists>` of files that will be removed
as a part of the ``make clean`` target.

Arguments to :prop_dir:`ADDITIONAL_MAKE_CLEAN_FILES` may use
:manual:`generator expressions <cmake-generator-expressions(7)>`.

This property only works for Makefile generators.
It is ignored on other generators.
