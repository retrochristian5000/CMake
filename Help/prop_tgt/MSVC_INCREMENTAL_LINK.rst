MSVC_INCREMENTAL_LINK
---------------------

.. versionadded:: 3.31

Select the incremental linking mode to be used by the MSVC linker.

The allowed values are:

.. include:: MSVC_INCREMENTAL_LINK-VALUES.txt

Use :manual:`generator expressions <cmake-generator-expressions(7)>` to
support per-configuration specification.  For example, the code:

.. code-block:: cmake

  add_executable(foo foo.c)
  set_property(TARGET foo PROPERTY
    MSVC_INCREMENTAL_LINK "$<IF:$<CONFIG:Debug>,Default,Never>")

specifies for target ``foo`` to use incremental linking when possible
in the ``Debug`` configuration, to silently fall back to non-incremental
linking when not possible, and to disable incremental linking in other
configurations.

The property is initialized from the value of the
:variable:`CMAKE_MSVC_INCREMENTAL_LINK` variable, if it is set.
If the property is not set, then CMake uses the default value
``$<IF:$<CONFIG:Debug,RelWithDebInfo>,Force,Never>`` to select incremental
linking flags.

.. note::

  This property has effect only when policy :policy:`CMP0171` is set to ``NEW``
  prior to the first :command:`project` or :command:`enable_language` command
  that enables a language using a compiler targeting the MSVC ABI.
