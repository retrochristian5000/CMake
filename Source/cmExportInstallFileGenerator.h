/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#pragma once

#include "cmConfigure.h" // IWYU pragma: keep

#include "cmExportFileGenerator.h"
#include "cmStateTypes.h"

class cmGeneratorTarget;
class cmGlobalGenerator;
class cmInstallExportGenerator;
class cmInstallTargetGenerator;
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
  virtual bool GeneratedFilesRequireVariableExpansion() const = 0;

protected:
  std::string InstallNameDir(const cmGeneratorTarget* target,
                             const std::string& config) override;

  void HandleMissingTarget(std::string& link_libs,
                           std::vector<std::string>& missingTargets,
                           const cmGeneratorTarget* depender,
                           cmGeneratorTarget* dependee) const override;

  void ComplainAboutMissingTarget(
    const cmGeneratorTarget* depender, cmGeneratorTarget* dependee,
    std::vector<std::string> const& exportFiles) const;
  std::pair<std::vector<std::string>, std::string> FindNamespaces(
    cmGlobalGenerator* gg, const std::string& name) const;

  ImportPropertyMap CommonInterfaceProperties(
    const cmGeneratorTarget* target,
    cmGeneratorExpression::PreprocessContext preprocessRule,
    std::vector<std::string>& missingTargets) const;

  ImportPropertyMap CommonInterfaceProperties(
    const cmTargetExport* te,
    cmGeneratorExpression::PreprocessContext preprocessRule,
    std::vector<std::string>& missingTargets) const;

  cmInstallExportGenerator* IEGen;

private:
  void PopulateCommonInterfaceProperties(
    const cmGeneratorTarget* target, ImportPropertyMap& properties,
    const std::string& exportIncludeDirectories,
    cmGeneratorExpression::PreprocessContext preprocessRule,
    std::vector<std::string>& missingTargets) const;

  // The import file generated for each configuration.
  std::map<std::string, std::string> ConfigImportFiles;
};
