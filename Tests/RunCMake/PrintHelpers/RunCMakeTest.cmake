include(RunCMake)

run_cmake(Variables)
# Wrapper-deprecated noise is gated to dedicated fixtures below; suppress
# it elsewhere so the unrelated stdout checks aren't polluted by the
# DEPRECATION warning.
block()
  set(RunCMake_TEST_OPTIONS -Wno-deprecated)
  run_cmake(PrintPropertiesArgForwarding)
  run_cmake(PrintPropertiesBackwardCompat)
  run_cmake(PrintPropertiesReservedKeywordPropertyName)
  run_cmake(PrintPropertiesReservedKeywordPositionalArg)
endblock()
run_cmake(PrintPropertiesDeprecation)
