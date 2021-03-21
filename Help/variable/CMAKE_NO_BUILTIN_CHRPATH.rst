CMAKE_NO_BUILTIN_CHRPATH
------------------------

Do not use the builtin ELF/XCOFF editor to fix RPATHs on installation.

When an ELF or XCOFF binary needs to have a different RPATH after installation
than it does in the build tree, CMake uses a builtin editor to change
the RPATH in the installed copy.  If this variable is set to true then
CMake will relink the binary before installation instead of using its
builtin editor.

.. versionadded:: 3.20

  This variable only applied to the ELF binary format prior to CMake 3.20.
