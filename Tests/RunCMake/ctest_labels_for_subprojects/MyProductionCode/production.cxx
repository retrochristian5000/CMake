#include <iostream>
int main(int argc, char* argv[])
{
  int j = 0;
  if (j > 0) {
    std::cout << "This doesn't happen." << std::endl;
    std::cout << "Neither does this." << std::endl;
  }
  j = j + 1;
  if (j > 0) {
    std::cout << "This does happen." << std::endl;
  }

  return 0;
}
