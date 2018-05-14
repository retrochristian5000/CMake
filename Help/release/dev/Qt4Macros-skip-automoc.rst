Qt4Macros-skip-automoc
----------------------

* The :module:`FindQt4` module ``qt4_wrap_cpp``, ``qt4_wrap_ui`` and
  ``qt4_add_resources`` macros now set :prop_sf:`SKIP_AUTOMOC` and
  :prop_sf:`SKIP_AUTOUIC` on their generated files. As these files
  should never need to be processed by moc or uic, this suppresses
  a :policy:`CMP0071`-related warning if said policy is not set.
