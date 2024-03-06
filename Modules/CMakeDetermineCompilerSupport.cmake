# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

macro(_determine_compile_features language_name standard_list)
  string(TOUPPER "${language_name}" upper_language_name)
  string(TOLOWER "${language_name}" lower_language_name)

  message(CHECK_START "Detecting ${upper_language_name} compile features")

  set(compile_features_names)

  # Unset each CMAKE_<LANG><STANDARD>_COMPILE_FEATURES variable.
  foreach(current_standard ${standard_list})
    set(current_compile_features_name "CMAKE_${upper_language_name}${current_standard}_COMPILE_FEATURES")

    list(APPEND compile_features_names "${current_compile_features_name}")
    unset(${current_compile_features_name})
  endforeach()

  # Call the cmake_record_<lang>_compile_features() macro. This will populate the
  # CMAKE_<LANG><STANDARD>_COMPILE_FEATURES with compile features which are determined to be supported
  # by the current compiler.
  include("${CMAKE_ROOT}/Modules/Internal/FeatureTesting.cmake")
  set(record_compile_features_command "cmake_record_${lower_language_name}_compile_features")

  if(NOT COMMAND "${record_compile_features_command}")
    message(CHECK_FAIL "failed")
    return()
  endif()

  cmake_language(CALL "${record_compile_features_command}")

  # The default cmake_record_<lang>_compile_features() macros will set a variable called _result in this
  # scope. Any non-zero value for this variable indicates a failure. However, it is possible for the
  # default macros to be replaced with a different command. So, we should check to make sure that the
  # variable is defined.
  if(NOT DEFINED _result OR (NOT _result EQUAL 0))
    message(CHECK_FAIL "failed")
    return()
  endif()

  # Remove duplicate compile features from successive CMAKE_<LANG><STANDARD>_COMPILE_FEATURES lists. For
  # example, if the current compiler supports both C++17 and C++23, then both CMAKE_CXX17_COMPILE_FEATURES and
  # CMAKE_CXX23_COMPILE_FEATURES will have features added. In that case, we remove any elements of
  # CMAKE_CXX23_COMPILE_FEATURES which already appear in CMAKE_CXX17_COMPILE_FEATURES.
  set(reversed_standard_list ${standard_list})
  list(REVERSE reversed_standard_list)

  block()
    list(SUBLIST reversed_standard_list 1 -1 lower_reversed_standard_list)

    foreach(higher_standard lower_standard IN ZIP_LISTS reversed_standard_list lower_reversed_standard_list)
      # lower_reversed_standard_list has one less element than reversed_standard_list. So, make sure that we
      # do not perform too many iterations.
      if(NOT DEFINED lower_standard)
        break()
      endif()

      set(higher_standard_compile_features_name "CMAKE_${upper_language_name}${higher_standard}_COMPILE_FEATURES")
      set(lower_standard_compile_features_name "CMAKE_${upper_language_name}${lower_standard}_COMPILE_FEATURES")

      if(${higher_standard_compile_features_name} AND ${lower_standard_compile_features_name})
        list(REMOVE_ITEM ${higher_standard_compile_features_name} ${${lower_standard_compile_features_name}})
      endif()
    endforeach()
  endblock()

  # If the CMAKE_<LANG>_COMPILE_FEATURES variable is not already set, then give it the values of every
  # CMAKE_<LANG><STANDARD>_COMPILE_FEATURES list.
  set(global_compile_features_name "CMAKE_${upper_language_name}_COMPILE_FEATURES")

  if(NOT ${global_compile_features_name})
    set(${global_compile_features_name})

    foreach(standard_compile_features_name IN LISTS compile_features_names)
      list(APPEND ${global_compile_features_name} ${${standard_compile_features_name}})
    endforeach()
  endif()

  # Propagate all of our calculated results to the parent scope.
  set(${global_compile_features_name} ${${global_compile_features_name}} PARENT_SCOPE)

  foreach(current_compile_features_name IN LISTS compile_features_names)
    set(${current_compile_features_name} ${${current_compile_features_name}} PARENT_SCOPE)
  endforeach()

  message(CHECK_PASS "done")
endmacro()

function(cmake_determine_compiler_support lang)
  if("x${lang}" STREQUAL "xC" AND COMMAND cmake_record_c_compile_features)
    set(c_standard_list
      90
      99
      11
      17
      23
    )

    _determine_compile_features(C "${c_standard_list}")
  elseif("x${lang}" STREQUAL "xCXX" AND COMMAND cmake_record_cxx_compile_features)
    set(cxx_standard_list
      98
      11
      14
      17
      20
      23
      26
    )

    _determine_compile_features(CXX "${cxx_standard_list}")
  elseif("x${lang}" STREQUAL "xCUDA" AND COMMAND cmake_record_cuda_compile_features)
    set(cuda_standard_list
      03
      11
      14
      17
      20
      23
      26
    )

    _determine_compile_features(CUDA "${cuda_standard_list}")
  elseif(lang STREQUAL HIP AND COMMAND cmake_record_hip_compile_features)
    set(hip_standard_list
      98
      11
      14
      17
      20
      23
      26
    )

    _determine_compile_features(HIP "${hip_standard_list}")
  endif()
endfunction()
