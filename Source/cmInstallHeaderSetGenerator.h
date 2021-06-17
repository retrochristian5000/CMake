/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#pragma once

#include <iosfwd>
#include <map>
#include <string>
#include <vector>

#include "cmInstallGenerator.h"
#include "cmListFileCache.h"
#include "cmScriptGenerator.h"

class cmGeneratorTarget;
class cmHeaderSet;
class cmLocalGenerator;

class cmInstallHeaderSetGenerator : public cmInstallGenerator
{
public:
  cmInstallHeaderSetGenerator(std::string targetName, cmHeaderSet* headerSet,
                              std::string const& dest,
                              std::string file_permissions,
                              std::vector<std::string> const& configurations,
                              std::string const& component,
                              MessageLevel message, bool exclude_from_all,
                              bool optional, cmListFileBacktrace backtrace);
  ~cmInstallHeaderSetGenerator() override;

  bool Compute(cmLocalGenerator* lg) override;

  std::string GetDestination(std::string const& config) const;
  std::string GetDestination() const { return this->Destination; }
  bool GetOptional() const { return this->Optional; }
  cmHeaderSet* GetHeaderSet() const { return this->HeaderSet; }
  cmGeneratorTarget* GetTarget() const { return this->Target; }

protected:
  void GenerateScriptForConfig(std::ostream& os, const std::string& config,
                               Indent indent) override;

private:
  std::string TargetName;
  cmLocalGenerator* LocalGenerator;
  cmHeaderSet* const HeaderSet;
  std::string const FilePermissions;
  bool const Optional;
  cmGeneratorTarget* Target;

  std::map<std::string, std::vector<std::string>> CalculateFilesPerDir(
    const std::string& config) const;
};
