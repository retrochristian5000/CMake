CMAKE_VS_TARGET_FRAMEWORK_VERSION
---------------------------------

.. versionadded:: 3.22

Visual Studio target framework version.

In cases that the MSBuild default value for ``TargetFrameworkVersion`` does
not work, the :ref:`Visual Studio Generators` may choose another value to
explicitly specify in ``.csproj`` files.  CMake provides the chosen
target framework version in this variable.

See the :variable:`CMAKE_DOTNET_TARGET_FRAMEWORK_VERSION` variable
and :prop_tgt:`DOTNET_TARGET_FRAMEWORK_VERSION` target property to
specify custom ``TargetFrameworkVersion`` values for project targets.
