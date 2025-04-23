include(RunCMake)


set(RunCMake_TEST_OPTIONS
  -Wno-dev
  "-DCMAKE_EXPERIMENTAL_GENERATE_SBOM:STRING=ca494ed3-b261-4205-a01f-603c95e4cae0"
)

function(run_cmake_install test)
    set(RunCMake_TEST_BINARY_DIR ${RunCMake_BINARY_DIR}/${test}-build)
    if (NOT RunCMake_GENERATOR_IS_MULTI_CONFIG)
        list(APPEND RunCMake_TEST_OPTIONS -DCMAKE_BUILD_TYPE=Release)
    endif()

    run_cmake(${test})

    set(RunCMake_TEST_NO_CLEAN TRUE)
    run_cmake_command(${test}-build ${CMAKE_COMMAND} --build . --config Release)
    run_cmake_command(${test}-install ${CMAKE_COMMAND} --install . --config Release)
endfunction()

run_cmake_install(Sbom)
