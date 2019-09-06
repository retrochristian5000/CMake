
#include "private_slot.h"

class PrivateSlotPrivate
{
public:
  void privateSlot() {}
};

PrivateSlot::PrivateSlot(QObject* parent)
  : QObject(parent)
  , d(new PrivateSlotPrivate)
{
}

#include "moc_private_slot.cpp"
