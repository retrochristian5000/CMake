#ifndef A
#  error "'A' not defined"
#endif
#ifndef B
#  error "'B' not defined"
#endif

extern void a(void);
extern void b(void);

int main(void)
{
  a();
  b();
  return 0;
}
