include(Compiler/PGI-CXX)
include(Compiler/NVHPC)

# Needed so that we support `LANGUAGE` property correctly
set(CMAKE_CXX_COMPILE_OPTIONS_EXPLICIT_LANGUAGE -x c++)

# Needed to generate dependency information with the Ninja generator
# as currently NVHPC -MD implicitly implies `-E`
set(CMAKE_CXX_DEPENDS_EXTRA_COMMANDS "<CMAKE_CXX_COMPILER> <DEFINES> <INCLUDES> <FLAGS> -x c++ -M <SOURCE> -MT <OBJECT> -MD<DEP_FILE>")

__compiler_nvhpc(CXX)
