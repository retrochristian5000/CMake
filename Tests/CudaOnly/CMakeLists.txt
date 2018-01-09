
ADD_TEST_MACRO(CudaOnly.EnableStandard CudaOnlyEnableStandard)
ADD_TEST_MACRO(CudaOnly.ExportPTX CudaOnlyExportPTX)
if(NOT APPLE)
  #CUDA device debug doesn't work on OSX
  ADD_TEST_MACRO(CudaOnly.GPUDebugFlag CudaOnlyGPUDebugFlag)
endif()
ADD_TEST_MACRO(CudaOnly.ResolveDeviceSymbols CudaOnlyResolveDeviceSymbols)
ADD_TEST_MACRO(CudaOnly.SeparateCompilation CudaOnlySeparateCompilation)
ADD_TEST_MACRO(CudaOnly.WithDefs CudaOnlyWithDefs)
