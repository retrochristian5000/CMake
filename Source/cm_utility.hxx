/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#ifndef cm_utility_hxx
#define cm_utility_hxx

#if defined(CMake_HAVE_CXX_IN_PLACE)
#  include <utility>
#endif

namespace cm {

#if defined(CMake_HAVE_CXX_IN_PLACE)

using std::in_place_t;
using std::in_place;

#else

struct in_place_t
{
  explicit in_place_t() = default;
};

constexpr in_place_t in_place{};

#endif
}

#endif
