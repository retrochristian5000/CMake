#include "FortranStatic.h"
extern void F_FortranStatic(void);

int FortranStaticC(void)
{
  F_FortranStatic();
  return 0;
}
