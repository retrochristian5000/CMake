#if defined(EXPECT_UPDATED)
// /Zc:__cplusplus makes the macro report the selected C++ standard,
// which is at least C++14 for MSVC.
#  if __cplusplus < 201402L
#    error "__cplusplus does not report the selected C++ standard."
#  endif
#  if __cplusplus != _MSVC_LANG
#    error "__cplusplus does not match _MSVC_LANG."
#  endif
#elif defined(EXPECT_LEGACY)
// Without /Zc:__cplusplus the macro always reports C++98.
#  if __cplusplus != 199711L
#    error "__cplusplus does not report the legacy 199711L value."
#  endif
#else
#  error "Neither EXPECT_UPDATED nor EXPECT_LEGACY is defined."
#endif

int main()
{
  return 0;
}
