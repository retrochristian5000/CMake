cmake_minimum_required(VERSION 3.4)
project(TestFindVulkan C CXX)
include(CTest)

SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/../../../Modules)
find_package(Vulkan REQUIRED
  COMPONENTS
    glslang
    shaderc_combined
    SPIRV-Tools)

add_executable(test_tgt main.c)
target_link_libraries(test_tgt Vulkan::Vulkan)
add_test(NAME test_tgt COMMAND test_tgt)

add_executable(test_var main.c)
target_include_directories(test_var PRIVATE ${Vulkan_INCLUDE_DIRS})
target_link_libraries(test_var PRIVATE ${Vulkan_LIBRARIES})
add_test(NAME test_var COMMAND test_var)

add_executable(test_tgt_dl main-dynamicVulkanLoading.cpp)
target_link_libraries(test_tgt_dl Vulkan::Headers ${CMAKE_DL_LIBS})
add_test(NAME test_tgt_dl COMMAND test_tgt_dl)

add_executable(test_var_dl main-dynamicVulkanLoading.cpp)
target_include_directories(test_var_dl PRIVATE ${Vulkan_INCLUDE_DIRS})
target_link_libraries(test_var_dl ${CMAKE_DL_LIBS})
add_test(NAME test_var_dl COMMAND test_var_dl)

add_executable(test_tgt_glslang main-glslang.cxx)
target_link_libraries(test_tgt_glslang Vulkan::glslang)
add_test(NAME test_tgt_glslang COMMAND test_tgt_glslang)

add_executable(test_tgt_shaderc_combined main-shaderc_combined.c)
target_link_libraries(test_tgt_shaderc_combined Vulkan::shaderc_combined)
add_test(NAME test_tgt_shaderc_combined COMMAND test_tgt_shaderc_combined)

add_executable(test_tgt_SPIRV-Tools main-SPIRV-Tools.c)
target_link_libraries(test_tgt_SPIRV-Tools Vulkan::SPIRV-Tools)
add_test(NAME test_tgt_SPIRV-Tools COMMAND test_tgt_SPIRV-Tools)

if(Vulkan_GLSLC_EXECUTABLE)
  add_test(NAME test_glslc
    COMMAND ${CMAKE_COMMAND}
    "-DVULKAN_GLSLC_EXECUTABLE=${Vulkan_GLSLC_EXECUTABLE}"
    "-DVULKAN_GLSLC_EXECUTABLE_TARGET=$<TARGET_FILE:Vulkan::glslc>"
    -P "${CMAKE_CURRENT_LIST_DIR}/Run-glslc.cmake"
    )
endif()

if(Vulkan_GLSLANG_VALIDATOR_EXECUTABLE)
  add_test(NAME test_glslangValidator
    COMMAND ${CMAKE_COMMAND}
    "-DVULKAN_GLSLANG_VALIDATOR_EXECUTABLE=${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE}"
    "-DVULKAN_GLSLANG_VALIDATOR_EXECUTABLE_TARGET=$<TARGET_FILE:Vulkan::glslangValidator>"
    -P "${CMAKE_CURRENT_LIST_DIR}/Run-glslangValidator.cmake"
    )
endif()
