int foo(int n)
{
  int s = 0;
  int i;
  for (i = 0; i < n; ++i) {
    if (i % 7 == 0) {
      s += i;
    } else {
      s -= 1;
    }
  }
  return s;
}
