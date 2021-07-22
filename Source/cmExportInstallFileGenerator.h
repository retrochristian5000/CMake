/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#pragma once

#include "cmConfigure.h" // IWYU pragma: keep

#include <iosfwd>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "cmExportFileGenerator.h"
#include "cmStateTypes.h"

class cmGeneratorTarget;
class cmGlobalGenerator;
class cmInstallExportGenerator;
class cmTargetExport;

/** \class cmExportInstallFileGenerator
 * \brief Generate a file exporting targets from an install tree.
 *
 * cmExportInstallFileGenerator generates files exporting targets from
 * install an installation tree.  The files are placed in a temporary
 * location for installation by cmInstallExportGenerator.  One main
 * file is generated that creates the imported targets and loads
 * per-configuration files.  Target locations and settings for each
 * configuration are written to these per-configuration files.  After
 * installation the main file loads the configurations that have been
 * installed.
 *
 * This is used to implement the INSTALL(EXPORT) command.
 */
class cmExportInstallFileGenerator : public cmExportFileGenerator
{
public:
  using Base = cmExportFileGenerator;

  cmExportInstallFileGenerator(cmInstallExportGenerator* ieGen);

  /** Get the per-config file generated for each configuration.  This
      maps from the configuration name to the file temporary location
      for installation.  */
  std::map<std::string, std::string> const& GetConfigImportFiles()
  {
    return this->ConfigImportFiles;
  }

  void SetImportFileForConfig(const std::string& config,
                              const std::string& fileName);

  /** Compute the globbing expression used to load per-config import
      files from the main file.  */
  virtual std::string GetConfigImportFileGlob() const;
  struct VariableExpansionSettings
  {
    bool Required;
    bool AtOnly;
  };
  virtual VariableExpansionSettings GeneratedFilesRequireVariableExpansion()
    const = 0;

  bool GenerateMainFile(std::ostream& os) final;

protected:
  struct InstallContext : public Context
  {
    std::vector<cmTargetExport const*> TargetExports;
    std::string Config;
  };

  InstallContext* MakeAndSetupContext(Context* preCreated) const override;

  // new virtual methods
  virtual bool GenerateImportHeaderCode(std::ostream& os,
                                        InstallContext& ctx) = 0;
  virtual bool GenerateImportFooterCode(std::ostream& os,
                                        InstallContext& ctx) = 0;

  virtual bool GenerateTargetExportCode(std::ostream& os, InstallContext& ctx,
                                        cmTargetExport const& target,
                                        cmStateEnums::TargetType targetType);

  std::string InstallNameDir(const cmGeneratorTarget* target,
                             const std::string& config) override;

  void HandleMissingTarget(std::string& link_libs,
                           std::vector<std::string>& missingTargets,
                           cmGeneratorTarget const* depender,
                           cmGeneratorTarget const* dependee) const override;

  std::pair<std::vector<std::string>, std::string> FindNamespaces(
    cmGlobalGenerator* gg, const std::string& name) const;
  cmListFileBacktrace GetBacktrace(cmMakefile const&) const override;

  cmInstallExportGenerator* IEGen;

private:
  void InvocationSyntax(std::ostream& os) const override;

  // The import file generated for each configuration.
  std::map<std::string, std::string> ConfigImportFiles;
};
