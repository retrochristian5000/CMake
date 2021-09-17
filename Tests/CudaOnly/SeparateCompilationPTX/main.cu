
#include <iostream>

#include <cuda.h>

#include "embedded_objs.h"

int main(int argc, char** argv)
{
  (void)argc;
  (void)argv;

  CUmodule module;

  cuInit(0);
  int count = 0;
  cuDeviceGetCount(&count);
  if (count == 0) {
    std::cerr << "Failed to retrieve the number of CUDA enabled devices"
              << std::endl;
    return 1;
  }

  std::cout << module << std::endl;
  cuModuleLoadData(&module, kernels);
  if (module == nullptr) {
    std::cerr << "Failed to load the embedded ptx" << std::endl;
    return 1;
  }
  std::cout << module << std::endl;

  return 0;
}
