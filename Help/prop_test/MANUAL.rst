MANUAL
------

If set to true, the test is not run by default.  It is only run when
explicitly selected with the :option:`ctest -R` option, which makes it
suitable for long-running, interactive, or otherwise special tests.

Unlike :prop_test:`DISABLED`, a ``MANUAL`` test can be run when
its name matches the ``-R`` regular expression.

Other selection options (``-I``, ``-L``, ``-E``, ``-LE``) do not by
themselves cause a ``MANUAL`` test to run.
