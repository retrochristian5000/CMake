/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing#kwsys for details.  */
#ifdef KWSYS_STRING_C
/*
All code in this source file is conditionally compiled to work-around
template definition auto-search on VMS.  Other source files in this
directory that use the stl string cause the compiler to load this
source to try to get the definition of the string template.  This
condition blocks the compiler from seeing the symbols defined here.
*/
#  include "kwsysPrivate.h"
#  include KWSYS_HEADER(String.h)

/* Work-around CMake dependency scanning limitation.  This must
   duplicate the above list of headers.  */
#  if 0
#    include "String.h.in"
#  endif

unsigned char kwsysString_tolower(char c)
{
  unsigned char const uc = (unsigned char)c;
  return uc ^ ((((uc + 230 - 'A') & 0xFF) / 230) << 5);
}

unsigned char kwsysString_toupper(char c)
{
  unsigned char const uc = (unsigned char)c;
  return uc ^ ((((uc + 230 - 'a') & 0xFF) / 230) << 5);
}

int kwsysString_strcasecmp(char const* lhs, char const* rhs)
{
  int result;
  while ((result = kwsysString_tolower(*lhs) - kwsysString_tolower(*rhs++),
          result == 0) &&
         *lhs++) {
  }
  return result;
}

int kwsysString_strncasecmp(char const* lhs, char const* rhs, size_t n)
{
  int result = 0;
  while (n &&
         (result = kwsysString_tolower(*lhs) - kwsysString_tolower(*rhs++),
          result == 0) &&
         *lhs++) {
    --n;
  }
  return result;
}

#endif /* KWSYS_STRING_C */
