/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing#kwsys for details.  */
#include "kwsysPrivate.h"
#include KWSYS_HEADER(String.h)

/* Work-around CMake dependency scanning limitation.  This must
   duplicate the above list of headers.  */
#if 0
#  include "String.h.in"
#endif

#include <locale.h>
#include <stdio.h>

static int testString_tolower(void)
{
  int i = 0;
  int j;
  for (; i < 'A'; ++i) {
    if ((j = kwsysString_tolower(i)) != i) {
      printf("tolower(%d) failed: %d\n", i, j);
      return 1;
    }
  }
  for (; i <= 'Z'; ++i) {
    if ((j = kwsysString_tolower(i)) != i + 32) {
      printf("tolower(%d) failed: %d\n", i, j);
      return 1;
    }
  }
  for (; i < 256; ++i) {
    if ((j = kwsysString_tolower(i)) != i) {
      printf("tolower(%d) failed: %d\n", i, j);
      return 1;
    }
  }
  return 0;
}

static int testString_toupper(void)
{
  int i = 0;
  int j;
  for (; i < 'a'; ++i) {
    if ((j = kwsysString_toupper(i)) != i) {
      printf("toupper(%d) failed: %d\n", i, j);
      return 1;
    }
  }
  for (; i <= 'z'; ++i) {
    if ((j = kwsysString_toupper(i)) != i - 32) {
      printf("toupper(%d) failed: %d\n", i, j);
      return 1;
    }
  }
  for (; i < 256; ++i) {
    if ((j = kwsysString_toupper(i)) != i) {
      printf("toupper(%d) failed: %d\n", i, j);
      return 1;
    }
  }
  return 0;
}

int testString(int argc, char* argv[])
{
  int result = 0;
  (void)argc;
  (void)argv;
  setlocale(LC_CTYPE, "");

  result |= testString_tolower();
  result |= testString_toupper();

  return result;
}
