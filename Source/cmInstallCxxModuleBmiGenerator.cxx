/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#include "cmInstallCxxModuleBmiGenerator.h"

#include <utility>

#include "cmGeneratorExpression.h"
#include "cmListFileCache.h"

class cmLocalGenerator;

cmInstallCxxModuleBmiGenerator::cmInstallCxxModuleBmiGenerator(
  std::string const& dest, std::string file_permissions,
  std::vector<std::string> const& configurations, std::string const& component,
  MessageLevel message, bool exclude_from_all, bool optional,
  cmListFileBacktrace backtrace)
  : cmInstallGenerator(dest, configurations, component, message,
                       exclude_from_all, false, std::move(backtrace))
  , LocalGenerator(nullptr)
  , FilePermissions(std::move(file_permissions))
  , Optional(optional)
{
  this->ActionsPerConfig = true;
}

cmInstallCxxModuleBmiGenerator::~cmInstallCxxModuleBmiGenerator() = default;

bool cmInstallCxxModuleBmiGenerator::Compute(cmLocalGenerator* lg)
{
  this->LocalGenerator = lg;
  return true;
}

std::string cmInstallCxxModuleBmiGenerator::GetDestination(
  std::string const& config) const
{
  return cmGeneratorExpression::Evaluate(this->Destination,
                                         this->LocalGenerator, config);
}

void cmInstallCxxModuleBmiGenerator::GenerateScriptActions(std::ostream& os,
                                                           Indent indent)
{
  // TODO
}

void cmInstallCxxModuleBmiGenerator::GenerateScriptForConfig(
  std::ostream& os, const std::string& config, Indent indent)
{
  // TODO
}
