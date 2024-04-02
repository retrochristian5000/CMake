#include "cxx_std.h"
#if defined(CXX_STD) && CXX_STD <= 201103L &&                                 \
  !(CXX_STD == 201103L &&                                                     \
    ((defined(__GNUC__) && defined(__GNUC_MINOR__) && __GNUC__ == 4 &&        \
      __GNUC_MINOR__ <= 8 && !defined(__clang__) &&                           \
      !defined(__INTEL_COMPILER) && !defined(__PGI)) ||                       \
     (defined(__SUNPRO_CC) && __SUNPRO_CC < 0x5150)))
#  error "cxx_std_14 not honored"
#endif
