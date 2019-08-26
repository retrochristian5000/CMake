PCH_SOURCE_<LANG>
-----------------

This property is implemented only when ``<LANG>`` is ``C`` or ``CXX``.

Read-only location of the generated precompile source file used
by :command:`target_precompile_headers`.

This property is initialized at generate time, and it will be
accessible only with a generator expression
`$<TARGET_PROPERTY:tgt,PCH_SOURCE_C|XX>`.

.. code-block:: cmake

  add_library(tgt ...)

  target_precompile_headers(tgt ...)

  add_custom_target(printPchSource
    COMMAND ${CMAKE_COMMAND} -E echo $<TARGET_PROPERTY:tgt,PCH_SOURCE_C>
  )
