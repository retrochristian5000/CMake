#include <cstdint>
#include <iostream>

#ifdef _WIN32
#  include <cstdlib>

#  include <fcntl.h> // for _O_BINARY
#  include <io.h>    // for _setmode
#endif

int main()
{
#ifdef _WIN32
  _setmode(fileno(stdout), _O_BINARY);
#endif

  for (std::uint64_t i = 0; i < JACK_COUNT; i++) {
    std::cout
      << " All   work   and   no   play   makes   Jack   a   dull   boy. \n";
  }

  return 0;
}
