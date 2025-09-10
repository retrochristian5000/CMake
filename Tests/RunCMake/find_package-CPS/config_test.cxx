#ifndef NAME
#  error NAME is not defined
#endif
#ifndef MARKER
#  error MARKER is not defined
#endif
#ifndef EXPECTED_MARKER
#  error EXPECTED_MARKER is not defined
#endif

#define STRINGIFY_IMPL(x) #x
#define STRINGIFY(x) STRINGIFY_IMPL(x)

#if EXPECTED_MARKER > 0
static_assert(MARKER + 0 == EXPECTED_MARKER,
              "MARKER has unexpected value " STRINGIFY(MARKER));
static_assert(MARKER + 0 == EXPECTED_MARKER, "CONFIG is " STRINGIFY(NAME));
#endif

#ifdef _WIN32
__declspec(dllexport)
#endif
void dummy_symbol()
{
}
