#if defined(__INTEL_COMPILER) && defined(_MSVC_LANG) && _MSVC_LANG < 201403L
#  if defined(__INTEL_CXX11_MODE__)
#    if defined(__cpp_aggregate_nsdmi)
#      define CXX_STD 201402L
#    else
#      define CXX_STD 201103L
#    endif
#  else
#    define CXX_STD 199711L
#  endif
#elif defined(__INTEL_COMPILER) && defined(_MSVC_LANG) &&                     \
  _MSVC_LANG == 201703L && defined(__cpp_aggregate_paren_init)
#  define CXX_STD 202002L
#elif defined(_MSC_VER) && defined(_MSVC_LANG)
#  define CXX_STD _MSVC_LANG
#elif __cplusplus == 201103L && defined(__cpp_namespace_attributes)
#  define CXX_STD 201703L
#elif __cplusplus == 201103L && defined(__cpp_aggregate_nsdmi)
#  define CXX_STD 201402L
#else
#  define CXX_STD __cplusplus
#endif
