#include <optional>
int main()
{
  std::optional<int> o;
  return o.has_value();
}
