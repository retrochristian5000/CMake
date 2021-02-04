IMPORTED_OBJECTS
----------------

.. versionadded:: 3.9

A :ref:`semicolon-separated list <CMake Language Lists>` of absolute paths
to the object files on disk for an :ref:`imported <Imported targets>`
:ref:`object library <object libraries>`.

Ignored for non-imported targets.

Projects may skip ``IMPORTED_OBJECTS`` if the configuration-specific
property :prop_tgt:`IMPORTED_OBJECTS_<CONFIG>` is set instead, except in
situations as noted in the section below.


Xcode Generator Considerations
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. versionadded:: 3.20

When using the :generator:`Xcode` generator, the project may be built for
more than one architecture.  For device builds, it may also be built with
either the device or simulator SDK.  This means the architecture isn't
typically known at configuration time.  One way of handling this is to
ensure that every object file listed in the ``IMPORTED_OBJECTS`` property
is a universal object file with slices for every architecture that either
SDK may need.  This isn't always possible, as some architectures are
supported by both device and simulator SDKs (e.g. ``arm64`` with Xcode 12
or later).

An alternative way of handling the variety of architectures and SDKs is to
incorporate both into the path of the object files.  Xcode variables of
the form ``$(...)`` can be used to represent these aspects, which Xcode
will substitute with the appropriate values at build time.  CMake doesn't
interpret these variables and embeds them unchanged in the Xcode project
file.  ``$(CURRENT_ARCH)`` can be used to represent the architecture,
while ``$(EFFECTIVE_PLATFORM_NAME)`` can be used to differentiate between
SDKs.

The following shows one example of how these two variables can be used to
refer to an object file whose location depends on both the SDK and the
architecture:

.. code-block:: cmake

  add_library(someObjs OBJECT IMPORTED)
  set_property(TARGET someObjs PROPERTY IMPORTED_OBJECTS
    "/path/to/somewhere/objects$(EFFECTIVE_PLATFORM_NAME)/$(CURRENT_ARCH)/func.o"
  )

  # Example paths:
  #   /path/to/somewhere/objects-iphoneos/arm64/func.o
  #   /path/to/somewhere/objects-iphonesimulator/x86_64/func.o

In some cases, you may want to have configuration-specific object files
as well.  The :variable:`CMAKE_CFG_INTDIR` variable can be a convenient
way of capturing this in combination with the SDK:

.. code-block:: cmake

  add_library(someObjs OBJECT IMPORTED)
  set_property(TARGET someObjs PROPERTY IMPORTED_OBJECTS
    "/path/to/somewhere/${CMAKE_CFG_INTDIR}/$(CURRENT_ARCH)/func.o"
  )

  # Example paths:
  #   /path/to/somewhere/Release-iphoneos/arm64/func.o
  #   /path/to/somewhere/Debug-iphonesimulator/x86_64/func.o

When any Xcode variable or :variable:`CMAKE_CFG_INTDIR` is used, CMake is
not able to fully evaluate the path(s) at configure time.  One consequence
of this is that the configuration-specific
:prop_tgt:`IMPORTED_OBJECTS_<CONFIG>` properties cannot be used, since
CMake cannot determine whether an object file at a particular ``<CONFIG>``'s
location exists.  The ``IMPORTED_OBJECTS`` property must be used for these
situations and the configuration-specific aspects of the path must be handled
by using :variable:`CMAKE_CFG_INTDIR` or with another Xcode variable
``$(CONFIGURATION)``.
