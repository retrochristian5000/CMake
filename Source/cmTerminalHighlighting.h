/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */

#pragma once

struct terminal_colors
{
#ifdef __unix__
  constexpr static char red[] = "\033[31m";
  constexpr static char yellow[] = "\033[33m";
  constexpr static char reset[] = "\033[39m";
#else
  constexpr static char red[] = "";
  constexpr static char yellow[] = "";
  constexpr static char reset[] = "";
#endif
};

struct terminal_effects
{
#ifdef __unix__
  constexpr static char bold[] = "\033[1m";
  constexpr static char reset[] = "\033[0m";
#else
  constexpr static char bold[] = "";
  constexpr static char reset[] = "";
#endif
};
