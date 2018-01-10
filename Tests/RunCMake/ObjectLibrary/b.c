#if defined(_WIN32) && defined(COMPILE_FOR_SHARED_LIB)
#define EXPORT __declspec(dllexport)
#define IMPORT __declspec(dllimport)
#else
#define EXPORT
#define IMPORT
#endif

extern IMPORT int a(void);
EXPORT int b()
{
  return a();
}
#ifndef REQUIRED
#error "REQUIRED needs to be defined"
#endif
