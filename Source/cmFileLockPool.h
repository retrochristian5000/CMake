/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#ifndef cmFileLockPool_h
#define cmFileLockPool_h

#include "cmConfigure.h" // IWYU pragma: keep

#include <memory>
#include <string>

class cmFileLockResult;

/** \class cmFileLockPool
 */
class cmFileLockPool
{
public:
  cmFileLockPool();
  cmFileLockPool(cmFileLockPool const&) = delete;
  cmFileLockPool(cmFileLockPool&&) noexcept;
  ~cmFileLockPool();

  cmFileLockPool& operator=(cmFileLockPool const&) = delete;
  cmFileLockPool& operator=(cmFileLockPool&&) noexcept;

  //@{
  /**
   * @brief Function scope control.
   */
  void PushFunctionScope();
  void PopFunctionScope();
  //@}

  //@{
  /**
   * @brief File scope control.
   */
  void PushFileScope();
  void PopFileScope();
  //@}

  //@{
  /**
   * @brief Lock the file in given scope.
   * @param timeoutSec Lock timeout. If -1 try until success or fatal error.
   */
  cmFileLockResult LockFunctionScope(std::string const& filename,
                                     unsigned long timeoutSec);
  cmFileLockResult LockFileScope(std::string const& filename,
                                 unsigned long timeoutSec);
  cmFileLockResult LockProcessScope(std::string const& filename,
                                    unsigned long timeoutSec);
  //@}

  /**
   * @brief Unlock the file explicitly.
   */
  cmFileLockResult Release(std::string const& filename);

private:
  class Internal;
  std::unique_ptr<Internal> Impl;
};

#endif // cmFileLockPool_h
