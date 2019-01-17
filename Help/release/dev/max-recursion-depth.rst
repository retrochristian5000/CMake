max-recursion-depth
-------------------

* CMake now imposes a maximum recursion limit to prevent a stack overflow on
  scripts that recurse infinitely. The default limit is 1000, but can be adjusted
  with :variable:`CMAKE_MAXIMUM_RECURSION_DEPTH`.
