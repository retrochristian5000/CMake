CMAKE_<LANG>_EXTENSIONS_DEFAULT
-------------------------------

.. versionadded:: 3.22

This indicates the compiler's default and mustn't be modified by the user.
Used as the default for the :prop_tgt:`<LANG>_EXTENSIONS` target property when
:variable:`CMAKE_<LANG>_EXTENSIONS` nor the paired language's is set (see
:policy:`CMP0128`).

To set the default extensions mode for targets use
:variable:`CMAKE_<LANG>_EXTENSIONS`.
