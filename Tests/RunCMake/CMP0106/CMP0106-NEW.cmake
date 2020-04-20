cmake_policy(SET CMP0106 NEW)

include(CMP0106-Common.cmake)
if (DEFINED BUILD_DOCUMENTATION OR
    DEFINED DOCUMENTATION_HTML_HELP OR
    DEFINED DOCUMENTATION_HTML_TARZ)
  message(FATAL_ERROR
    "Cache variables seem to have been made with a `NEW` policy "
    "setting.")
endif ()
