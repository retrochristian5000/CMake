# Autogen tests common for Qt4 and Qt5
ADD_AUTOGEN_TEST(MocOnly mocOnly)
ADD_AUTOGEN_TEST(MocOptions mocOptions)
if(ALLOW_WRAP_CPP)
  ADD_AUTOGEN_TEST(UicOnly uicOnly)
endif()
