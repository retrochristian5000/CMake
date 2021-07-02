CMAKE_LINK_WHAT_YOU_USE_CHECK
-----------------------------

.. versionadded:: 3.22

command executed after link step to check libraries usage.
This check is currently only defined on ``ELF`` platforms with value
``ldd -u -r``.

See also variable :variable:`CMAKE_LINK_WHAT_YOU_USE_FLAG`.
