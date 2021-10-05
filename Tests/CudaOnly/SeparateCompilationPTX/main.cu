
#include <iostream>

#include <cuda.h>

#include "embedded_objs.h"

int main()
{

  CUmodule module;

  cuInit(0);
  int count = 0;
  cuDeviceGetCount(&count);
  if (count == 0) {
    std::cerr << "No CUDA devices found\n";
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
