CMAKE_MSVC_INCREMENTAL_LINK
---------------------------

.. versionadded:: 3.31

Select the incremental linking mode to be used by the MSVC linker.
This variable is used to initialize the :prop_tgt:`MSVC_INCREMENTAL_LINK`
property on all targets as they are created.  It is also propagated by
calls to the :command:`try_compile` command into the test project.

The allowed values are:

.. include:: ../prop_tgt/MSVC_INCREMENTAL_LINK-VALUES.txt

Use :manual:`generator expressions <cmake-generator-expressions(7)>` to
support per-configuration specification.  For example, the code:

.. code-block:: cmake

  set(CMAKE_MSVC_INCREMENTAL_LINK "$<IF:$<CONFIG:Debug>,Default,Never>")

specifies for all following targets to use incremental linking when possible
in the ``Debug`` configuration, to silently fall back to non-incremental
linking when not possible, and to disable incremental linking in other
configurations.

If this variable is not set then the :prop_tgt:`MSVC_INCREMENTAL_LINK` target
property will not be set automatically.  If that property is not set then
CMake uses the default value ``$<IF:$<CONFIG:Debug,RelWithDebInfo>,Force,Never>``
to select the incremental link mode.

.. note::

  This variable has effect only when policy :policy:`CMP0171` is set to ``NEW``
  prior to the first :command:`project` or :command:`enable_language` command
  that enables a language using a compiler targeting the MSVC ABI.
