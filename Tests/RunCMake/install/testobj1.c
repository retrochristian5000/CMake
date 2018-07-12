#ifdef _WIN32
__declspec(dllimport)
#endif
  int obj1(void);

int main(int argc, char** argv)
{
  return obj1();
}
