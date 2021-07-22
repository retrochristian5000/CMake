/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */

#include "cmExportUtility.h"

#include <algorithm>
#include <iterator>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include <cm/string_view>
#include <cmext/string_view>

#include <cmsys/RegularExpression.hxx>

#include "cmGeneratorExpression.h"
#include "cmGeneratorTarget.h"
#include "cmInstallTargetGenerator.h"
#include "cmLocalGenerator.h"
#include "cmMakefile.h"
#include "cmProperty.h"
#include "cmStateSnapshot.h"
#include "cmStateTypes.h"
#include "cmStringAlgorithms.h"
#include "cmSystemTools.h"
#include "cmTarget.h"

#define CMAKE_EXPORT_PKG_CONF_USE_FULL_LIBRARY_PATH 1
namespace {
using StringVec = std::vector<std::string>;

const auto ImportPrefixStr = "${_IMPORT_PREFIX}"_s;
const auto PrefixVarName = "prefix"_s;
const auto PrefixStr = "${prefix}"_s;

struct PkgConfigTargetData
{
  std::vector<std::string> requiredPackages;
  std::vector<std::string> privateRequiredPackages;
  std::vector<std::string> includeDirs;
  std::set<std::string> compileOptions;
  std::set<std::string> compileDefinitions;
  std::vector<std::string> linkLibraries;
};

void RemoveDuplicatesStable(StringVec& v)
{
  v.resize(std::distance(v.begin(), std::unique(v.begin(), v.end())));
}

cmInstallTargetGenerator const* InstallGenerator(
  cmGeneratorTarget const& target, std::string const& config)
{
  if (target.Target->GetInstallGenerators().empty()) {
    return nullptr;
  }

  const bool impLib = target.HasImportLibrary(config);
  for (auto const* ig : target.Target->GetInstallGenerators()) {
    if (ig->IsImportLibrary() == impLib) {
      return ig;
    }
  }

  return nullptr;
}

//
std::string LinkArtifactPath(cmGeneratorTarget const& target,
                             std::string const& config)
{
  const cmStateEnums::ArtifactType at = target.HasImportLibrary(config)
    ? cmStateEnums::ImportLibraryArtifact
    : cmStateEnums::RuntimeBinaryArtifact;
  if (cmInstallTargetGenerator const* ig = InstallGenerator(target, config)) {
    return cmStrCat("${"_s, PrefixVarName, "}/"_s, ig->GetDestination(config),
                    '/',
                    target.GetFullName(config, at)); // JoinPath() instead?
  }

  return target.GetFullName(config, at);
}

void PopulatePkgConfigTargetData(
  PkgConfigTargetData& data, cmGeneratorTarget const& target,
  Exporting::PropertyLoader const& targetPropertyLoader,
  std::string const& config)
{
  using namespace Exporting;
  using namespace Exporting::PkgConfig;

  const auto expandedProperty =
    [&config, &target](
      const PropertyMap& properties, const std::string& name,
      const std::string& implicitSuffixName = {}) -> std::vector<std::string> {
    const auto it = properties.find(name);
    if (it == properties.end()) {
      return {};
    }
    auto result = cmExpandedList(cmGeneratorExpression::Evaluate(
      it->second, target.LocalGenerator, config));
    return implicitSuffixName.empty()
      ? result
      : OmitImplicitEntries(target, config, implicitSuffixName, result);
  };

  PropertyMap properties = targetPropertyLoader(target);

  const StringVec systemIncludeDirs = expandedProperty(
    properties, "INTERFACE_SYSTEM_INCLUDE_DIRECTORIES", "INCLUDE_DIRECTORIES");
  for (std::string dir : systemIncludeDirs) {
    ReplaceImportPrefix(dir);
    data.includeDirs.push_back(std::move(dir));
  }
  const StringVec includeDirs = expandedProperty(
    properties, "INTERFACE_INCLUDE_DIRECTORIES", "INCLUDE_DIRECTORIES");
  for (std::string dir : includeDirs) {
    ReplaceImportPrefix(dir);
    data.includeDirs.push_back(std::move(dir));
  }

  const auto setInserter = [](auto& set) {
    return std::inserter(set, set.begin());
  };

  const StringVec compileOptions =
    expandedProperty(properties, "INTERFACE_COMPILE_OPTIONS");
  std::copy(compileOptions.begin(), compileOptions.end(),
            setInserter(data.compileOptions));
  const StringVec compileFeatures =
    expandedProperty(properties, "INTERFACE_COMPILE_FEATURES");
  for (const auto& f : compileFeatures) {
    const std::string flag = Exporting::CompileFeatureToCompileFlag(target, f);
    if (!flag.empty()) {
      data.compileOptions.insert(flag);
    }
  }
  const StringVec compileDefinitions =
    expandedProperty(properties, "INTERFACE_COMPILE_DEFINITIONS");
  std::copy(compileDefinitions.begin(), compileDefinitions.end(),
            setInserter(data.compileDefinitions));

  if (target.IsLinkable()) {
    const auto artifactPath = LinkArtifactPath(target, config);
    if (!cmIsNOTFOUND(artifactPath)) {
      data.linkLibraries.push_back(artifactPath);
    }
  }

  if (cmIsOn(properties["INTERFACE_POSITION_INDEPENDENT_CODE"])) {
    data.compileOptions.insert(
      target.GetLocalGenerator()->GetPositionIndependentFlags(
        target.GetLinkerLanguage(config), target.GetType()));
  }

  const cmsys::RegularExpression linkOnlyRx{ "\\$<LINK_ONLY:(.+)>" };
  const StringVec linkLibrariesNames =
    cmExpandedList(properties["INTERFACE_LINK_LIBRARIES"]);
  for (std::string linkLibraryName : linkLibrariesNames) {
    cmsys::RegularExpressionMatch m;
    bool isPrivate = false;
    if (linkOnlyRx.find(linkLibraryName.c_str(), m)) {
      // we do not express a difference between static and shared libraries yet
      // in the pkg-config format, so we simply strip it out of the genex
      linkLibraryName = m.match(1);
      // but we will put such a package in the Required.private list
      isPrivate = true;
    }
    const cmGeneratorTarget* linkTarget =
      target.LocalGenerator->FindGeneratorTargetToUse(linkLibraryName);
    if (linkTarget) {
      if (const cmProp pkgConfigTargetName =
            linkTarget->GetProperty("PKG_CONFIG_NAME")) {
        (isPrivate ? data.privateRequiredPackages : data.requiredPackages)
          .push_back(*pkgConfigTargetName);
      } else {
        PopulatePkgConfigTargetData(data, *linkTarget, targetPropertyLoader,
                                    config);
      }
    } else {
      data.linkLibraries.push_back(linkLibraryName);
    }
  }
}

#ifndef CMAKE_EXPORT_PKG_CONF_USE_FULL_LIBRARY_PATH
class LibraryNameScheme
{
public:
  LibraryNameScheme(cmMakefile const& mf);

