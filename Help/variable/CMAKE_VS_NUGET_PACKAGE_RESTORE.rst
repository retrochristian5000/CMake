CMAKE_VS_NUGET_PACKAGE_RESTORE
------------------------------

.. versionadded:: 3.23

Cache variable to control, if Visual Studio should automatically
attempt to restore NuGet packages prior to a build. NuGet packages
can be defined using the :prop_tgt:`VS_PACKAGE_REFERENCES` property
on a target. If no package references are defined, this setting will
do nothing.

The command line option `--resolve-package-references` can be used
alternatively to control the resolve behavior globally. This option
will take precedence over the cache variable.

This setting is stored as a cache entry. Default value is `ON`.

See also the :prop_tgt:`VS_PACKAGE_REFERENCES` property.
