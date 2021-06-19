#include <cstdio>

int main()
{
#if defined(__clang__)
  std::printf("__clang__\n");
#else
  std::printf("!__clang__\n");
#endif
#if defined(__GNUC__)
  std::printf("__GNUC__\n");
#else
  std::printf("!__GNUC__\n");
#endif
#if defined(__SUNPRO_CC)
  std::printf("__SUNPRO_CC\n");
#else
  std::printf("!__SUNPRO_CC\n");
#endif
#if defined(__STRICT_ANSI__)
  std::printf("__STRICT_ANSI__\n");
#else
  std::printf("!__STRICT_ANSI__\n");
#endif
  std::printf("__cplusplus=%u\n\n", __cplusplus);

#if (defined(__clang__) || defined(__GNUC__) || defined(__SUNPRO_CC)) && !defined(__STRICT_ANSI__)
  std::printf("GNU ");
#endif

  std::printf("C++"
#if __cplusplus > 202002L
              "23"
#elif __cplusplus > 201703L
              "20"
#elif __cplusplus >= 201703L
              "17"
#elif __cplusplus >= 201402L
              "14"
#elif __cplusplus >= 201103L
              "11"
#else
              "98"
#endif
              "\n");
}
