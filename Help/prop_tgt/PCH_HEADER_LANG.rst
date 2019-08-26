PCH_HEADER_<LANG>
-----------------

This property is implemented only when ``<LANG>`` is ``C`` or ``CXX``.

Read-only location of the generated precompile header file used
by :command:`target_precompile_headers`.

This property is initialized at generate time, and it will be
accessible only with a generator expression
`$<TARGET_PROPERTY:tgt,PCH_HEADER_C|XX>`.

.. code-block:: cmake

  add_library(tgt ...)

  target_precompile_headers(tgt ...)

  add_custom_target(printPchHeader
    COMMAND ${CMAKE_COMMAND} -E echo $<TARGET_PROPERTY:tgt,PCH_HEADER_C>
  )
