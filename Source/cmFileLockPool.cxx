/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#include "cmFileLockPool.h"

#include "cmFileLock.h"
#include "cmFileLockResult.h"
#include "cm_memory.hxx"

#include <assert.h>
#include <utility>
#include <vector>

namespace {

/** \class ScopePool
 */
class ScopePool
{
public:
  ScopePool() = default;
  ~ScopePool() = default;

  ScopePool(ScopePool const&) = delete;
  ScopePool& operator=(ScopePool const&) = delete;

  cmFileLockResult Lock(std::string const& filename, unsigned long timeoutSec);
  cmFileLockResult Release(std::string const& filename);
  bool IsAlreadyLocked(std::string const& filename) const;

private:
  typedef std::vector<std::unique_ptr<cmFileLock>> List;
  List Locks;
};

cmFileLockResult ScopePool::Lock(std::string const& filename,
                                 unsigned long timeoutSec)
{
  std::unique_ptr<cmFileLock> lock = cm::make_unique<cmFileLock>();
  {
    cmFileLockResult const result = lock->Lock(filename, timeoutSec);
    if (!result.IsOk()) {
      return result;
    }
  }
  this->Locks.push_back(std::move(lock));
  return cmFileLockResult::MakeOk();
}

cmFileLockResult ScopePool::Release(std::string const& filename)
{
  for (auto& lock : this->Locks) {
    if (lock->IsLocked(filename)) {
      return lock->Release();
    }
  }
  return cmFileLockResult::MakeOk();
}

bool ScopePool::IsAlreadyLocked(std::string const& filename) const
{
  for (auto const& lock : this->Locks) {
    if (lock->IsLocked(filename)) {
      return true;
    }
  }
  return false;
}
};

/** \class cmFileLockPool::Internal
 * \brief Private implementation class for cmFileLockPool.
 */
class cmFileLockPool::Internal
{
public:
  bool IsAlreadyLocked(std::string const& filename) const;

public:
  typedef std::vector<std::unique_ptr<ScopePool>> List;
  List FunctionScopes;
  List FileScopes;
  ScopePool ProcessScope;
};

bool cmFileLockPool::Internal::IsAlreadyLocked(
  std::string const& filename) const
{
  for (auto const& funcScope : this->FunctionScopes) {
    const bool result = funcScope->IsAlreadyLocked(filename);
    if (result) {
      return true;
    }
  }

  for (auto const& fileScope : this->FileScopes) {
    const bool result = fileScope->IsAlreadyLocked(filename);
    if (result) {
      return true;
    }
  }

  return this->ProcessScope.IsAlreadyLocked(filename);
}

cmFileLockPool::cmFileLockPool()
  : Impl(cm::make_unique<Internal>())
{
}

cmFileLockPool::cmFileLockPool(cmFileLockPool&&) noexcept = default;

cmFileLockPool::~cmFileLockPool() = default;

cmFileLockPool& cmFileLockPool::operator=(cmFileLockPool&&) noexcept = default;

void cmFileLockPool::PushFunctionScope()
{
  Impl->FunctionScopes.push_back(cm::make_unique<ScopePool>());
}

void cmFileLockPool::PopFunctionScope()
{
  assert(!Impl->FunctionScopes.empty());
  Impl->FunctionScopes.pop_back();
}

void cmFileLockPool::PushFileScope()
{
  Impl->FileScopes.push_back(cm::make_unique<ScopePool>());
}

void cmFileLockPool::PopFileScope()
{
  assert(!Impl->FileScopes.empty());
  Impl->FileScopes.pop_back();
}

cmFileLockResult cmFileLockPool::LockFunctionScope(std::string const& filename,
                                                   unsigned long timeoutSec)
{
  if (Impl->IsAlreadyLocked(filename)) {
    return cmFileLockResult::MakeAlreadyLocked();
  }
  if (Impl->FunctionScopes.empty()) {
    return cmFileLockResult::MakeNoFunction();
  }
  return Impl->FunctionScopes.back()->Lock(filename, timeoutSec);
}

cmFileLockResult cmFileLockPool::LockFileScope(std::string const& filename,
                                               unsigned long timeoutSec)
{
  if (Impl->IsAlreadyLocked(filename)) {
    return cmFileLockResult::MakeAlreadyLocked();
  }
  assert(!Impl->FileScopes.empty());
  return Impl->FileScopes.back()->Lock(filename, timeoutSec);
}

cmFileLockResult cmFileLockPool::LockProcessScope(std::string const& filename,
                                                  unsigned long timeoutSec)
{
  if (Impl->IsAlreadyLocked(filename)) {
    return cmFileLockResult::MakeAlreadyLocked();
  }
  return Impl->ProcessScope.Lock(filename, timeoutSec);
}

cmFileLockResult cmFileLockPool::Release(std::string const& filename)
{
  for (auto& funcScope : Impl->FunctionScopes) {
    cmFileLockResult const result = funcScope->Release(filename);
    if (!result.IsOk()) {
      return result;
    }
  }

  for (auto& fileScope : Impl->FileScopes) {
    cmFileLockResult const result = fileScope->Release(filename);
    if (!result.IsOk()) {
      return result;
    }
  }

  return Impl->ProcessScope.Release(filename);
}
