HIP_ARCHITECTURES
-----------------

.. versionadded:: 3.21

List of AMD GPU architectures to generate device code for.

An empty or false value (e.g. ``OFF``) defers architecture generation to compiler
defaults.

This property is initialized by the value of the :variable:`CMAKE_HIP_ARCHITECTURES`
variable if it is set when a target is created.

The HIP compilation model has two modes: whole, and separable. Whole
compilation, has generation of device specific code occurring at compile time.
While separable compilation defers generation of device specific code to link
time. Therefore the ``HIP_ARCHITECTURES`` target property should be set on
targets that compile or linking with any HIP sources.

Examples
^^^^^^^^

.. code-block:: cmake

  set_property(TARGET tgt PROPERTY HIP_ARCHITECTURES gfx801 gfx900)

Generates code both ``gfx801``, and ``gfx900``.

