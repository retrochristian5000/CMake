#ifdef DEF_SubDirAImported
#  error "DEF_SubDirAImported is defined but should not be!"
#endif
#ifndef DEF_SubDirBImported
#  error "DEF_SubDirBImported is not defined but should be!"
#endif
#ifdef DEF_TopDirImported
#  error "DEF_TopDirImported is defined but should not be!"
#endif

int main(void)
{
  return 0;
}
