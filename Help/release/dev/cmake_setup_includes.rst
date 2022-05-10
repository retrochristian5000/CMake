cmake_setup_includes
--------------------

* The :variable:`CMAKE_SETUP_INCLUDES` variable was added to allow injection
  of custom code at the site of the first :command:`project` call, after the
  host and target platform details have been determined.
