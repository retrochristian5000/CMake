cmake_minimum_required(VERSION 3.22)
if(POLICY CMP0126)
  cmake_policy(SET CMP0126 NEW)
endif()

# a simple CSharp only test case
project (DotNetSdk CSharp)

set(CMAKE_DOTNET_SDK "Microsoft.NET.Sdk")

add_library(dotNetSdkLib1 SHARED lib1.cs)

add_executable(DotNetSdk csharponly.cs)
target_link_libraries(DotNetSdk dotNetSdkLib1)
