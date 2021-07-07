CMAKE_REQUIRE_FIND_PACKAGE_<PackageName>
----------------------------------------

.. versionadded:: 3.22

Variable for making :command:`find_package` call ``REQUIRED``.

Every non-``REQUIRED`` :command:`find_package` call in a project can be
turned into ``REQUIRED`` by setting the variable
``CMAKE_REQUIRE_FIND_PACKAGE_<PackageName>`` to ``TRUE``.
This can be used to assert assumptions about build environment and to
ensure the build will fail early if they do not hold.

This switch should be used during the initial CMake run.  Otherwise if
the package has already been found in a previous CMake run, the
variables which have been stored in the cache will still be there.  In
that case it is recommended to remove the cache variables for this
package from the cache using the cache editor or :manual:`cmake(1)` ``-U``

See also the :variable:`CMAKE_DISABLE_FIND_PACKAGE_<PackageName>` variable.
