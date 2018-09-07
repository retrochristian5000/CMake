#ifdef DEF_SubDirAImported
#  error "DEF_SubDirAImported is defined but should not be!"
#endif
#ifdef DEF_SubDirBImported
#  error "DEF_SubDirBImported is defined but should not be!"
#endif
#ifndef DEF_TopDirImported
#  error "DEF_TopDirImported is not defined but should be!"
#endif

int main(void)
{
  return 0;
}
