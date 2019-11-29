
#ifdef _WIN32
#  define EXPORT __declspec(dllexport)
#else
#  define IMPORT
#endif

int curand_main();
int nppif_main();

int static_version()
{
  return curand_main() == 0 && nppif_main() == 0;
}
