/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#include "cmExportInstallPkgConfigGenerator.h"

#include <algorithm>
#include <cstddef>
#include <memory>
#include <ostream>
#include <set>

#include "cmExportSet.h"
#include "cmGeneratorExpression.h"
#include "cmGeneratorTarget.h"
#include "cmInstallExportGenerator.h"
#include "cmInstallTargetGenerator.h"
#include "cmLocalGenerator.h"
#include "cmMakefile.h"
#include "cmStateTypes.h"
#include "cmSystemTools.h"
#include "cmTarget.h"
#include "cmTargetExport.h"

#include "../Utilities/std/cmext/string_view"

namespace {
using string_vec = std::vector<std::string>;

void removeDuplicatesStable(string_vec& v)
{
  v.resize(std::distance(v.begin(), std::unique(v.begin(), v.end())));
}

void removeDuplicatesSort(string_vec& v)
{
  std::set<std::string> s(v.begin(), v.end());
  v.assign(s.begin(), s.end());
}
}

cmExportInstallPkgConfigGenerator::cmExportInstallPkgConfigGenerator(
  cmInstallExportGenerator* iegen)
  : cmExportInstallFileGenerator(iegen)
{
}

struct cmExportInstallPkgConfigGenerator::PkgConfigTargetData
{
  std::vector<std::string> requiredPackages;
  std::vector<std::string> includeDirs;
  std::vector<std::string> compileOptions;
  std::vector<std::string> compileDefinitions;
  std::vector<std::string> linkLibraries;
};

bool cmExportInstallPkgConfigGenerator::GenerateTargetExportCode(
  std::ostream& os, cmExportInstallFileGenerator::Context& ctx,
  const cmTargetExport& target, cmStateEnums::TargetType targetType)
{
  const cmGeneratorTarget* gt = target.Target;

  if (this->ExportOld) {
    //       this->P
  }

  string_vec cflags;
  PkgConfigTargetData data;

  PopulatePkgConfigData(*gt, ctx, data);
  removeDuplicatesSort(data.compileDefinitions);
  removeDuplicatesSort(data.compileOptions);
  removeDuplicatesStable(data.includeDirs);
  removeDuplicatesSort(data.requiredPackages);

  if (!data.requiredPackages.empty()) {
    os << "Requires: " << cmJoin(data.requiredPackages, " ") << std::endl;
  }

  os << "Cflags:";

  for (const auto& item : data.compileOptions) {
    os << ' ' << item;
  }

  for (const auto& item : data.includeDirs) {
    os << " -I" << item;
  }

  for (const auto& item : data.compileDefinitions) {
    os << " -D" << item;
  }
  os << std::endl;

  string_vec libFiles;
  libFiles.push_back(gt->GetFullName(ctx.Config));
  // the first item is the te's full path, we don't need it
  std::copy(data.linkLibraries.begin() + 1, data.linkLibraries.end(),
            std::back_inserter(libFiles));
  const auto linkEntries = this->SplitLinkLibs(libFiles);
  os << "Libs: -L${libdir}";
  for (const auto& dir :
       RemoveImplicitEntries(*gt, "LINK_DIRECTORIES", linkEntries.first)) {
    os << " -L" << dir;
  }

  for (const auto& lib : linkEntries.second) {
    os << ' ' << lib;
  }

  os << std::endl;

  return true;
}

bool cmExportInstallPkgConfigGenerator::
  GeneratedFilesRequireVariableExpansion() const
{
  return true;
}

