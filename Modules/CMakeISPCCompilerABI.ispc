
export void ispcCompilerABI() {

#if defined(__GNU__) && defined(__ELF__) && defined(__ARM_EABI__)
  print("INFO:abi[ELF ARMEABI]");
  static char const info_abi[] =
#elif defined(__GNU__) && defined(__ELF__) && defined(__ARMEB__)
  print("INFO:abi[ELF ARM]");
#elif defined(__GNU__) && defined(__ELF__) && defined(__ARMEL__)
  print("INFO:abi[ELF ARM]");

#elif defined(__linux__) && defined(__ELF__) && defined(__amd64__) &&         \
  defined(__ILP32__)
print("INFO:abi[ELF X32]");

#elif defined(__ELF__)
print("INFO:abi[ELF]");
#endif

// Sync with
//   Help/variable/CMAKE_LANG_COMPILER_ARCHITECTURE_ID.rst
//   Modules/CMakeCompilerABI.h
//   Modules/CMakeFortranCompilerABI.F
//   Modules/CMakeFortranCompilerABI.F90
//   Modules/Internal/CMakeParseCompilerArchitectureId.cmake
#if defined(__APPLE__) && defined(__arm64__)
#  if defined(__ARM64_ARCH_8_32__)
print("INFO:arch[arm64_32]");
#  elif defined(__arm64e__)
print("INFO:arch[arm64e]");
#  else
print("INFO:arch[arm64]");
#  endif
#elif defined(_MSC_VER) && defined(_M_ARM64EC)
print("INFO:arch[arm64ec]");
#elif defined(_MSC_VER) && defined(_M_ARM64)
print("INFO:arch[arm64]");
#elif defined(__arm64ec__)
print("INFO:arch[arm64ec]");
#elif defined(__aarch64__)
print("INFO:arch[aarch64]");
#elif __ARM_ARCH == 7 || _M_ARM == 7 || defined(__ARM_ARCH_7__)
#  if defined(__APPLE__) && defined(__ARM_ARCH_7K__)
print("INFO:arch[armv7k]");
#  elif defined(__APPLE__) && defined(__ARM_ARCH_7S__)
print("INFO:arch[armv7s]");
#  else
print("INFO:arch[armv7]");
#  endif
#elif __ARM_ARCH == 6 || _M_ARM == 6 || defined(__ARM_ARCH_6__)
print("INFO:arch[armv6]");
#elif __ARM_ARCH == 5 || _M_ARM == 5 || defined(__ARM_ARCH_5__)
print("INFO:arch[armv5]");
#elif defined(__alpha) || defined(__alpha) || defined(_M_ALPHA)
print("INFO:arch[alpha]");
#elif defined(__x86_64) || defined(__x86_64__) || defined(__amd64) ||         \
  defined(__amd64__) || defined(_M_X64) || defined(_M_AMD64)
print("INFO:arch[x86_64]");
#elif defined(__i686) || defined(__i686__) || _M_IX86 == 600
print("INFO:arch[i686]");
#elif defined(__i586) || defined(__i586__) || _M_IX86 == 500
print("INFO:arch[i586]");
#elif defined(__i486) || defined(__i486__) || _M_IX86 == 400
print("INFO:arch[i486]");
#elif defined(__i386) || defined(__i386__) || defined(_M_IX86)
print("INFO:arch[i386]");
#elif defined(__ia64) || defined(__ia64__) || defined(_M_IA64)
print("INFO:arch[ia64]");
#elif defined(__loongarch64)
print("INFO:arch[loongarch64]");
#elif defined(__loongarch__)
print("INFO:arch[loongarch32]");
#elif defined(__m68k__)
print("INFO:arch[m68k]");
#elif defined(__mips64) || defined(__mips64__)
#  if defined(_MIPSEL)
print("INFO:arch[mips64el]");
#  else
print("INFO:arch[mips64]");
#  endif
#elif defined(__mips) || defined(__mips__)
#  if defined(_MIPSEL)
print("INFO:arch[mipsel]");
#  else
print("INFO:arch[mips]");
#  endif
#elif defined(__riscv) && __riscv_xlen == 64
print("INFO:arch[riscv64]");
#elif defined(__riscv) && __riscv_xlen == 32
print("INFO:arch[riscv32]");
#elif defined(__s390x__)
print("INFO:arch[s390x]");
#elif defined(__s390__)
print("INFO:arch[s390]");
#elif defined(__sparcv9) || defined(__sparcv9__) || defined(__sparc64__)
print("INFO:arch[sparcv9]");
#elif defined(__sparc) || defined(__sparc__)
print("INFO:arch[sparc]");
#elif defined(__hppa) || defined(__hppa__)
#  if defined(__LP64__)
print("INFO:arch[parisc64]");
#  else
print("INFO:arch[parisc]");
#  endif
#elif defined(__ppc64__) || defined(__powerpc64__) || defined(__PPC64__) || defined(_ARCH_PPC64)
#  if defined(_LITTLE_ENDIAN) || defined(__LITTLE_ENDIAN__)
print("INFO:arch[ppc64le]");
#  else
print("INFO:arch[ppc64]");
#  endif
#elif defined(__ppc__) || defined(__powerpc__) || defined(__PPC__) || defined(_ARCH_PPC)
#  if defined(_LITTLE_ENDIAN) || defined(__LITTLE_ENDIAN__)
print("INFO:arch[ppcle]");
#  else
print("INFO:arch[ppc]");
#  endif
#endif

}
