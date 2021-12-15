cmake_minimum_required(VERSION 3.22)

# a simple CSharp only test case
project (DotNetSdk CSharp)

set(CMAKE_DOTNET_SDK "Microsoft.NET.Sdk")

add_library(dotNetSdkLib1 SHARED lib1.cs)

add_executable(DotNetSdk csharponly.cs)
target_link_libraries(DotNetSdk dotNetSdkLib1)
