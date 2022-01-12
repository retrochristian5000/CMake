INTERFACE_LINK_LIBRARIES_DIRECT
-------------------------------

List of libraries that consumers of this library should treat
as direct link dependencies.

This target property may be set to *include* items in a dependent
target's final set of direct link dependencies.  See the
:prop_tgt:`INTERFACE_LINK_LIBRARIES_DIRECT_EXCLUDE` target property
to exclude items.

The initial set of a dependent target's direct link dependencies is
specified by its :prop_tgt:`LINK_LIBRARIES` target property.  Indirect
link dependencies are specified by the transitive closure of the direct
link dependencies' :prop_tgt:`INTERFACE_LINK_LIBRARIES` properties.
Any link dependency may specify additional direct link dependencies
using the ``INTERFACE_LINK_LIBRARIES_DIRECT`` target property.
The set of direct link dependencies is filtered to exclude items named
by any dependency's :prop_tgt:`INTERFACE_LINK_LIBRARIES_DIRECT_EXCLUDE`
target property.

.. |INTERFACE_PROPERTY_LINK_DIRECT| replace:: ``INTERFACE_LINK_LIBRARIES_DIRECT``
.. include:: INTERFACE_LINK_LIBRARIES_DIRECT.txt

Example: Static Plugin
^^^^^^^^^^^^^^^^^^^^^^

Consider a static library ``Foo`` that provides a static plugin
``FooPlugin`` to consuming application executables, where the
implementation of the plugin depends on ``Foo`` and other things.
In this case, the application should link to ``FooPlugin`` directly,
before ``Foo``.  However, the application author only knows about ``Foo``.
We can express this as follows:

.. code-block:: cmake

  # Core library used by other components.
  add_library(Core STATIC core.cpp)

  # Foo is a static library for use by applications.
  # Implementation of Foo depends on Core.
  add_library(Foo STATIC foo.cpp foo_plugin_helper.cpp)
  target_link_libraries(Foo PRIVATE Core)

  # Extra parts of Foo for use by its static plugins.
  # Implementation of Foo's extra parts depends on both Core and Foo.
  add_library(FooExtras STATIC foo_extras.cpp)
  target_link_libraries(FooExtras PRIVATE Core Foo)

  # The Foo library has an associated static plugin
  # that should be linked into the final executable.
  # Implementation of the plugin depends on Core, Foo, and FooExtras.
  add_library(FooPlugin STATIC foo_plugin.cpp)
  target_link_libraries(FooPlugin PRIVATE Core Foo FooExtras)

  # An app that links Foo should link Foo's plugin directly.
  set_property(TARGET Foo APPEND PROPERTY INTERFACE_LINK_LIBRARIES_DIRECT FooPlugin)

  # An app does not need to link Foo directly because the plugin links it.
  set_property(TARGET Foo APPEND PROPERTY INTERFACE_LINK_LIBRARIES_DIRECT_EXCLUDE Foo)

An application ``app`` only needs to specify that it links to ``Foo``:

.. code-block:: cmake

  add_executable(app main.cpp)
  target_link_libraries(app PRIVATE Foo)

The ``INTERFACE_LINK_LIBRARIES_DIRECT`` target property on ``Foo`` tells
CMake to pretend that ``app`` also links directly to ``FooPlugin``.
The ``INTERFACE_LINK_LIBRARIES_DIRECT_EXCLUDE`` target property on ``Foo``
tells CMake to pretend that ``app`` did *not* link directly to ``Foo``.
Instead, ``Foo`` will be linked as a dependency of ``FooPlugin``.  The
final link line for ``app`` will link the libraries in the following
order:

* ``FooPlugin`` as a direct link dependency of ``app``.
* ``FooExtras`` as a dependency of ``FooPlugin``.
* ``Foo`` as a dependency of ``FooPlugin`` and ``FooExtras``.
* ``Core`` as a dependency of ``FooPlugin``, ``FooExtras``, and ``Foo``.
