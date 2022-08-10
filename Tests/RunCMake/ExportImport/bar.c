#if defined(_WIN32)
__declspec(dllimport)
#endif
  int foo();

#ifdef _WIN32
__declspec(dllexport)
#endif
  int bar()
{
  return foo();
}
