#ifndef __CUDA_ARCH_LIST__
#  error "No architecture specified."
#endif

#include <vector>

int main()
{
  std::vector<int> arch_list = {__CUDA_ARCH_LIST__};
  // When compiling for `all` or `all-major` we should have a long list of values
  if(arch_list.size() <= 1)
  {
    return 1;
  }
  return 0;
}