  std::string TryOptimize(std::string const& fileName) const;

private:
  bool IsSharedLibrary(std::string const& fileName) const;
  bool IsStaticLibrary(std::string const& fileName) const;

  std::string StaticSuffix;
  std::string SharedPrefix;
  std::string SharedSuffix;
};

LibraryNameScheme::LibraryNameScheme(cmMakefile const& mf)
  : StaticSuffix{ mf.GetSafeDefinition("CMAKE_STATIC_LIBRARY_SUFFIX") }
  , SharedPrefix{ mf.GetSafeDefinition("CMAKE_SHARED_LIBRARY_PREFIX") }
  , SharedSuffix{ mf.GetSafeDefinition("CMAKE_SHARED_LIBRARY_SUFFIX") }
{
}

bool LibraryNameScheme::IsSharedLibrary(const std::string& fileName) const
{
  return cmHasPrefix(fileName, this->SharedPrefix) &&
    cmHasSuffix(fileName, this->SharedSuffix);
}

bool LibraryNameScheme::IsStaticLibrary(const std::string& fileName) const
{
  return cmHasSuffix(fileName, this->StaticSuffix);
}

std::string LibraryNameScheme::TryOptimize(const std::string& fileName) const
{
  if (this->IsStaticLibrary(fileName)) {
    return cmStrCat("-l:"_s, fileName);
  }

  if (this->IsSharedLibrary(fileName)) {
    return cmStrCat("-l"_s,
                    fileName.substr(this->SharedPrefix.size(),
                                    fileName.size() -
                                      this->SharedPrefix.size() -
                                      this->SharedSuffix.size()));
  }

  return {};
}

std::pair<StringVec, StringVec> SplitLinkLibs(StringVec const& items,
                                              LibraryNameScheme const& scheme)
{
  std::vector<std::string> dirs;
  std::vector<std::string> libs;

  for (const std::string& item : items) {
    if (cmHasPrefix(item, '-')) {
      libs.push_back(item);
    } else {
      const auto fileName = cmSystemTools::GetFilenameName(item);
      std::string optimized = scheme.TryOptimize(fileName);
      if (!optimized.empty()) {
        libs.push_back(std::move(optimized));
        const auto dir = cmSystemTools::GetFilenamePath(item);
        if (!dir.empty()) {
          dirs.push_back(dir);
        }
      } else {
        libs.push_back(item);
      }
    }
  }

  RemoveDuplicatesStable(dirs);
  return std::make_pair(std::move(dirs), std::move(libs));
}
#endif
}

