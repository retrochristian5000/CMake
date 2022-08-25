MSVC_DEBUG_INFORMATION_FORMAT
-----------------------------

.. versionadded:: 3.25

Select debug information format targeting the MSVC ABI.

The allowed values are:

.. include:: MSVC_DEBUG_INFORMATION_FORMAT-VALUES.txt

Use :manual:`generator expressions <cmake-generator-expressions(7)>` to
support per-configuration specification.  For example, the code:

.. code-block:: cmake

  add_executable(foo foo.c)
  set_property(TARGET foo PROPERTY
    MSVC_DEBUG_INFORMATION_FORMAT "$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>")

selects for the target ``foo`` the program database debug information format
for the Debug configuration.

If this property is not set, CMake uses the default value
``$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>`` to select a debug information
format.

.. note::

  This property has effect only when policy :policy:`CMP0141` is set to ``NEW``
  prior to the first :command:`project` or :command:`enable_language` command
  that enables a language using a compiler targeting the MSVC ABI.
