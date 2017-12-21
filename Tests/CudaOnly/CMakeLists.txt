
ADD_TEST_MACRO(CudaOnly.EnableStandard CudaOnlyEnableStandard)
ADD_TEST_MACRO(CudaOnly.ExportPTX CudaOnlyExportPTX)
if(CMAKE_CUDA_COMPILER_VERSION VERSION_GREATER_EQUAL 9.0.0)
  #CUDA's __CUDACC_DEBUG__ define was added in 9.0
  ADD_TEST_MACRO(CudaOnly.GPUDebugFlag CudaOnlyGPUDebugFlag)
endif()
ADD_TEST_MACRO(CudaOnly.ResolveDeviceSymbols CudaOnlyResolveDeviceSymbols)
ADD_TEST_MACRO(CudaOnly.SeparateCompilation CudaOnlySeparateCompilation)
ADD_TEST_MACRO(CudaOnly.WithDefs CudaOnlyWithDefs)
