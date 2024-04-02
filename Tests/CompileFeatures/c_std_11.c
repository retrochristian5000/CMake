#include "c_std.h"
#if defined(C_STD) && C_STD < 201000L &&                                      \
  !(defined(__SUNPRO_C) && __SUNPRO_C < 0x5140 && C_STD == 199901L)
#  error "c_std_11 not honored"
#endif
