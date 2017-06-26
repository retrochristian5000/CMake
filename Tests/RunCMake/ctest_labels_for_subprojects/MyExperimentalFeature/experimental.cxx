#include <iostream>
int main(int argc, char* argv[])
{
  int i = 0;
  if (i > 0) {
    std::cout << "This doesn't happen." << std::endl;
    std::cout << "Neither does this." << std::endl;
  }
  i = i + 1;
  if (i > 0) {
    std::cout << "This does happen." << std::endl;
  }

  return 0;
}
