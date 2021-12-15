DOTNET_SDK
----------

.. versionadded:: 3.23

Specify the .NET SDK used for SDK-style projects.

Used to specify the .NET SDK for C# projects. For example: ``Microsoft.NET.Sdk``.

This property is only evaluated for :ref:`Visual Studio Generators`
VS 2019 and above.

This property will be ignored when set for targets that are not C#-only
(aka non-managed targets).

This property must be a non-empty string to generate .Net SDK-style projects.
`CMake` does not perform any validations for the value of the property.

Can be initialized for all targets using the variable
:variable:`CMAKE_DOTNET_SDK`.