bool Exporting::PkgConfig::CanBeExported(cmGeneratorTarget const& target,
                                         cmStateEnums::TargetType targetType)
{
  switch (targetType) {
    case cmStateEnums::EXECUTABLE:
    case cmStateEnums::STATIC_LIBRARY:
    case cmStateEnums::SHARED_LIBRARY:
    case cmStateEnums::MODULE_LIBRARY:
    case cmStateEnums::INTERFACE_LIBRARY:
      break;
    default:
      return false;
  }

  cmProp prop = target.GetProperty("INTERFACE_SOURCES");
  return !prop || prop->empty();
}

void Exporting::PkgConfig::GenerateExportMetadata(
  std::ostream& os, cmGeneratorTarget const& target)
{
  const auto optionallyAddProperty =
    [](std::ostream& out, cmGeneratorTarget const& tgt,
       std::string_view property, const char* variableName) {
      const cmProp value = tgt.Makefile->GetDefinition(variableName);
      if (value && !value->empty()) {
        out << property << ": " << *value << std::endl;
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

std::vector<std::string> Exporting::PkgConfig::OmitImplicitEntries(
  cmGeneratorTarget const& target, std::string const& config,
  std::string const& entryName, std::vector<std::string> const& entries)
{
  const std::vector<std::string> implicitEntriesList =
    cmExpandedList(*target.Makefile->GetDefinition(cmStrCat(
      "CMAKE_", target.GetLinkerLanguage(config), "_IMPLICIT_"_s, entryName)));
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
  for (auto pos = input.find(ImportPrefixStr); pos != std::string::npos;
       pos = input.find(ImportPrefixStr, pos + PrefixStr.size())) {
    input.replace(pos, ImportPrefixStr.size(), PrefixStr);
  }
}

bool Exporting::PkgConfig::GenerateTargetExportCode(
  std::ostream& os, cmGeneratorTarget const& target,
  cmStateEnums::TargetType /*targetType*/, std::string const& config,
  PropertyLoader const& targetPropertyLoader)
{

  PkgConfigTargetData data;
  PopulatePkgConfigTargetData(data, target, targetPropertyLoader, config);
  RemoveDuplicatesStable(data.includeDirs);
  RemoveDuplicatesStable(data.requiredPackages);

  auto const printPrefixed = [](std::ostream& out, auto const& range,
                                auto prefix) {
    for (const auto& item : range) {
      out << prefix << item;
    }
  };

  if (!data.requiredPackages.empty()) {
    os << "Requires:";
    printPrefixed(os, data.requiredPackages, " ");
    os << std::endl;
  }

  if (!data.privateRequiredPackages.empty()) {
    os << "Requires.private:";
    printPrefixed(os, data.privateRequiredPackages, " ");
    os << std::endl;
  }

  if (!data.compileDefinitions.empty() || !data.compileOptions.empty() ||
      !data.includeDirs.empty()) {
    os << "Cflags:";
    printPrefixed(os, data.compileOptions, ' ');
    printPrefixed(os, data.includeDirs, " -I");
    printPrefixed(os, data.compileDefinitions, " -D");
    os << std::endl;
  }

#ifndef CMAKE_EXPORT_PKG_CONF_USE_FULL_LIBRARY_PATH
  const auto linkEntries =
    SplitLinkLibs(data.linkLibraries, LibraryNameScheme(*target.Makefile));

  if (!linkEntries.second.empty()) {
    os << "Libs:";
    printPrefixed(os,
                  OmitImplicitEntries(target, config, "LINK_DIRECTORIES",
                                      linkEntries.first),
                  " -L");

    printPrefixed(os, linkEntries.second, ' ');
    os << std::endl;
  }
#else
  if (!data.linkLibraries.empty()) {
    os << "Libs:";
    printPrefixed(os, data.linkLibraries, ' ');
    os << std::endl;
  }
#endif

  return true;
}