bool cmExportInstallPkgConfigGenerator::GenerateImportHeaderCode(
  std::ostream& os, Context& ctx)
{
  if (ctx.TargetExports.empty()) {
    return true;
  }

  if (ctx.TargetExports.size() > 1) {
    std::ostringstream e;
    e << "Export \"" << this->IEGen->GetExportSet()->GetName()
      << "\" contains " << ctx.TargetExports.size() << " targets. "
      << "install(EXPORT ...) does not yet support multi-target "
      << "export sets in the PKG_CONFIG_MODE";
    cmSystemTools::Error(e.str());
    return false;
  }

  const auto optionallyAddProperty =
    [](std::ostream& os, const cmGeneratorTarget* target,
       std::string_view property, const char* variableName) {
      const cmProp value = target->Makefile->GetDefinition(variableName);
      if (value && !value->empty()) {
        os << property << ": " << *value << std::endl;
      }
    };

  cmTargetExport const& te = *ctx.TargetExports.front();
  const auto destPath = te.LibraryGenerator->GetDestination(ctx.Config);

  os << "prefix=$ENV{DESTDIR}" << std::endl;
  os << "libdir=" << cmInstallGenerator::GetDestDirPath(destPath) << std::endl;
  os << std::endl;

  os << "Name: " << te.Target->Makefile->GetStateSnapshot().GetProjectName()
     << std::endl;
  optionallyAddProperty(os, te.Target, "Description", "PROJECT_DESCRIPTION");
  optionallyAddProperty(os, te.Target, "URL", "PROJECT_HOMEPAGE_URL");
  optionallyAddProperty(os, te.Target, "Version", "PROJECT_VERSION");

  return true;
}

bool cmExportInstallPkgConfigGenerator::GenerateImportFooterCode(
  std::ostream& /*os*/, Context& /*ctx*/)
{
  return true;
}

void cmExportInstallPkgConfigGenerator::PopulatePkgConfigData(
  cmGeneratorTarget const& target, Context& ctx,
  PkgConfigTargetData& data) const
{
  const auto expandedProperty =
    [&ctx, &target](
      const ImportPropertyMap& properties, const std::string& name,
      const std::string& implicitSuffixName = {}) -> std::vector<std::string> {
    const auto it = properties.find(name);
    if (it == properties.end()) {
      return {};
    }
    auto result = cmExpandedList(cmGeneratorExpression::Evaluate(
      it->second, target.LocalGenerator, ctx.Config));
    return implicitSuffixName.empty()
      ? result
      : RemoveImplicitEntries(target, implicitSuffixName, result);
  };

  ImportPropertyMap properties = this->CommonInterfaceProperties(
    &target, cmGeneratorExpression::InstallInterface, ctx);
  this->PopulateInterfaceLinkLibrariesProperty(
    &target, cmGeneratorExpression::InstallInterface, properties, ctx);
  if (this->ExportOld) {
    //       this->P
  }

  const string_vec systemIncludeDirs = expandedProperty(
    properties, "INTERFACE_SYSTEM_INCLUDE_DIRECTORIES", "INCLUDE_DIRECTORIES");
  for (std::string dir : systemIncludeDirs) {
    this->ReplaceImportPrefix(dir);
    data.includeDirs.push_back(std::move(dir));
  }
  const string_vec includeDirs = expandedProperty(
    properties, "INTERFACE_INCLUDE_DIRECTORIES", "INCLUDE_DIRECTORIES");
  for (std::string dir : includeDirs) {
    this->ReplaceImportPrefix(dir);
    data.includeDirs.push_back(std::move(dir));
  }

  const string_vec compileOptions =
    expandedProperty(properties, "INTERFACE_COMPILE_OPTIONS");
  std::copy(compileOptions.begin(), compileOptions.end(),
            std::back_inserter(data.compileOptions));
  const string_vec compileFeatures =
    expandedProperty(properties, "INTERFACE_COMPILE_FEATURES");
  for (const auto& f : compileFeatures) {
    const std::string flag = this->CompileFeatureToCompileFlag(target, f);
    if (!flag.empty()) {
      data.compileOptions.push_back(flag);
    }
  }
  const string_vec compileDefinitions =
    expandedProperty(properties, "INTERFACE_COMPILE_DEFINITIONS");
  std::copy(compileDefinitions.begin(), compileDefinitions.end(),
            std::back_inserter(data.compileDefinitions));

  if (target.IsLinkable()) {
    cmStateEnums::ArtifactType artifact = target.HasImportLibrary(ctx.Config)
      ? cmStateEnums::ImportLibraryArtifact
      : cmStateEnums::RuntimeBinaryArtifact;
    const auto artifactPath = target.GetFullPath(ctx.Config, artifact);
    if (!cmIsNOTFOUND(artifactPath)) {
      data.linkLibraries.push_back(artifactPath);
    }
  }

  const string_vec linkLibrariesNames =
    cmExpandedList(properties["INTERFACE_LINK_LIBRARIES"]);
  for (const std::string& linkLibraryName : linkLibrariesNames) {
    const cmGeneratorTarget* linkTarget =
      target.LocalGenerator->FindGeneratorTargetToUse(linkLibraryName);
    if (linkTarget) {
      if (const cmProp pkgConfigTargetName =
            linkTarget->GetProperty("PKG_CONFIG_NAME")) {
        data.requiredPackages.push_back(*pkgConfigTargetName);
      } else {
        this->PopulatePkgConfigData(*linkTarget, ctx, data);
      }
    } else {
      data.linkLibraries.push_back(linkLibraryName);
    }
  }
}

