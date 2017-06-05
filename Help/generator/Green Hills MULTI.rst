Green Hills MULTI
-----------------

Generates Green Hills MULTI project files (experimental, work-in-progress).

The ``-A <arch>`` can be supplied for setting the target architecture.
``<arch>`` must be one of "ARM", "PPC", or "86".  If the target architecture
is not specified then the default architecture of "ARM" will be used.

The ``-T <toolset>`` can be supplied for setting the toolset to be used.
All toolsets are expected to be located at ``GHS_TOOLSET_ROOT``.
If the toolset is not specified then the latest toolset will be used.
For example if ``toolset`` is ``comp_201517`` then
:variable:`CMAKE_MAKE_PROGRAM` will be set to `c:/ghs/comp_201517/gmake.exe`.

Customizations are available through the following cache variables:

* ``GHS_TARGET_PLATFORM``

Default to ``integrity``.
Valid values are ``integrity``, ``integrity178``, ``integrityIOT``,
``threadx``, ``uvelosity``, ``velosity``, ``vxworks``, ``standalone``.
Used to determine which OS to use.  Also, determines ``<platform>`` value
in ``GHS_PRIMARY_TARGET``.

* ``GHS_PRIMARY_TARGET``

Default to ``<arch>_<platform>.tgt``.
Used to populate the ``primaryTarget`` setting in the top-level project.

* ``GHS_OS_ROOT``

Default to ``C:/ghs``.  Used for determining ``GHS_OS_DIR``.

* ``GHS_OS_DIR``

Default to latest platform OS installation located in ``GHS_OS_ROOT``.
Used to set ``-os_dir`` option in top-level project.

* ``GHS_BSP_NAME``

No default value.
Used to set ``-bsp`` option in top-level project.

* ``GHS_CUSTOMIZATION``

No default value.
Used to set ``customization`` commands in top-level project.

* ``GHS_GPJ_MACROS``

No default value.
Used to set ``macro`` commands in top-level project.

* ``GHS_TOOLSET_ROOT``

Default to ``C:/ghs``.  Root path for ``toolset``.

.. note::
  This generator is deemed experimental as of CMake |release|
  and is still a work in progress.  Future versions of CMake
  may make breaking changes as the generator matures.
