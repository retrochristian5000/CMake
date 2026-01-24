/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file LICENSE.rst or https://cmake.org/licensing for details.  */
#include "cmVersion.h"

#if !defined(CMAKE_BOOTSTRAP)
#  include <cstddef>
#  include <string>
#  include <utility>
#  include <vector>

#  include <cm3p/archive.h>
#  include <cm3p/curl/curl.h>
#  include <cm3p/expat.h>
#  include <cm3p/json/version.h>
#  include <cm3p/kwiml/version.h>

#  include "cmThirdParty.h" // IWYU pragma: keep

#  ifdef CMAKE_USE_SYSTEM_LIBRHASH
#    include <cstdint>

#    include <cm3p/rhash.h>
#  endif
#  include <cm3p/uv.h>
#  include <cm3p/zlib.h>

#  ifdef CMAKE_USE_OPENSSL
#    include <openssl/crypto.h>
#  endif

#  include "cmStringAlgorithms.h"

std::vector<cmVersion::DependencyInfo> const&
cmVersion::CollectDependencyInfo()
{
  static std::vector<DependencyInfo> deps;
  if (!deps.empty()) {
    return deps;
  }

  // BZIP2
  {
    DependencyInfo info;
    info.name = "bzip2";
    info.version = archive_bzlib_version();
    size_t pos = info.version.find(',');
    if (pos != std::string::npos) {
      // Convert `1.0.8, 30-Mar-2009` to `1.0.8`
      info.version.erase(pos);
    }
#  ifdef CMAKE_USE_SYSTEM_BZIP2
    info.type = DependencyType::System;
#  else
    info.type = DependencyType::Bundled;
#  endif
    deps.emplace_back(std::move(info));
  }

  // CPPDAP
  {
    DependencyInfo info;
    info.name = "cppdap";
#  ifdef CMAKE_USE_SYSTEM_CPPDAP
    info.type = DependencyType::System;
    // Cannot get runtime version from cppdap library
#  else
    info.type = DependencyType::Bundled;
    // Hardcoded in protocol.h header file comments
    info.version = "1.65.0";
#  endif
    deps.emplace_back(std::move(info));
  }

  // CURL
  {
    DependencyInfo info;
    info.name = "curl";
    curl_version_info_data* curlVersion = curl_version_info(CURLVERSION_NOW);
    if (curlVersion && curlVersion->version) {
      info.version = curlVersion->version;
    }
#  ifdef CMAKE_USE_SYSTEM_CURL
    info.type = DependencyType::System;
#  else
    info.type = DependencyType::Bundled;
#  endif
    deps.emplace_back(std::move(info));

    // NGHTTP2
    {
      DependencyInfo infoNghttp2;
      infoNghttp2.name = "nghttp2";
      if (curlVersion && curlVersion->nghttp2_version) {
        infoNghttp2.version = curlVersion->nghttp2_version;
      }
#  ifdef CMAKE_USE_SYSTEM_NGHTTP2
      infoNghttp2.type = DependencyType::System;
#  else
      infoNghttp2.type = DependencyType::Bundled;
#  endif
      deps.emplace_back(std::move(infoNghttp2));
    }
  }

  // EXPAT
  {
    DependencyInfo info;
    info.name = "expat";

    XML_Expat_Version version = XML_ExpatVersionInfo();
    info.version =
      cmStrCat(version.major, '.', version.minor, '.', version.micro);
#  ifdef CMAKE_USE_SYSTEM_EXPAT
    info.type = DependencyType::System;
#  else
    info.type = DependencyType::Bundled;
#  endif
    deps.emplace_back(std::move(info));
  }

  // FORM
  {
    DependencyInfo info;
    info.name = "form";
    // Cannot get any version from form library
#  ifdef CMAKE_USE_SYSTEM_FORM
    info.type = DependencyType::System;
#  else
    info.type = DependencyType::Bundled;
#  endif
    deps.emplace_back(std::move(info));
  }

  // JSONCPP
  {
    DependencyInfo info;
    info.name = "jsoncpp";
    info.version = JSONCPP_VERSION_STRING;
#  ifdef CMAKE_USE_SYSTEM_JSONCPP
    info.type = DependencyType::System;
#  else
    info.type = DependencyType::Bundled;
#  endif
    deps.emplace_back(std::move(info));
  }

  // KWIML
  {
    DependencyInfo info;
    info.name = "kwiml";

    // Library is header-only, so we can safely use the defined version
    info.version = KWIML_VERSION_STRING;

#  ifdef CMAKE_USE_SYSTEM_KWIML
    info.type = DependencyType::System;
#  else
    info.type = DependencyType::Bundled;
#  endif
    deps.emplace_back(std::move(info));
  }

  // LIBARCHIVE
  {
    DependencyInfo info;
    info.name = "libarchive";
    info.version = archive_version_string();
    if (cmHasLiteralPrefix(info.version, "libarchive ")) {
      info.version.erase(0, 11);
    }
#  ifdef CMAKE_USE_SYSTEM_LIBARCHIVE
    info.type = DependencyType::System;
#  else
    info.type = DependencyType::Bundled;
#  endif
    deps.emplace_back(std::move(info));
  }

  // LIBLZMA
  {
    DependencyInfo info;
    info.name = "liblzma";
    info.version = archive_liblzma_version();
#  ifdef CMAKE_USE_SYSTEM_LIBLZMA
    info.type = DependencyType::System;
#  else
    info.type = DependencyType::Bundled;
#  endif
    deps.emplace_back(std::move(info));
  }

  // LIBRHASH
  {
    DependencyInfo info;
    info.name = "librhash";
#  ifdef CMAKE_USE_SYSTEM_LIBRHASH
    info.type = DependencyType::System;
    std::uint64_t version = rhash_get_version();
    std::uint8_t major = (version >> 24) & 0xFFu;
    std::uint8_t minor = (version >> 16) & 0xFFu;
    std::uint8_t patch = (version >> 8) & 0xFFu;
    info.version = cmStrCat(major, '.', minor, '.', patch);
#  else
    info.type = DependencyType::Bundled;
    // Hardcoded in `update-librhash.bash` script
    info.version = "1.4.4";
#  endif
    deps.emplace_back(std::move(info));
  }

  // LIBUV
  {
    DependencyInfo info;
    info.name = "libuv";
    info.version = uv_version_string();
#  ifdef CMAKE_USE_SYSTEM_LIBUV
    info.type = DependencyType::System;
#  else
    info.type = DependencyType::Bundled;
#  endif
    deps.emplace_back(std::move(info));
  }

#  ifdef CMAKE_USE_OPENSSL
  // OPENSSL
  {
    DependencyInfo info;
    info.name = "openssl";
    info.version = OpenSSL_version(OPENSSL_VERSION_STRING);
    // Bundled version of OpenSSL is not presented
    info.type = DependencyType::System;
    deps.emplace_back(std::move(info));
  }
#  endif

  // ZLIB
  {
    DependencyInfo info;
    info.name = "zlib";
    info.version = zlibVersion();
#  ifdef CMAKE_USE_SYSTEM_ZLIB
    info.type = DependencyType::System;
#  else
    info.type = DependencyType::Bundled;
#  endif
    deps.emplace_back(std::move(info));
  }

  // ZSTD
  {
    DependencyInfo info;
    info.name = "zstd";
    info.version = archive_libzstd_version();
#  ifdef CMAKE_USE_SYSTEM_ZSTD
    info.type = DependencyType::System;
#  else
    info.type = DependencyType::Bundled;
#  endif
    deps.emplace_back(std::move(info));
  }

  return deps;
}

#endif
