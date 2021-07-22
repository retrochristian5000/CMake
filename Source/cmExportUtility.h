/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#pragma once

#include <iosfwd>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "cmStateTypes.h"

class cmGeneratorTarget;
class cmTargetExport;

namespace Exporting {
using PropertyMap = std::map<std::string, std::string>;

struct NamespacedTarget
{
  std::string const& Namespace;
  cmGeneratorTarget const& Target;

  std::string GetQualifiedName() const;
};

cmStateEnums::TargetType GetExportTargetType(
  cmTargetExport const& targetExport);

std::string CompileFeatureToCompileFlag(cmGeneratorTarget const& target,
                                        std::string const& compileFeature);

#ifndef CMAKE_BOOTSTRAP
enum class GenerateType
{
  Build,
  Install
};

namespace AndroidMK {
void GenerateInterfaceProperties(cmGeneratorTarget const& target,
                                 std::ostream& os,
                                 const PropertyMap& properties,
                                 GenerateType type, std::string const& config);
}

namespace PkgConfig {
void GenerateExportMetadata(std::ostream& os, cmGeneratorTarget const& target);
bool GenerateInterfaceProperties(std::ostream& os,
                                 cmGeneratorTarget const& target,
                                 GenerateType type, std::string const& config);
std::vector<std::string> RemoveImplicitEntries(
  cmGeneratorTarget const& target, std::string const& entryName,
  std::vector<std::string> const& entries);

void ReplaceImportPrefix(std::string& input);
}
#endif

namespace CMakeConfig {
std::string Escape(std::string const& str);

void GenerateImportTargetCode(std::ostream& os, NamespacedTarget const& target,
                              cmStateEnums::TargetType targetType);

void GenerateRequiredCMakeVersion(std::ostream& os, const char* versionString);

void GenerateImportedFileChecksCode(
  std::ostream& os, NamespacedTarget const& target,
  PropertyMap const& properties,
  std::set<std::string> const& importedLocations);

void GenerateExpectedTargetsCode(std::ostream& os,
                                 std::string const& expectedTargets);
void GenerateMissingTargetsCheckCode(
  std::ostream& os, const std::vector<std::string>& missingTargets);

void GenerateImportedFileCheckLoop(std::ostream& os);

void GenerateImportPropertyCode(std::ostream& os, const std::string& config,
                                NamespacedTarget const& target,
                                PropertyMap const& properties);

void GenerateImportVersionCode(std::ostream& os);
void GenerateImportHeaderCode(std::ostream& os, const std::string& config);

void GeneratePolicyHeaderCode(std::ostream& os);
void GeneratePolicyFooterCode(std::ostream& os);
void GenerateImportFooterCode(std::ostream& os);
}
}
