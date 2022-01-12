extern void orderB_poison(void);

void orderA(void)
{
  orderB_poison();
}
