/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#include "cmSourceGroup.h"

#include "cm_memory.hxx"
#include "cmsys/RegularExpression.hxx"

#include <set>
#include <utility>

class cmSourceGroup::Internal
{
public:
  /**
   * The name of the source group.
   */
  std::string Name;

  /**
   * Full path to group.
   */
  std::string FullName;

  /**
   * The regular expression matching the files in the group.
   */
  cmsys::RegularExpression GroupRegex;

  /**
   * Set of file names explicitly added to this group.
   */
  std::set<std::string> GroupFiles;

  /**
   * Vector of all source files that have been assigned to
   * this group.
   */
  std::vector<cmSourceFile const*> SourceFiles;

  /**
   * Vector of all children source groups.
   */
  std::vector<cmSourceGroup> GroupChildren;
};

cmSourceGroup::cmSourceGroup(std::string name, const char* regex,
                             const char* parentName)
  : Impl(cm::make_unique<Internal>())
{
  Impl->Name = std::move(name);
  this->SetGroupRegex(regex);
  if (parentName) {
    Impl->FullName = parentName;
    Impl->FullName += "\\";
  }
  Impl->FullName += Impl->Name;
}

cmSourceGroup::~cmSourceGroup() = default;

cmSourceGroup::cmSourceGroup(cmSourceGroup const& grp)
  : Impl(cm::make_unique<Internal>(*grp.Impl))
{
}

cmSourceGroup::cmSourceGroup(cmSourceGroup&&) noexcept = default;

cmSourceGroup& cmSourceGroup::operator=(cmSourceGroup const& grp)
{
  Impl->operator=(*grp.Impl);
  return *this;
}

cmSourceGroup& cmSourceGroup::operator=(cmSourceGroup&&) noexcept = default;

void cmSourceGroup::SetGroupRegex(const char* regex)
{
  if (regex) {
    Impl->GroupRegex.compile(regex);
  } else {
    Impl->GroupRegex.compile("^$");
  }
}

void cmSourceGroup::AddGroupFile(std::string const& name)
{
  Impl->GroupFiles.insert(name);
}

std::string const& cmSourceGroup::GetName() const
{
  return Impl->Name;
}

std::string const& cmSourceGroup::GetFullName() const
{
  return Impl->FullName;
}

bool cmSourceGroup::MatchesRegex(std::string const& name)
{
  return Impl->GroupRegex.find(name);
}

bool cmSourceGroup::MatchesFiles(std::string const& name) const
{
  return Impl->GroupFiles.find(name) != Impl->GroupFiles.cend();
}

void cmSourceGroup::AssignSource(cmSourceFile const* sf)
{
  Impl->SourceFiles.push_back(sf);
}

std::vector<cmSourceFile const*> const& cmSourceGroup::GetSourceFiles() const
{
  return Impl->SourceFiles;
}

std::vector<cmSourceGroup> const& cmSourceGroup::GetGroupChildren() const
{
  return Impl->GroupChildren;
}

void cmSourceGroup::AddChild(cmSourceGroup const& child)
{
  Impl->GroupChildren.push_back(child);
}

cmSourceGroup* cmSourceGroup::LookupChild(std::string const& name)
{
  for (cmSourceGroup& group : Impl->GroupChildren) {
    // look if descenened is the one were looking for
    if (group.GetName() == name) {
      return (&group); // if it so return it
    }
  }

  // if no child with this name was found return NULL
  return nullptr;
}

cmSourceGroup* cmSourceGroup::MatchChildrenFiles(std::string const& name)
{
  if (this->MatchesFiles(name)) {
    return this;
  }
  for (cmSourceGroup& group : Impl->GroupChildren) {
    cmSourceGroup* result = group.MatchChildrenFiles(name);
    if (result) {
      return result;
    }
  }
  return nullptr;
}

cmSourceGroup* cmSourceGroup::MatchChildrenRegex(std::string const& name)
{
  for (cmSourceGroup& group : Impl->GroupChildren) {
    cmSourceGroup* result = group.MatchChildrenRegex(name);
    if (result) {
      return result;
    }
  }
  if (this->MatchesRegex(name)) {
    return this;
  }

  return nullptr;
}
