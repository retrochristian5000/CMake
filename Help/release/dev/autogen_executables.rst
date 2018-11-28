AUTO*_EXECUTABLE
----------------

* The :variable:`AUTOMOC_EXECUTABLE`, :variable:`AUTORCC_EXECUTABLE` and
  :variable:`AUTOUIC_EXECUTABLE` variables all take a path to an executable
  and force automoc/autorcc/autouic to use this executable.

  Setting these will also prevent the configure time testing for these
  executables. This is mainly useful when you build these tools yourself.
