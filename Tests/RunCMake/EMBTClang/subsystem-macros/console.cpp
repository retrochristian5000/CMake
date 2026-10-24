// A console target must be compiled with __CONSOLE__ (-tC).
#ifndef __CONSOLE__
#  error "console target compiled without __CONSOLE__"
#endif
int main()
{
  return 0;
}
