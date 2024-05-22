#include <windows.h>

#include <stdio.h>
#include <string.h>

int main(void)
{
  STARTUPINFOW si;
  memset(&si, 0, sizeof(si));
  GetStartupInfoW(&si);
  if (si.cbReserved2 != 0 || si.lpReserved2 != NULL) {
    fprintf(stderr, "si.cbReserved2: %u\n", si.cbReserved2);
    fprintf(stderr, "si.lpReserved2: %p\n", si.lpReserved2);
    return 1;
  }
  return 0;
}
