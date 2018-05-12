#include "item.hpp"
#include <QCoreApplication>
#include <QTimer>

int main(int argc, char* argv[])
{
  QCoreApplication app(argc, argv);
  Q_INIT_RESOURCE(example);
  Item item;

  QTimer::singleShot(0, &item, SLOT(go()));
  QTimer::singleShot(0, &app, SLOT(quit()));

  return app.exec();
}
