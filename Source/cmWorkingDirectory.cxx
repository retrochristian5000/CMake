/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#include "cmWorkingDirectory.h"

#include <cerrno>

#include "cmSystemTools.h"

cmWorkingDirectory::cmWorkingDirectory(std::string const& newdir)
{
  this->OldDir = cmSystemTools::GetLogicalWorkingDirectory();
  this->SetDirectory(newdir);
}

cmWorkingDirectory::~cmWorkingDirectory()
{
  this->Pop();
}

bool cmWorkingDirectory::SetDirectory(std::string const& newdir)
{
  cmsys::Status status = cmSystemTools::SetLogicalWorkingDirectory(newdir);
  if (status) {
    this->ResultCode = 0;
    return true;
  }
  this->ResultCode = status.GetPOSIX();
  return false;
}

void cmWorkingDirectory::Pop()
{
  if (!this->OldDir.empty()) {
    this->SetDirectory(this->OldDir);
    this->OldDir.clear();
  }
}
