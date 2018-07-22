#ifndef TestClass_hpp
#define TestClass_hpp

#include <QObject>

class TestClass : public QObject
{
  Q_OBJECT
public Q_SLOTS:

// -- MAC
#ifndef Q_OS_MAC
  void MacNotDef();
#else
  void MacNotDefElse();
#endif

#ifdef Q_OS_MAC
  void MacDef();
#else
  void MacDefElse();
#endif

// -- LINUX
#ifndef Q_OS_LINUX
  void LinuxNotDef();
#else
  void LinuxNotDefElse();
#endif

#ifdef Q_OS_LINUX
  void LinuxDef();
#else
  void LinuxDefElse();
#endif

// -- WINDOWS
#ifndef Q_OS_WINDOWS
  void WindowsNotDef();
#else
  void WindowsNotDefElse();
#endif

#ifdef Q_OS_WINDOWS
  void WindowsDef();
#else
  void WindowsDefElse();
#endif
};

#endif /* TestClass_hpp */
