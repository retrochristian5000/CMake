deprecate-findqt
----------------

* A new policy, :policy:`CMP0084`, has been added to pretend that CMake's
  internal :module:`FindQt` module doesn't exist for :command:`find_package`.
  This has been done to allow the Qt project upstream to export its own
  ``QtConfig.cmake`` modules which can be found by ``find_package(Qt)`` without
  having to specify ``CONFIG``. Projects that still want to use Qt 3 and 4 can
  call ``find_package(Qt[34])``, call ``include(FindQt)``, or add
  :module:`FindQt` to their :variable:`CMAKE_MODULE_PATH`.
