
#if defined(_WIN32)
__declspec(dllimport)
#endif
  void base();

void lib()
{
  base();
}
