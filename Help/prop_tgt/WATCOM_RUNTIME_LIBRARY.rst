WATCOM_RUNTIME_LIBRARY
----------------------

.. versionadded:: 3.24

Select the Watcom runtime library for use by compilers targeting the Watcom ABI.

The allowed values are:

.. include:: WATCOM_RUNTIME_LIBRARY-VALUES.txt

Use :manual:`generator expressions <cmake-generator-expressions(7)>` to
support per-configuration specification.  For example, the code:

.. code-block:: cmake

  add_executable(foo foo.c)
  set_property(TARGET foo PROPERTY
    WATCOM_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

selects for the target ``foo`` a multi-threaded statically-linked runtime
library with or without debug information depending on the configuration.

If this property is not set then CMake uses the default value
``MultiThreaded$<$<CONFIG:Debug>:Debug>DLL`` on Windows and
``SingleThreaded$<$<CONFIG:Debug>:Debug>`` on other platforms to select a Watcom
runtime library.

.. note::

  This property has effect only when policy :policy:`CMP0134` is set to ``NEW``
  prior to the first :command:`project` or :command:`enable_language` command
  that enables a language using a compiler targeting the Watcom ABI.