void cmExportInstallPkgConfigGenerator::ReplaceImportPrefix(std::string& input)
{
  static const auto importPrefix = "${_IMPORT_PREFIX}"_s;
  static const auto destPrefix = "$ENV{DESTDIR}"_s;
  auto pos = input.find(importPrefix);
  while (pos != std::string::npos) {
    input.replace(pos, importPrefix.size(), destPrefix);
    pos = input.find(importPrefix, pos + destPrefix.size());
  }
}

std::string cmExportInstallPkgConfigGenerator::CompileFeatureToCompileFlag(
  cmGeneratorTarget const& target, std::string const& compileFeature)
{
  // compile feature alike: c_std_23 or cxx_std_20
  // variables:
  // CMAKE_CXX20_STANDARD_COMPILE_OPTION "-std=c++20")
  // CMAKE_CXX20_EXTENSION_COMPILE_OPTION "-std=gnu++20")
  auto langEndPos = compileFeature.find('_');
  auto versionDelimPos = compileFeature.find('_', langEndPos + 1);
  const auto lang =
    cmSystemTools::UpperCase(compileFeature.substr(0, langEndPos));

  const auto featureType =
    compileFeature.substr(langEndPos + 1, versionDelimPos - langEndPos - 1);
  if (featureType == "std"_s) {
    return target.Makefile->GetSafeDefinition(
      cmStrCat("CMAKE_", lang, compileFeature.substr(versionDelimPos + 1),
               "_STANDARD_COMPILE_OPTION"));
  }

  return {};
}

std::vector<std::string>
cmExportInstallPkgConfigGenerator::RemoveImplicitEntries(
  cmGeneratorTarget const& target, std::string const& entryName,
  std::vector<std::string> const& entries)
{
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

std::pair<std::vector<std::string>, std::vector<std::string>>
cmExportInstallPkgConfigGenerator::SplitLinkLibs(
  const std::vector<std::string>& items) const
{
  std::vector<std::string> dirs;
  std::vector<std::string> libs;

  for (const std::string& item : items) {
    if (item.find("-l") == 0) {
      libs.push_back(item);
    } else {
      std::string fileName = cmSystemTools::GetFilenameWithoutExtension(item);
      if (fileName.find("lib") == 0) { // CMAKE_SHARED_LIBRARY_PREFIX?
        libs.push_back("-l" + fileName.substr(3));
        const auto dir = cmSystemTools::GetFilenamePath(item);
        if (!dir.empty()) {
          dirs.push_back(dir);
        }
      } else {
        libs.push_back(item);
      }
    }
  }

  removeDuplicatesStable(dirs);
  return std::make_pair(std::move(dirs), std::move(libs));
}

void cmExportInstallPkgConfigGenerator::GenerateImportTargetCode(
  std::ostream&, cmGeneratorTarget const*, cmStateEnums::TargetType)
{
}

void cmExportInstallPkgConfigGenerator::GenerateImportTargetsConfig(
  std::ostream&, std::string const&, cmExportFileGenerator::Context&,
  std::string const&)
{
}
