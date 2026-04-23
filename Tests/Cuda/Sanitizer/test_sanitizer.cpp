#include <cstring>
#include <iostream>

#include <generated_cuda_runtime_api_meta.h>
#include <sanitizer.h>

namespace {

bool malloc_called = false;
bool free_called = false;

void sanitizer_callback(void* userdata, Sanitizer_CallbackDomain domain,
                        Sanitizer_CallbackId cbid, void const* cbdata)
{
  if (domain == SANITIZER_CB_DOMAIN_RUNTIME_API) {
    auto const* runtime_cbdata =
      static_cast<Sanitizer_CallbackData const*>(cbdata);
    if (runtime_cbdata->callbackSite == SANITIZER_API_ENTER) {
      if (!strcmp(runtime_cbdata->functionName, "cudaMalloc")) {
        malloc_called = true;
      } else if (!strcmp(runtime_cbdata->functionName, "cudaFree")) {
        free_called = true;
      }
    }
  }
}

}

#define SANITIZER_TRY(call)                                                   \
  do {                                                                        \
    SanitizerResult result;                                                   \
    if ((result = (call)) != SANITIZER_SUCCESS) {                             \
      const char* result_str;                                                 \
      sanitizerGetResultString(result, &result_str);                          \
      std::cerr << __FILE__ << ":" << __LINE__ << ": " << result_str << "\n"; \
      return 1;                                                               \
    }                                                                         \
  } while (0)

#define CUDA_TRY(call)                                                        \
  do {                                                                        \
    cudaError_t result;                                                       \
    if ((result = (call)) != cudaSuccess) {                                   \
      std::cerr << __FILE__ << ":" << __LINE__ << ": "                        \
                << cudaGetErrorName(result) << "\n";                          \
      return 1;                                                               \
    }                                                                         \
  } while (0)

int main()
{
  Sanitizer_SubscriberHandle handle;
  SANITIZER_TRY(sanitizerSubscribe(&handle, sanitizer_callback, nullptr));
  SANITIZER_TRY(
    sanitizerEnableDomain(1, handle, SANITIZER_CB_DOMAIN_RUNTIME_API));

  void* dev_ptr;
  CUDA_TRY(cudaMalloc(&dev_ptr, 10));
  if (!malloc_called) {
    std::cerr << "cudaMalloc() did not invoke callback\n";
    return 1;
  }

  CUDA_TRY(cudaFree(dev_ptr));
  if (!free_called) {
    std::cerr << "cudaFree() did not invoke callback\n";
    return 1;
  }

  SANITIZER_TRY(sanitizerUnsubscribe(handle));
  return 0;
}
