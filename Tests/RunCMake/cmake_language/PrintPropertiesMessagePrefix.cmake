enable_language(C)
add_library(mylib STATIC nothing.c)
set_target_properties(mylib PROPERTIES MY_PROP "my_value")

# Default banner opens the message.
cmake_language(PRINT_PROPERTIES TARGETS mylib NAMED MY_PROP)

# The internal __MESSAGE_PREFIX keyword overrides the default banner.
cmake_language(PRINT_PROPERTIES TARGETS mylib NAMED MY_PROP
  __MESSAGE_PREFIX "CUSTOM_BANNER\n")

# ALL mode with a custom prefix
cmake_language(PRINT_PROPERTIES TARGETS mylib ALL
  PROPERTY_NAME_REGEX "^MY_PROP$" __MESSAGE_PREFIX "BANNER\n")
