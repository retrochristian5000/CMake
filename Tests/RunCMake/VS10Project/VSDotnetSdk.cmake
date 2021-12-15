enable_language(CSharp)

if(NOT CMAKE_CSharp_COMPILER)
    return()
endif()

set(CMAKE_DOTNET_SDK "Microsoft.NET.Sdk")
add_library(foo SHARED foo.cs)

set(CMAKE_DOTNET_SDK "Microsoft.NET.Sdk.Web")
add_library(bar SHARED foo.cs)

set(CMAKE_DOTNET_SDK "")
add_library(baz SHARED foo.cs)
