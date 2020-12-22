
set(_compiler_id_pp_test "defined(__FUJITSU) && !defined(__CLANG_FUJITSU)")

set(_compiler_id_version_compute "
# if defined(__FCC_major__)
#   define @PREFIX@COMPILER_VERSION_MAJOR @MACRO_DEC@(__FCC_major__)
#   define @PREFIX@COMPILER_VERSION_MINOR @MACRO_DEC@(__FCC_minor__)
#   define @PREFIX@COMPILER_VERSION_PATCH @MACRO_DEC@(__FCC_patchlevel__)
# elif defined(__FCC_VERSION)
#   define @PREFIX@COMPILER_VERSION_MAJOR @MACRO_DEC@(0)
#   define @PREFIX@COMPILER_VERSION_MINOR @MACRO_DEC@(0)
#   define @PREFIX@COMPILER_VERSION_PATCH @MACRO_DEC@(0)
#   define @PREFIX@COMPILER_VERSION_TWEAK @MACRO_DEC@(__FCC_VERSION/100)
# elif defined(__fcc_version)
#   define @PREFIX@COMPILER_VERSION_MAJOR @MACRO_DEC@(0)
#   define @PREFIX@COMPILER_VERSION_MINOR @MACRO_DEC@(0)
#   define @PREFIX@COMPILER_VERSION_PATCH @MACRO_DEC@(0)
#   define @PREFIX@COMPILER_VERSION_TWEAK @MACRO_DEC@(__fcc_version/100)
# endif
")
