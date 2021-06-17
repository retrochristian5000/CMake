/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#include "cmInstallHeaderSetGenerator.h"

#include <map>
#include <string>
#include <utility>
#include <vector>

#include "cmGeneratorExpression.h"
#include "cmGlobalGenerator.h"
#include "cmHeaderSet.h"
#include "cmInstallType.h"
#include "cmLocalGenerator.h"
#include "cmStringAlgorithms.h"

cmInstallHeaderSetGenerator::cmInstallHeaderSetGenerator(
  std::string targetName, cmHeaderSet* headerSet, std::string const& dest,
  std::string file_permissions, std::vector<std::string> const& configurations,
  std::string const& component, MessageLevel message, bool exclude_from_all,
  bool optional, cmListFileBacktrace backtrace)
  : cmInstallGenerator(dest, configurations, component, message,
                       exclude_from_all, false, std::move(backtrace))
  , TargetName(std::move(targetName))
  , HeaderSet(headerSet)
  , FilePermissions(std::move(file_permissions))
  , Optional(optional)
{
  this->ActionsPerConfig = true;
}

cmInstallHeaderSetGenerator::~cmInstallHeaderSetGenerator() = default;

bool cmInstallHeaderSetGenerator::Compute(cmLocalGenerator* lg)
{
  this->LocalGenerator = lg;

  // Lookup this target in the current directory.
  this->Target = lg->FindLocalNonAliasGeneratorTarget(this->TargetName);
  if (!this->Target) {
    // If no local target has been found, find it in the global scope.
    this->Target =
      lg->GetGlobalGenerator()->FindGeneratorTarget(this->TargetName);
  }

  return true;
}

std::string cmInstallHeaderSetGenerator::GetDestination(
  std::string const& config) const
{
  return cmGeneratorExpression::Evaluate(this->Destination,
                                         this->LocalGenerator, config);
}

void cmInstallHeaderSetGenerator::GenerateScriptForConfig(
  std::ostream& os, const std::string& config, Indent indent)
{
  for (auto const& dirEntry : this->CalculateFilesPerDir(config)) {
    std::string destSub;
    if (!dirEntry.first.empty()) {
      destSub = cmStrCat('/', dirEntry.first);
    }
    this->AddInstallRule(os, cmStrCat(this->GetDestination(config), destSub),
                         cmInstallType_FILES, dirEntry.second,
                         this->GetOptional(), this->FilePermissions.c_str(),
                         nullptr, nullptr, nullptr, indent);
  }
}

std::map<std::string, std::vector<std::string>>
cmInstallHeaderSetGenerator::CalculateFilesPerDir(
  const std::string& config) const
{
  std::map<std::string, std::vector<std::string>> result;

  auto dirCge = this->HeaderSet->CompileDirectory();
  auto const& dir = this->HeaderSet->EvaluateDirectory(
    dirCge, this->LocalGenerator, config, this->Target);

  auto fileCges = this->HeaderSet->CompileHeaderEntries();
  for (auto const& fileCge : fileCges) {
    this->HeaderSet->EvaluateHeaderEntry(
      dir, result, fileCge, this->LocalGenerator, config, this->Target);
  }

  return result;
}
