#include <utility>
int main()
{
  std::in_place_t ip = std::in_place;
  (void)ip;
  return 0;
}
