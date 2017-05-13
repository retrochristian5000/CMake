Green Hills MULTI
-----------------

Generates Green Hills MULTI project files (experimental, work-in-progress).

The ``-A <arch>`` can be supplied for setting the target architecture;
this also sets the :variable:`CMAKE_GENERATOR_PLATFORM` variable.
``<arch>`` must be one of "ARM", "PPC", or "86".  If the target architecture
is not specified then the default target of "ARM" will be used.

Customizations are available through the following cache variables:

* ``GHS_BSP_NAME``
* ``GHS_CUSTOMIZATION``
* ``GHS_GPJ_MACROS``
* ``GHS_OS_DIR``
* ``GHS_COMPILER_ROOT``

.. note::
  This generator is deemed experimental as of CMake |release|
  and is still a work in progress.  Future versions of CMake
  may make breaking changes as the generator matures.
