/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#ifndef cmFSPermissions_h
#define cmFSPermissions_h

#include "cm_sys_stat.h"

#include <string>

namespace cmFSPermissions {

// Table of permissions flags.
#if defined(_WIN32) && !defined(__CYGWIN__)
constexpr mode_t mode_owner_read = S_IREAD;
constexpr mode_t mode_owner_write = S_IWRITE;
constexpr mode_t mode_owner_execute = S_IEXEC;
constexpr mode_t mode_group_read = 040;
constexpr mode_t mode_group_write = 020;
constexpr mode_t mode_group_execute = 010;
constexpr mode_t mode_world_read = 04;
constexpr mode_t mode_world_write = 02;
constexpr mode_t mode_world_execute = 01;
constexpr mode_t mode_setuid = 04000;
constexpr mode_t mode_setgid = 02000;
#else
constexpr mode_t mode_owner_read = S_IRUSR;
constexpr mode_t mode_owner_write = S_IWUSR;
constexpr mode_t mode_owner_execute = S_IXUSR;
constexpr mode_t mode_group_read = S_IRGRP;
constexpr mode_t mode_group_write = S_IWGRP;
constexpr mode_t mode_group_execute = S_IXGRP;
constexpr mode_t mode_world_read = S_IROTH;
constexpr mode_t mode_world_write = S_IWOTH;
constexpr mode_t mode_world_execute = S_IXOTH;
constexpr mode_t mode_setuid = S_ISUID;
constexpr mode_t mode_setgid = S_ISGID;
#endif

bool stringToModeT(std::string const& arg, mode_t& permissions);

} // ns

#endif
