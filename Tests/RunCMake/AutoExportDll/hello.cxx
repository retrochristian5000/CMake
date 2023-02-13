#include "hello.h"

#include <stdio.h>
int Hello::Data = 0;
void Hello::real()
{
  return;
}
void hello()
{
  printf("hello");
}
void Hello::operator delete[](void*){};
void Hello::operator delete(void*){};

VirtualHello::VirtualHello()
{
}
VirtualHello::~VirtualHello()
{
}
int VirtualHello::getI()
{
  return 10;
}
