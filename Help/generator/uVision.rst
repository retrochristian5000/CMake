uVision
------------

.. versionadded:: 3.20

Generates uVision project files.

Instance Selection
^^^^^^^^^^^^^^^^^^

uVision supports multiple installations on the same machine.
The :variable:`CMAKE_GENERATOR_INSTANCE` variable may be set as a
cache entry containing the absolute path to a uVision instance.

Toolset Selection
^^^^^^^^^^^^^^^^^

The ``ARMClang`` toolset is selected by default.
The :variable:`CMAKE_GENERATOR_TOOLSET` option may be set, perhaps
via the :manual:`cmake(1)` ``-T`` option, to specify another toolset.

The toolset must be located in the ARM Folder of the
uVision installation folder (e.g. 'C:\\Keil\\ARM\\ARMCC' for the 'ARMCC' toolset).

Device Selection
^^^^^^^^^^^^^^^^

The device to use must be selected by setting the variable :variable:`CMAKE_UV_DEVICE`.
This must be done either with the :manual:`cmake(1)` ``-D`` option or prior to the very first
:command:`project` command.

The generator always tries to create a .uvprojx project file first, which uses device packs.
If no Pack for the selected device is installed, the generator tries to generate a .uvproj project,
which uses the legacy device database.

Packs
^^^^^

Packs are only available in the .uvprojx project format and must be added as imported module
libraries and linked against.

.. code-block:: cmake

  add_library(ARM::CMSIS:CORE MODULE IMPORTED)
  set_target_properties(ARM::CMSIS:CORE PROPERTIES
    IMPORTED_LOCATION "Pack"
    UV_PACK           "ARM.CMSIS"
    UV_COMPONENT      "ARM::CMSIS:CORE")
  target_link_libraries(MyProject PRIVATE ARM::CMSIS:CORE)

.. note::
  This generator is deemed experimental as of CMake |release|
  and is still a work in progress.  Future versions of CMake
  may make breaking changes as the generator matures.
