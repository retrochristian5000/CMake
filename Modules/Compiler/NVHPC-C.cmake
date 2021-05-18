include(Compiler/PGI-C)
include(Compiler/NVHPC)

# Needed so that we support `LANGUAGE` property correctly
set(CMAKE_C_COMPILE_OPTIONS_EXPLICIT_LANGUAGE -x c)

# Needed to generate dependency information with the Ninja generator
# as currently NVHPC -MD implicitly implies `-E`
set(CMAKE_C_DEPENDS_EXTRA_COMMANDS "<CMAKE_C_COMPILER> <DEFINES> <INCLUDES> <FLAGS> -x c -M <SOURCE> -MT <OBJECT> -MD<DEP_FILE>")

__compiler_nvhpc(C)
