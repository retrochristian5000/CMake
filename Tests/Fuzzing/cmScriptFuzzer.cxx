/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file LICENSE.rst or https://cmake.org/licensing for details.  */

/*
 * Fuzzer for CMake script execution
 *
 * This fuzzer executes CMake scripts in script mode (-P).
 * This exercises the majority of CMake's codebase including:
 * - All built-in commands
 * - Variable expansion
 * - Control flow (if, foreach, while, function, macro)
 * - String/list/file operations
 * - Generator expressions
 *
 * This is the highest-impact fuzzer for coverage.
 *
 * Performance notes:
 * - Uses memfd_create on Linux for memory-backed file I/O
 * - Falls back to temp files on other platforms
 */

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

#include <unistd.h>

#include "cmCMakePolicyCommand.h"
#include "cmExecutionStatus.h"
#include "cmGlobalGenerator.h"
#include "cmMakefile.h"
#include "cmMessenger.h"
#include "cmState.h"
#include "cmStateSnapshot.h"
#include "cmSystemTools.h"
#include "cmake.h"

#ifdef __linux__
#  include <sys/mman.h>
#  ifndef MFD_CLOEXEC
#    define MFD_CLOEXEC 0x0001U
#  endif
#endif

static constexpr size_t kMaxInputSize = 256 * 1024;
static std::string g_testDir;
static std::string g_scriptFile;
static bool g_useMemfd = false;

extern "C" int LLVMFuzzerInitialize(int* argc, char*** argv)
{
  (void)argc;
  (void)argv;

  // Suppress output during fuzzing (set once at init)
  cmSystemTools::SetMessageCallback(
    [](std::string const&, cmMessageMetadata const&) {});
  cmSystemTools::SetStdoutCallback([](std::string const&) {});
  cmSystemTools::SetStderrCallback([](std::string const&) {});

#ifdef __linux__
  // Try to use memfd for better performance
  int fd = memfd_create("cmake_fuzz", MFD_CLOEXEC);
  if (fd >= 0) {
    g_useMemfd = true;
    // Create path via /proc/self/fd
    g_scriptFile = "/proc/self/fd/" + std::to_string(fd);
    // Keep fd open - will be reused
    // Create minimal test directory for cmake
    g_testDir = "/tmp";
  } else
#endif
  {
    // Fallback to temp file
    char tmpl[] = "/tmp/cmake_fuzz_script_XXXXXX";
    char* dir = mkdtemp(tmpl);
    if (dir) {
      g_testDir = dir;
    } else {
      g_testDir = "/tmp/cmake_fuzz_script";
      cmSystemTools::MakeDirectory(g_testDir);
    }
    g_scriptFile = g_testDir + "/fuzz_script.cmake";
  }

  return 0;
}

extern "C" int LLVMFuzzerTestOneInput(uint8_t const* data, size_t size)
{
  if (size == 0 || size > kMaxInputSize) {
    return 0;
  }

#ifdef __linux__
  if (g_useMemfd) {
    // Extract fd from path and write directly
    int fd = std::atoi(g_scriptFile.c_str() + 14); // "/proc/self/fd/"
    ftruncate(fd, 0);
    lseek(fd, 0, SEEK_SET);
    if (write(fd, data, size) != static_cast<ssize_t>(size)) {
      return 0;
    }
  } else
#endif
  {
    // Write script to temp file
    FILE* fp = fopen(g_scriptFile.c_str(), "wb");
    if (!fp)
      return 0;
    fwrite(data, 1, size, fp);
    fclose(fp);
  }

  // Create cmake instance for script mode
  cmake cm(cmState::Role::Script);
  cm.SetHomeDirectory(g_testDir);
  cm.SetHomeOutputDirectory(g_testDir);

  // Run the script
  std::vector<std::string> args;
  args.push_back("cmake");
  args.push_back("-P");
  args.push_back(g_scriptFile);

  (void)cm.Run(args, false);

  // Cleanup temp file (memfd doesn't need cleanup)
  if (!g_useMemfd) {
    unlink(g_scriptFile.c_str());
  }

  return 0;
}
