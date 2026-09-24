automoc-cxx-modules-scan
------------------------

* :prop_tgt:`AUTOMOC` now has its generated ``mocs_compilation.cpp``
  scanned for C++ module dependencies.  A header that imports a module
  may now be processed by ``moc``.
