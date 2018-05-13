#include "item.hpp"
#include <QCoreApplication>
int main(int argc, char* argv[])
{
  QCoreApplication app(argc, argv);
  Q_INIT_RESOURCE(example);
  Item item;
  return 0;
}
