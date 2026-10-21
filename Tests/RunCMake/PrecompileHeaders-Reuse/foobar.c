#include "bar.h"
#include "foo.h"
#include "foo2.h"

#if defined(EXPECT_PIC) && !defined(__PIC__)
#  error "position independent code flags are missing"
#endif

#if defined(EXPECT_PIE) && !defined(__PIE__)
#  error "position independent executable flags are missing"
#endif

int main(void)
{
  int zeroSize = 0;

#ifdef HAVE_PCH_SUPPORT
  zeroSize = (int)strlen("");
#endif

  return foo() + foo2() + bar() + zeroSize;
}
