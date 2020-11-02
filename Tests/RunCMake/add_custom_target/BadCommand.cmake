add_custom_target("a" COMMAND "$<1:$<OUTPUT_CONFIG:a>>")
add_custom_target("b" COMMAND "$<1:$<COMMAND_CONFIG:b>>")
