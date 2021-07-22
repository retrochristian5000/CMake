/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#pragma once

#include "cmConfigure.h" // IWYU pragma: keep

#include <iosfwd>
#include <set>
#include <string>
#include <vector>

#include "cmExportFileGenerator.h"
#include "cmStateTypes.h"

class cmGeneratorTarget;
class cmGlobalGenerator;
class cmMakefile;

class cmExportTryCompileFileGenerator : public cmExportFileGenerator
{
public:
  cmExportTryCompileFileGenerator(cmGlobalGenerator* gg,
                                  std::vector<std::string> const& targets,
                                  cmMakefile* mf,
                                  std::set<std::string> const& langs);

  /** Set the list of targets to export.  */
  void SetConfig(const std::string& config) { this->Config = config; }

protected:
  // Implement virtual methods from the superclass.
  bool GenerateMainFile(std::ostream& os) override;

  void GenerateImportTargetsConfig(std::ostream&, std::string const&, Context&,
                                   std::string const&) override
  {
  }

  void GenerateImportTargetCode(std::ostream& os,
                                cmGeneratorTarget const* target,
                                cmStateEnums::TargetType targetType) override;

  void HandleMissingTarget(std::string&, std::vector<std::string>&,
                           cmGeneratorTarget const*,
                           cmGeneratorTarget const*) const override
  {
  }

  void PopulateProperties(cmGeneratorTarget const* target,
                          ImportPropertyMap& properties,
                          std::set<const cmGeneratorTarget*>& emitted);

  std::string InstallNameDir(cmGeneratorTarget const* target,
                             const std::string& config) override;

  Context* MakeAndSetupContext(Context* preCreated) const override;

private:
  std::string FindTargets(const std::string& prop,
                          const cmGeneratorTarget* tgt,
                          std::string const& language,
                          std::set<const cmGeneratorTarget*>& emitted);
  void InvocationSyntax(std::ostream& os) const override;

  std::vector<cmGeneratorTarget const*> Exports;
  std::string Config;
  std::vector<std::string> Languages;
};
