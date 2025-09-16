/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file LICENSE.rst or https://cmake.org/licensing for details.  */
#pragma once

#include "cmConfigure.h" // IWYU pragma: keep

#include <map>
#include <set>
#include <string>
#include <vector>

#include <cm/memory>
#include <cm/string_view>

#include "cmVSVersion.h"

namespace cm {
namespace VS {

struct Solution
{
  Solution() = default;
  Solution(Solution&&) = default;
  Solution& operator=(Solution&&) = default;

  struct ProjectConfig
  {
    std::string Config;
    bool Build = false;
    bool Deploy = false;
  };

  struct Project
  {
    std::string Name;
    std::string Id;
    std::string Type;
    std::string Path;
    std::vector<Project const*> BuildDependencies;
    std::vector<ProjectConfig> Configs;
    std::string Platform;
  };

  struct Folder
  {
    std::string Name;
    std::string Id;
    std::vector<Folder const*> Folders;
    std::vector<Project const*> Projects;
    std::set<std::string> Files;
  };

  enum class Load
  {
    Pre,
    Post,
  };

  struct PropertyGroup
  {
    std::string Name;
    Load Scope = Load::Post;
    std::map<std::string, std::string> Map;
  };

  Version VSVersion;
  VersionExpress VSExpress = VersionExpress::No;
  std::string Platform;
  std::vector<std::string> Configs;
  std::vector<Folder const*> Folders;
  std::vector<Project const*> Projects;
  std::vector<PropertyGroup const*> PropertyGroups;
  std::string StartupProject;

  Folder* GetFolder(cm::string_view name);
  Project* GetProject(cm::string_view name);
  PropertyGroup* GetPropertyGroup(cm::string_view name);
  void CanonicalizeOrder();

private:
  Solution(Solution const&) = delete;
  Solution& operator=(Solution const&) = delete;
  std::map<cm::string_view, std::unique_ptr<Folder>> FolderMap;
  std::map<cm::string_view, std::unique_ptr<Project>> ProjectMap;
  std::map<cm::string_view, std::unique_ptr<PropertyGroup>> PropertyGroupMap;
};

void WriteSln(std::ostream& sln, Solution const& solution);

}
}
