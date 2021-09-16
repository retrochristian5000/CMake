#if defined(__GNUC__) &&                                                      \
  !(defined(__INTEL_COMPILER) || defined(__INTEL_LLVM_COMPILER))
void THIS_IS_GNU();
#endif
#ifdef __MINGW64__
void THIS_IS_MINGW64();
#endif
#ifdef __MINGW32__
void THIS_IS_MINGW32();
#endif
#ifdef __CYGWIN__
void THIS_IS_CYGWIN();
#endif
