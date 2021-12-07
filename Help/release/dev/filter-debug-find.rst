filter-debug-find
-----------------

* The :manual:`cmake(1)` command line tool's ``--debug-find`` option
  gained ability to take a =PackageName,... value
  to enable find_package debug messages scoped for specific packages

* The :manual:`cmake(1)` command line tool gained a ``--debug-find-var``
  option to enable debug messages for specific find_* calls
  that use given var result variable.