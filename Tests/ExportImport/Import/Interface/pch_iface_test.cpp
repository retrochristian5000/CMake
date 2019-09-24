#ifdef EXPECT_PCH
// Verify that pch/pch.h was included via '-include ...' or equivalent.
#  ifndef PCH_PCH_H_INCLUDED
#    error "Expected PCH_PCH_H_INCLUDED."
#  endif
#elif defined(__GNUC__) || defined(__clang__) || defined(_INTEL_COMPILER) ||  \
  defined(_MSC_VER)
#  error "Expected EXPECT_PCH for this compiler."
#endif

int main()
{
  return 0;
}
