int test(int a);
int inner(int a);

int main(int argc, char* argv[])
{
  return test(0) + inner(0);
}
