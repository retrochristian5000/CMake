#ifdef VERIFY_DEFAULT
#  define TARGET_SYMBOL defaultLinkIncremental
#elif defined(VERIFY_FORCE)
#  define TARGET_SYMBOL forceLinkIncremental
#elif defined(VERIFY_NEVER)
#  define TARGET_SYMBOL neverLinkIncremental
#endif

// The symbol that TARGET_SYMBOL represents is never defined,
// but the linker flags in our override rules will tell the linker
// to bind from TARGET_SYMBOL to incrementalLink.
#ifdef __cplusplus
extern "C" {
#endif

void TARGET_SYMBOL();

void linkIncremental()
{
}

#ifdef __cplusplus
}
#endif

int main()
{
  TARGET_SYMBOL();
  return 100;
}
