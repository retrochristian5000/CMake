project(Outer SPDX_LICENSE "MIT" USE_SPDX_LICENSE_FOR_TARGETS LANGUAGES NONE)

function(check_prop tgt expected)
  get_property(set TARGET ${tgt} PROPERTY SPDX_LICENSE SET)
  get_property(val TARGET ${tgt} PROPERTY SPDX_LICENSE)
  if(set AND NOT val STREQUAL expected)
    message(SEND_ERROR "${tgt}: expected SPDX_LICENSE '${expected}', got '${val}'")
  endif()
endfunction()

if(NOT PROJECT_USE_SPDX_LICENSE_FOR_TARGETS OR NOT Outer_USE_SPDX_LICENSE_FOR_TARGETS)
  message(SEND_ERROR "variables expected to be ON")
endif()
if(DEFINED CMAKE_PROJECT_USE_SPDX_LICENSE_FOR_TARGETS)
  message(SEND_ERROR "CMAKE_PROJECT_USE_SPDX_LICENSE_FOR_TARGETS must not be set")
endif()

add_library(outer_lib INTERFACE)
check_prop(outer_lib "MIT")

add_library(outer_explicit INTERFACE)
set_property(TARGET outer_explicit PROPERTY SPDX_LICENSE "Apache-2.0")
check_prop(outer_explicit "Apache-2.0")

add_library(outer_imported INTERFACE IMPORTED)
check_prop(outer_imported "<unset>")

add_subdirectory(UseSpdxLicenseForTargets-sub)
check_prop(inherit_lib "MIT")
check_prop(nested_plain_lib "<unset>")
check_prop(nested_opt_lib "BSD-3-Clause")
