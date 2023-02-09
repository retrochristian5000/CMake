check_source_compiles(C
  [=[
  #include <stdlib.h>
  #include <stdnoreturn.h>
  noreturn void f(){ exit(0); }
  int main(void) { f(); return 1; }
  ]=]
  HAVE_NORETURN)
