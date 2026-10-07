#include <stdio.h>

extern int foo(int n);

int main(void)
{
  printf("%d\n", foo(100000));
  return 0;
}
