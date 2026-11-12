find_package(ScopeTest CONFIG ${import_args} REQUIRED COMPONENTS target)
expect_scope_targets("${expect_global}")

# Reusing the root must not prevent a later find from loading an appendix.
find_package(ScopeTest CONFIG ${import_args} REQUIRED COMPONENTS extra)
expect_global(ScopeTest::extra "${expect_global}")

find_package(ScopeTest CONFIG ${import_args} REQUIRED COMPONENTS extra)
expect_scope_targets("${expect_global}")
expect_property(
  ScopeTest::extra INTERFACE_COMPILE_DEFINITIONS "definitions"
  "$<$<CONFIG:debug>:SCOPE_EXTRA=1>")
