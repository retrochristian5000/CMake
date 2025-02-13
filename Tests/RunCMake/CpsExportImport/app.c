#include <stdio.h>

extern
#ifdef _WIN32
__declspec(dllimport)
#endif
int ask();

int main()
{
  printf("%i\n", ask());
  return 0;
}
