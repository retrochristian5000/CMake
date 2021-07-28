/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#pragma once

/**
 * Enumerate types known to file(INSTALL).
 */
enum cmInstallMode
{
  cmInstallMode_COPY,
  cmInstallMode_SYMLINK,
  cmInstallMode_SYMLINK_OR_COPY
};
