linker-launcher
---------------

* The :ref:`Makefile Generators` and the :generator:`Ninja` generator learned to
  add linker launcher tools along with the linker for ``EXE``, ``MODULE``,
  ``SHARED``, and ``STATIC`` targets. See the
  :variable:`CMAKE_<TYPE>_LINKER_LAUNCHER` variable and
  :prop_tgt:`<TYPE>_LINKER_LAUNCHER` target property for details.
