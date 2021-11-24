CMAKE_VS_NUGET_PACKAGE_RESTORE
------------------------------

.. versionadded:: 3.23

Switch to control, if Visual Studio should automatically attempt to
restore NuGet packages prior to a build. NuGet packages can be
defined using the :prop_tgt:`VS_PACKAGE_REFERENCES` property on a
target. If no packages are defined, this setting will do nothing.

This setting is stored as a cache entry. Default value is `ON`.

See also the :prop_tgt:`VS_PACKAGE_REFERENCES` property.
