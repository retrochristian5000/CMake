#include "cxx_std.h"
#if defined(CXX_STD) && CXX_STD < 201103L &&                                  \
  !(CXX_STD == 199711L &&                                                     \
    (defined(__IBMCPP__) && defined(_AIX) && __IBMCPP__ == 1610))
#  error "cxx_std_11 not honored"
#endif
