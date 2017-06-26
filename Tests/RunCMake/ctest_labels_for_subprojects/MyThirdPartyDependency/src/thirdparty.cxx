#include <iostream>

int main(int argc, char* argv[])
{
  std::cout << "This function has an error!" << std::endl;
  n = 5;
  return 0;
}

int notcalled()
{
  std::cout << "This function doesn't get called." << std::endl;
  return 0;
}
