#include <string.h>

unsigned char const long_unsigned[] = {
#include "long.c.txt"
};

char const long_signed[] = {
#include "long_signed.c.txt"
};

int main(void)
{
  if (sizeof(long_unsigned) != sizeof(long_signed)) {
    return 1;
  }
  if (memcmp(long_unsigned, long_signed, sizeof(long_unsigned))) {
    return 1;
  }
  return 0;
}
