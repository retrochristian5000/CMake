<PackageName>_ROOT
------------------

Calls to :command:`find_package(<PackageName>)` will search in prefixes
specified by the ``<PackageName>_ROOT`` environment variable, where
``<PackageName>`` is the name given to the ``find_package`` call
and ``_ROOT`` is literal.  See policy :policy:`CMP0074`.

This variable may hold a single prefix or a list of prefixes separated
by ``:`` on UNIX or ``;`` on Windows.

See also the :variable:`<PackageName>_ROOT` CMake variable.
