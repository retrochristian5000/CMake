/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#include "cmStringAlgorithms.h"

#include <algorithm>
#include <cstdio>

cmAlphaNum::cmAlphaNum(int val)
{
  int res = std::snprintf(Digits_, sizeof(Digits_), "%i", val);
  if (res > 0) {
    View_ = cm::string_view(Digits_, static_cast<std::size_t>(res));
  }
}

cmAlphaNum::cmAlphaNum(unsigned int val)
{
  int res = std::snprintf(Digits_, sizeof(Digits_), "%u", val);
  if (res > 0) {
    View_ = cm::string_view(Digits_, static_cast<std::size_t>(res));
  }
}

cmAlphaNum::cmAlphaNum(long int val)
{
  int res = std::snprintf(Digits_, sizeof(Digits_), "%li", val);
  if (res > 0) {
    View_ = cm::string_view(Digits_, static_cast<std::size_t>(res));
  }
}

cmAlphaNum::cmAlphaNum(unsigned long int val)
{
  int res = std::snprintf(Digits_, sizeof(Digits_), "%lu", val);
  if (res > 0) {
    View_ = cm::string_view(Digits_, static_cast<std::size_t>(res));
  }
}

cmAlphaNum::cmAlphaNum(long long int val)
{
  int res = std::snprintf(Digits_, sizeof(Digits_), "%lli", val);
  if (res > 0) {
    View_ = cm::string_view(Digits_, static_cast<std::size_t>(res));
  }
}

cmAlphaNum::cmAlphaNum(unsigned long long int val)
{
  int res = std::snprintf(Digits_, sizeof(Digits_), "%llu", val);
  if (res > 0) {
    View_ = cm::string_view(Digits_, static_cast<std::size_t>(res));
  }
}

cmAlphaNum::cmAlphaNum(float val)
{
  int res = std::snprintf(Digits_, sizeof(Digits_), "%g", val);
  if (res > 0) {
    View_ = cm::string_view(Digits_, static_cast<std::size_t>(res));
  }
}

cmAlphaNum::cmAlphaNum(double val)
{
  int res = std::snprintf(Digits_, sizeof(Digits_), "%g", val);
  if (res > 0) {
    View_ = cm::string_view(Digits_, static_cast<std::size_t>(res));
  }
}

std::string cmCatViews(std::initializer_list<cm::string_view> views)
{
  std::size_t total_size = 0;
  for (cm::string_view const& view : views) {
    total_size += view.size();
  }

  std::string result(total_size, '\0');
  std::string::iterator sit = result.begin();
  for (cm::string_view const& view : views) {
    sit = std::copy_n(view.data(), view.size(), sit);
  }
  return result;
}
