#include <stdlib.h>

void __global__ kernel(int* in1, int* in2, int* out)
{
  size_t idx = blockIdx.x * blockDim.x + threadIdx.x;
  out[idx] = in1[idx] + in2[idx];
}
