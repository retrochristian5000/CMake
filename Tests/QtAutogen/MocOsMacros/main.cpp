#include "TestClass.hpp"
#include <QtGlobal>

int main()
{
  TestClass a;
#ifdef Q_OS_MAC
  a.MacNotDefElse();
  a.MacDef();
#else
  a.MacNotDef();
  a.MacDefElse();
#endif

#ifdef Q_OS_LINUX
  a.LinuxNotDefElse();
  a.LinuxDef();
#else
  a.LinuxNotDef();
  a.LinuxDefElse();
#endif

#ifdef Q_OS_WINDOWS
  a.WindowsNotDefElse();
  a.WindowsDef();
#else
  a.WindowsNotDef();
  a.WindowsDefElse();
#endif

  return 0;
}
