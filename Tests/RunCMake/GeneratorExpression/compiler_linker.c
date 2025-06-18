
#include <string.h>

#define xstr(s) str(s)
#define str(s) #s

int main(void)
{
  if (strcmp(xstr(VAR), xstr(GENEX)) != 0) {
    return 1;
  }

  return 0;
}
