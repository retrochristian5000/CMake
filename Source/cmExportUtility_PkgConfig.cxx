/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */

#include "cmExportUtility.h"

#include <algorithm>
#include <iterator>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include <cm/string_view>
#include <cmext/string_view>

#include "cmGeneratorTarget.h"
#include "cmMakefile.h"
#include "cmProperty.h"
#include "cmStateSnapshot.h"
#include "cmStringAlgorithms.h"

void Exporting::PkgConfig::GenerateExportMetadata(
  std::ostream& os, cmGeneratorTarget const& target)
{
  const auto optionallyAddProperty =
    [](std::ostream& os, cmGeneratorTarget const& target,
       std::string_view property, const char* variableName) {
      const cmProp value = target.Makefile->GetDefinition(variableName);
      if (value && !value->empty()) {
        os << property << ": " << *value << std::endl;
      }
    };

  os << "Name: " << target.Makefile->GetStateSnapshot().GetProjectName()
     << std::endl;
  optionallyAddProperty(os, target, "Description", "PROJECT_DESCRIPTION");
  optionallyAddProperty(os, target, "URL", "PROJECT_HOMEPAGE_URL");
  const char* VersionAttribute = "Version";
  if (const cmProp targetVersion = target.GetProperty("VERSION")) {
    os << VersionAttribute << ": " << *targetVersion << std::endl;
  } else {
    optionallyAddProperty(os, target, VersionAttribute, "PROJECT_VERSION");
  }
}

std::vector<std::string> Exporting::PkgConfig::RemoveImplicitEntries(
  const cmGeneratorTarget& target, const std::string& entryName,
  const std::vector<std::string>& entries)
{
  // TODO target languages
  const std::vector<std::string> implicitEntriesList =
    cmExpandedList(*target.Makefile->GetDefinition(
      cmStrCat("CMAKE_C_IMPLICIT_"_s, entryName)));
  const std::set<std::string> implicitEntries(implicitEntriesList.begin(),
                                              implicitEntriesList.end());

  std::vector<std::string> result;
  std::copy_if(entries.begin(), entries.end(), std::back_inserter(result),
               [&implicitEntries](const std::string& d) {
                 return !implicitEntries.count(d);
               });
  return result;
}

void Exporting::PkgConfig::ReplaceImportPrefix(std::string& input)
{
  static const auto importPrefix = "${_IMPORT_PREFIX}"_s;
  static const auto destPrefix = "$ENV{DESTDIR}"_s;
  auto pos = input.find(importPrefix);
  while (pos != std::string::npos) {
    input.replace(pos, importPrefix.size(), destPrefix);
    pos = input.find(importPrefix, pos + destPrefix.size());
  }
}
