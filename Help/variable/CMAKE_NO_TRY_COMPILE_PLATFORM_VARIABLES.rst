CMAKE_NO_TRY_COMPILE_PLATFORM_VARIABLES
------------------------------------

.. versionadded:: 3.24

Switch to control wether or not variables listed within
variable:`CMAKE_TRY_COMPILE_PLATFORM_VARIABLES` will be propagated into
the test project of :command:`try_compile`

If :policy:`CMP0137` is ``NEW`` and ``CMAKE_NO_TRY_COMPILE_PLATFORM_VARIABLES``
is empty. It will be treated as if the value of
``CMAKE_NO_TRY_COMPILE_PLATFORM_VARIABLES`` is ``false``.
If :policy:`CMP0137` is ``OLD`` an empty value will be treated as ``true``
but it will have no effect on :command:`try_compile` using the source
function signature and ``CMAKE_TRY_COMPILE_PLATFORM_VARIABLES`` will be
passed as expected. Setting the value of
``CMAKE_NO_TRY_COMPILE_PLATFORM_VARIABLES`` explicitly to ``true`` will
however deactivate the passing of the variables in
``CMAKE_TRY_COMPILE_PLATFORM_VARIABLES`` even when using source signature.
