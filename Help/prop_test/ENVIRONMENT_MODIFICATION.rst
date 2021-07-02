ENVIRONMENT_MODIFICATION
------------------------

Specify environment variables that should be modified for running a test. Note
that the operations performed by this property are performed after the
:prop_test:`ENVIRONMENT` property is applied.

If set to a list of environment variables and values of the form
``MYVAR=OP:VALUE``. The ``OP`` may be one of:

  - ``ignore``: Ignore all modifications prior to this entry.
  - ``set``: Replaces the current value of ``MYVAR`` with ``VALUE``.
  - ``unset``: Unsets the current value of ``MYVAR``.
  - ``string_append``: Appends ``VALUE`` to the current value of ``MYVAR``.
  - ``string_prepend``: Prepends ``VALUE`` to the current value of ``MYVAR``.
  - ``path_list_append``: Appends ``VALUE`` to the current value of ``MYVAR``
    using the platform-specific list separator.
  - ``path_list_prepend``: Prepends ``VALUE`` to the current value of
    ``MYVAR`` using the platform-specific list separator.
  - ``cmake_list_append``: Appends ``VALUE`` to the current value of ``MYVAR``
    using ``;`` as the separator.
  - ``cmake_list_prepend``: Prepends ``VALUE`` to the current value of
    ``MYVAR`` using ``;`` as the separator.

Modifications to the same variable will be collected before being applied. For
example, an entry of ``MYVAR=ignore:`` will cancel any prior modifications to
``MYVAR`` described in the :prop_test:`ENVIRONMENT_MODIFICATION` property.

Unrecognized ``OP`` values will result in the test failing before it is
executed. This is so that future operations may be added without changing
valid behavior of existing tests.

The environment is restored to its previous state after the test is done.
