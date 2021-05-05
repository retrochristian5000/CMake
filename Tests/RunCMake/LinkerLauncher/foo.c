#include <stdio.h>

#if defined(EXPORT) && defined(_WIN32)
#  define EXPORT_ATTR __declspec(dllexport)
#else
#  define EXPORT_ATTR
#endif

void EXPORT_ATTR hello(void)
{
  printf("hello, world\n");
}
