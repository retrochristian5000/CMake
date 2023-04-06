INTERFACE_AUTOMOC_MACRO_NAMES
-----------------------------

.. versionadded:: 3.27

A :ref:`semicolon-separated list <CMake Language Lists>` list of macro names
used by :prop_tgt:`AUTOMOC_MACRO_NAMES` to inherit macro names from
dependencies.

When a target with :prop_tgt:`AUTOMOC` enabled linkes to a library that sets
``INTERFACE_AUTOMOC_MACRO_NAMES``, the target inherits the listed macro names
and adds them to the value of its :prop_tgt:`AUTOMOC_MACRO_NAMES` variable.
Thus, the target will automatically generate MOC files for source files that
contain the inherited macro names.

By default ``INTERFACE_AUTOMOC_MACRO_NAMES`` is empty.

See the :manual:`cmake-qt(7)` manual for more information on using CMake
with Qt.

Example 1
^^^^^^^^^

In this case the macro names(``"STATIC_LIB_1;STATIC_LIB_2"``) of ``static_lib``
are inherited by ``myapp`` as if specified by ``AUTOMOC_MACRO_NAMES`` of
``myapp``.  This means that ``myapp`` will automatically ``moc`` process source
files that contain ``STATIC_LIB_1`` or ``STATIC_LIB_2``.

.. code-block:: cmake

  set(AUTOMOC ON)
  add_executable(myapp main.cpp)
  target_link_libraries(myapp PRIVATE static_lib)

  add_library(static_lib STATIC static.cpp)
  set_property(TARGET static_lib PROPERTY INTERFACE_AUTOMOC_MACRO_NAMES "STATIC_LIB_1;STATIC_LIB_2")

Example 2
^^^^^^^^^

In this example, the ``INTERFACE_AUTOMOC_MACRO_NAMES`` target property of the
``*_link`` libraries propagates through ``shared_lib``, ``static_lib`` and
``interface_lib``.  This means that any target that links against them will
automatically ``moc`` process source files that contain the respective macro
names: ``SHARED_LINK_LIB``, ``STATIC_LINK_LIB`` or ``INTERFACE_LINK_LIB``.
If you link the libraries with the ``PRIVATE`` keyword, the target links
will not inherit the variables.

.. code-block:: cmake

  set(AUTOMOC ON)
  add_library(shared_lib SHARED lib.cpp)
  add_library(static_lib STATIC lib.cpp)
  add_library(interface_lib INTERFACE)

  target_link_libraries(shared_lib PUBLIC shared_link_lib)
  target_link_libraries(static_lib PUBLIC static_link_lib)
  target_link_libraries(interface_lib PUBLIC interface_link_lib)

  add_library(shared_link_lib SHARED lib.cpp)
  set_property(TARGET shared_link_lib PROPERTY INTERFACE_AUTOMOC_MACRO_NAMES "SHARED_LINK_LIB")

  add_library(static_link_lib STATIC lib.cpp)
  set_property(TARGET static_link_lib PROPERTY INTERFACE_AUTOMOC_MACRO_NAMES "STATIC_LINK_LIB")

  add_library(interface_link_lib INTERFACE)
  set_property(TARGET interface_link_lib PROPERTY INTERFACE_AUTOMOC_MACRO_NAMES "INTERFACE_LINK_LIB")
