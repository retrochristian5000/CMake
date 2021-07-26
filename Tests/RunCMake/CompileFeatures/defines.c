#include <stdio.h>

int main()
{
#if defined(__clang__)
  printf("__clang__\n");
#else
  printf("!__clang__\n");
#endif
#if defined(__GNUC__)
  printf("__GNUC__\n");
#else
  printf("!__GNUC__\n");
#endif
#if defined(__SUNPRO_CC)
  printf("__SUNPRO_CC\n");
#else
  printf("!__SUNPRO_CC\n");
#endif
#if defined(__INTEL_COMPILER)
  printf("__INTEL_COMPILER\n");
#else
  printf("!__INTEL_COMPILER\n");
#endif
#if defined(__TI_COMPILER_VERSION__)
  printf("__TI_COMPILER_VERSION__\n");
#else
  printf("!__TI_COMPILER_VERSION__\n");
#endif
#if defined(__STRICT_ANSI__)
  printf("__STRICT_ANSI__\n");
#else
  printf("!__STRICT_ANSI__\n");
#endif
  printf("__STDC_VERSION__=%u\n\n", __STDC_VERSION__);

#if (defined(__clang__) || defined(__GNUC__) ||                               \
     defined(__TI_COMPILER_VERSION__)) &&                                     \
  !defined(__STRICT_ANSI__) && !defined(_MSC_VER)
  printf("GNU ");
#endif

  printf("C"
#if __STDC_VERSION__ > 201710L
         "23"
#elif __STDC_VERSION__ >= 201710L
         "17"
#elif __STDC_VERSION__ >= 201000L
         "11"
#elif __STDC_VERSION__ >= 199901L
         "99"
#else
         "90"
#endif
         "\n");
}
