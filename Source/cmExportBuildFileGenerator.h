/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#pragma once

#include "cmConfigure.h" // IWYU pragma: keep

#include <iosfwd>
#include <string>
#include <utility>
#include <vector>

#include "cmExportFileGenerator.h"
#include "cmStateTypes.h"

class cmExportSet;
class cmGeneratorTarget;
class cmGlobalGenerator;
class cmLocalGenerator;

/** \class cmExportBuildFileGenerator
 * \brief Generate a file exporting targets from a build tree.
 *
 * cmExportBuildFileGenerator generates a file exporting targets from
 * a build tree.  A single file exports information for all
 * configurations built.
 *
 * This is used to implement the EXPORT() command.
 */
class cmExportBuildFileGenerator : public cmExportFileGenerator
{
public:
  using base = cmExportFileGenerator;

  cmExportBuildFileGenerator();

  /** Set the list of targets to export.  */
  void SetTargets(std::vector<std::string> const& targets);
  void GetTargets(std::vector<std::string>& targets) const;
  void AppendTargets(std::vector<std::string> const& targets);
  void SetExportSet(cmExportSet*);

  /** Set whether to append generated code to the output file.  */
  void SetAppendMode(bool append) { this->AppendMode = append; }

  void Compute(cmLocalGenerator* lg);

protected:
  // Implement virtual methods from the superclass.
  bool GenerateMainFile(std::ostream& os) final override;
  void GenerateImportTargetCode(std::ostream& os,
                                cmGeneratorTarget const* target,
                                cmStateEnums::TargetType targetType) override;

  // new virtual methods
  virtual bool GenerateImportHeaderCode(std::ostream& os, Context& ctx);
  virtual bool GenerateImportFooterCode(std::ostream& os, Context& ctx);

  void GenerateImportTargetsConfig(std::ostream& os, std::string const& config,
                                   cmExportFileGenerator::Context& ctx,
                                   std::string const& suffix) override;
  cmStateEnums::TargetType GetExportTargetType(
    cmGeneratorTarget const* target) const;
  void HandleMissingTarget(std::string& link_libs,
                           std::vector<std::string>& missingTargets,
                           cmGeneratorTarget const* depender,
                           cmGeneratorTarget const* dependee) const override;

  /** Fill in properties indicating built file locations.  */
  void SetImportLocationProperty(const std::string& config,
                                 std::string const& suffix,
                                 cmGeneratorTarget* target,
                                 ImportPropertyMap& properties);

  std::string InstallNameDir(cmGeneratorTarget const* target,
                             const std::string& config) override;

  std::pair<std::vector<std::string>, std::string> FindBuildExportInfo(
    cmGlobalGenerator* gg, const std::string& name) const;

  bool GetTargets(std::vector<cmGeneratorTarget*>& targets) const;

private:
  void InvocationSyntax(std::ostream& os) const override;

  std::vector<std::string> Targets;
  cmExportSet* ExportSet;
  cmLocalGenerator* LG;
};
