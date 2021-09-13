CMAKE_CONFIGURATION_TYPES
-------------------------

Specifies the available build types on multi-config generators.

This specifies what build types (configurations) will be available
such as ``Debug``, ``Release``, ``RelWithDebInfo`` etc.

This variable is initialized by the first :command:`project` or
:command:`enable_language` command called in a project when a new build
tree is first created.  If the :envvar:`CMAKE_CONFIGURATION_TYPES`
environment variable is set, its value is used.  Otherwise, the default
value is generator-specific.

See :manual:`cmake-buildsystem(7)` for more details on build configurations.
