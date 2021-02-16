include(RunCMake)

foreach (subcommand IN ITEMS GET INSERT REMOVE_AT SUBLIST-length SUBLIST-start)
  run_cmake(CMP0121-${subcommand}-WARN)
  run_cmake(CMP0121-${subcommand}-OLD)
  run_cmake(CMP0121-${subcommand}-NEW)
endforeach ()
