/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#pragma once

#include "cmConfigure.h" // IWYU pragma: keep

#include <iosfwd>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "cmExportInstallFileGenerator.h"
#include "cmStateTypes.h"

class cmGeneratorTarget;
class cmGlobalGenerator;
class cmInstallExportGenerator;
class cmInstallTargetGenerator;
class cmTargetExport;

/** \class cmExportInstallCMakeConfigFileGenerator
 * \brief Generate a file exporting targets from an install tree.
 *
 * cmExportInstallCMakeConfigFileGenerator generates files exporting targets
 * from install an installation tree.  The files are placed in a temporary
 * location for installation by cmExportInstallCMakeConfigFileGenerator.  One
 * main file is generated that creates the imported targets and loads
 * per-configuration files.  Target locations and settings for each
 * configuration are written to these per-configuration files.  After
 * installation the main file loads the configurations that have been
 * installed.
 *
 * This is used to implement the the CMAKE_CONFIG mode of the
 * INSTALL(EXPORT) command.
 */
class cmExportInstallCMakeConfigFileGenerator
  : public cmExportInstallFileGenerator
{
public:
  /** Construct with the export installer that will install the
      files.  */
  cmExportInstallCMakeConfigFileGenerator(cmInstallExportGenerator* iegen);

  // TODO make protected
  bool GeneratedFilesRequireVariableExpansion() const override;

protected:
  // Implement virtual methods from the superclass.
  bool GenerateMainFile(std::ostream& os) override;
  void GenerateImportTargetsConfig(
    std::ostream& os, const std::string& config, std::string const& suffix,
    std::vector<std::string>& missingTargets) override;
  cmStateEnums::TargetType GetExportTargetType(
    cmTargetExport const* targetExport) const;

  void ReplaceInstallPrefix(std::string& input) const override;

  /** Generate the relative import prefix.  */
  virtual void GenerateImportPrefix(std::ostream&);

  /** Generate the relative import prefix.  */
  virtual void LoadConfigFiles(std::ostream&);

  virtual void CleanupTemporaryVariables(std::ostream&);

  /** Generate a per-configuration file for the targets.  */
  virtual bool GenerateImportFileConfig(
    const std::string& config, std::vector<std::string>& missingTargets);

  /** Fill in properties indicating installed file locations.  */
  void SetImportLocationProperty(const std::string& config,
                                 std::string const& suffix,
                                 cmInstallTargetGenerator* itgen,
                                 ImportPropertyMap& properties,
                                 std::set<std::string>& importedLocations);
};
