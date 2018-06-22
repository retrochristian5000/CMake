#define __STDC_FORMAT_MACROS

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int res = 0;
bool print = false;

void test_expression(int64_t x, int64_t y, const char* text)
{
  bool fail = (x) != (y);
  if (fail) {
    res++;
    printf("Problem with EXPR:");
  }
  if (fail || print) {
    printf("Expression: \"%s\" in CMake returns %" PRId64, text, (y));
    if (fail) {
      printf(" while in C returns: %" PRId64, (x));
    }
    printf("\n");
  }
}

int main(int argc, char* argv[])
{
  if (argc > 1) {
    printf("Usage: %s\n", argv[0]);
    return 1;
  }

#include "MathTestTests.h"

  if (res != 0) {
    printf("%s: %d math tests failed\n", argv[0], res);
    return 1;
  }
  return 0;
}
