extern
#ifdef _WIN32
__declspec(dllimport)
#endif
int answer();

#ifdef _WIN32
__declspec(dllexport)
#endif
int ask()
{
  return answer();
}
