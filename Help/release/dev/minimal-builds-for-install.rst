minimal-builds-for-install
--------------------------

* The :ref:`Ninja Generators`, :ref:`Makefile Generators`, and
  :generator:`FASTBuild` generator now provide ``install_prep/all`` and
  ``install_prep/<component>`` convenience build targets by default.  The
  :variable:`CMAKE_SKIP_INSTALL_PREP_TARGETS` variable may be enabled to
  skip generating them.  These targets build only the known prerequisites of
  a subsequent full or component-specific installation, including targets
  that produce installed C++ module BMIs, module export metadata, or
  generated files.
