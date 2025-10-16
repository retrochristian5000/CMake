include(RunCMake)

run_cmake(CREATE_LINK)
run_cmake(CREATE_LINK-COPY_ON_ERROR-file)
run_cmake(CREATE_LINK-noarg)
run_cmake(CREATE_LINK-noexist)

if(NOT WIN32
    AND NOT MSYS # FIXME: This works on CYGWIN but not on MSYS
    )
  run_cmake(CREATE_LINK-SYMBOLIC)
  run_cmake(CREATE_LINK-SYMBOLIC-noexist)
endif()

run_cmake(CMP0205-WARN)
run_cmake(CMP0205-OLD)
run_cmake(CMP0205-NEW)
