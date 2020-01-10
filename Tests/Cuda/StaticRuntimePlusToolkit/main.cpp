
#ifdef _WIN32
#  define IMPORT __declspec(dllimport)
#else
#  define IMPORT
#endif

IMPORT int shared_version();
IMPORT int static_version();
IMPORT int mixed_version();

int main()
{
  return mixed_version() == 0 && shared_version() == 0 &&
    static_version() == 0;
}
