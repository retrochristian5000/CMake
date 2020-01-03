
#ifdef _WIN32
#  define IMPORT __declspec(dllimport)
#else
#  define IMPORT
#endif

int curand_main();
int nppif_main();

int mixed_version()
{
  return curand_main() == 0 && nppif_main() == 0;
}
