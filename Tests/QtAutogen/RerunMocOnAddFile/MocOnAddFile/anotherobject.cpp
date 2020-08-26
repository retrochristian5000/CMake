#include <qobject.h>

class AnotherObject : public QObject
{
  Q_OBJECT
public:
  AnotherObject(QObject* parent = nullptr) {}
};

AnotherObject* createAnotherObject()
{
  return new AnotherObject();
}

#include "anotherobject.moc"
