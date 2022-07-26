/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#pragma once

#include "cmConfigure.h" // IWYU pragma: keep

namespace ArgumentParser {

template <typename T>
struct Maybe : public T
{
  using T::T;
};

template <typename T>
struct MaybeEmpty : public T
{
  using T::T;
};

template <typename T>
struct NonEmpty : public T
{
  using T::T;
};

} // namespace ArgumentParser
