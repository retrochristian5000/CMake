CMAKE_LINK_WHAT_YOU_USE_FLAG
----------------------------

.. versionadded:: 3.22

Linker flag to be used to configure linker so that all specified libraries on
the command line will be linked into the target.
This flag is currently only defined on ``ELF`` platforms with value
``-Wl,--no-as-needed``.

The language specific variable :variable:`CMAKE_<LANG>_LINK_WHAT_YOU_USE_FLAG`
will be used preferably if defined.

See also variable :variable:`CMAKE_LINK_WHAT_YOU_USE_CHECK`.
