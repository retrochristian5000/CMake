cuda-archs-env
--------------

* Add :envvar:`CUDAARCHS` environment variable for initializing
  :variable:`CMAKE_CUDA_ARCHITECTURES`. Useful in cases where the compiler
  default is unsuitable for the machine's GPU.
