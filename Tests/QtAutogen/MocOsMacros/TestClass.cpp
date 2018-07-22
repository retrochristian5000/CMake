#include "TestClass.hpp"
#include <iostream>

// -- Mac
#ifndef Q_OS_MAC
void TestClass::MacNotDef()
{
  std::cout << "MacNotDef\n";
}
#else
void TestClass::MacNotDefElse()
{
  std::cout << "MacNotDefElse\n";
}
#endif

#ifdef Q_OS_MAC
void TestClass::MacDef()
{
  std::cout << "MacDef\n";
}
#else
void TestClass::MacDefElse()
{
  std::cout << "MacDefElse\n";
}
#endif

// -- Linux
#ifndef Q_OS_LINUX
void TestClass::LinuxNotDef()
{
  std::cout << "LinuxNotDef\n";
}
#else
void TestClass::LinuxNotDefElse()
{
  std::cout << "LinuxNotDefElse\n";
}
#endif

#ifdef Q_OS_LINUX
void TestClass::LinuxDef()
{
  std::cout << "LinuxDef\n";
}
#else
void TestClass::LinuxDefElse()
{
  std::cout << "LinuxDefElse\n";
}
#endif

// -- Windows
#ifndef Q_OS_WINDOWS
void TestClass::WindowsNotDef()
{
  std::cout << "WindowsNotDef\n";
}
#else
void TestClass::WindowsNotDefElse()
{
  std::cout << "WindowsNotDefElse\n";
}
#endif

#ifdef Q_OS_WINDOWS
void TestClass::WindowsDef()
{
  std::cout << "WindowsDef\n";
}
#else
void TestClass::WindowsDefElse()
{
  std::cout << "WindowsDefElse\n";
}
#endif
