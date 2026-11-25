#include <atomic>

std::atomic<long long> value{ 0 };

int main(int argc, char**)
{
  long long expected = argc;
  value.store(expected);
  if (value.load() != expected || value.fetch_add(1) != expected) {
    return 1;
  }
  ++expected;
  return value.compare_exchange_strong(expected, 0) ? 0 : 1;
}
