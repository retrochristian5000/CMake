extern void orderD_poison(void);

void orderA(void)
{
  orderD_poison();
}

void orderB(void)
{
  orderD_poison();
}

void orderC(void)
{
  orderD_poison();
}
