extern void orderC_poison(void);

void orderA(void)
{
  orderC_poison();
}

void orderB(void)
{
  orderC_poison();
}
