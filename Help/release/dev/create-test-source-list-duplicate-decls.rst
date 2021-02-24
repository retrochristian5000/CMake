create-test-source-list-duplicate-decls
---------------------------------------

* The :command:`create_test_sourcelist` command now generates sources without
  duplicate declarations of test functions to avoid lint warnings from
  ``clang-tidy``.
