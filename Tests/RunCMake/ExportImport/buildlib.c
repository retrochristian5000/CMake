#if !(!defined(PROJECT_LOCAL_INTERFACE) && defined(BUILD_INTERFACE) &&        \
      !defined(INSTALL_INTERFACE))
#  error "Incorrect compile definitions"
#endif

void buildlib(void)
{
}
