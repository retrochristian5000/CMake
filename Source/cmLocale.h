/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file LICENSE.rst or https://cmake.org/licensing for details.  */
#pragma once

#include "cmConfigure.h" // IWYU pragma: keep

#include <clocale>
#include <string>

class cmLocaleRAII
{
public:
  cmLocaleRAII()
    : OldLocale(setlocale(LC_CTYPE, nullptr))
  {
#ifndef _WIN32
    // On Windows, C.UTF-8 maps to C locale, which is worth that
    // user-specified. On non-Windows, try en_US.UTF-8 and C.UTF-8 first:
    // sometimes user defines pure "C" locale without UTF-8 support (such as
    // fedora 43 docker container), and sometimes it specifies the "C.UTF-8"
    // one (such as ubuntu 24.04 on host machine).
    // On macOS 10.11, the locale is "C" by default, so we need to set it to
    // "UTF-8". On CentOS 7, the locale should be set to "en_US.UTF-8" to apply
    // UTF-8 encoding.
    if (setlocale(LC_CTYPE, "UTF-8") != nullptr) {
      return;
    }

    if (setlocale(LC_CTYPE, "en_US.UTF-8") != nullptr) {
      return;
    }

    if (setlocale(LC_CTYPE, "C.UTF-8") != nullptr) {
      return;
    }
#endif

    // User-specified locale is a last hope - should always work
    setlocale(LC_CTYPE, "");
  }

  ~cmLocaleRAII() { setlocale(LC_CTYPE, this->OldLocale.c_str()); }

  cmLocaleRAII(cmLocaleRAII const&) = delete;
  cmLocaleRAII& operator=(cmLocaleRAII const&) = delete;

private:
  std::string OldLocale;
};
