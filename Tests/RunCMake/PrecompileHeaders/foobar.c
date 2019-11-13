#include "bar.h"
#include "foo.h"
#include "foo2.h"

int main()
{
  printf("");
  return foo() + foo2() + bar();
}
