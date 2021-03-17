include(${CMAKE_CURRENT_LIST_DIR}/FindPackageHandleStandardArgs.cmake)

if (WIN32)
  find_program(UVISION_EXECUTABLE UV4.exe PATHS
    "${CMAKE_GENERATOR_INSTANCE}/UV4"
    "$ENV{ARMROOT}/UV4"
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Keil\\Products\\MDK;Path]/../UV4"
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Keil µVision4;LastInstallDir]/UV4"
    DOC "Program used to build from .uvproj(x) files."
  )
  find_package_handle_standard_args(UVision REQUIRED_VARS UVISION_EXECUTABLE)
endif()