/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#pragma once

#include "cmConfigure.h" // IWYU pragma: keep

#include <iosfwd>
#include <set>
#include <string>

#include "cmExportFileGenerator.h"
#include "cmExportInstallFileGenerator.h"
#include "cmStateTypes.h"

class cmGeneratorTarget;
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
  using Base = cmExportInstallFileGenerator;
  /** Construct with the export installer that will install the
      files.  */
  cmExportInstallCMakeConfigFileGenerator(cmInstallExportGenerator* iegen);

  // TODO make protected
  VariableExpansionSettings GeneratedFilesRequireVariableExpansion()
    const override;

protected:
  struct CMakeInstallContext : public InstallContext
  {
    bool require2_8_12 = false;
    bool require3_0_0 = false;
    bool require3_1_0 = false;
    bool requiresConfigFiles = false;
  };

  // Implement virtual methods from the superclass.
  bool GenerateImportHeaderCode(std::ostream& os,
                                InstallContext& ctx) override;
  bool GenerateImportFooterCode(std::ostream& os,
                                InstallContext& ctx) override;
  bool GenerateTargetExportCode(std::ostream& os, InstallContext& ctx,
                                const cmTargetExport& target,
                                cmStateEnums::TargetType targetType) override;
  void GenerateImportTargetCode(std::ostream& os,
                                cmGeneratorTarget const* target,
                                cmStateEnums::TargetType targetType) override;
  void GenerateImportTargetsConfig(std::ostream& os, std::string const& config,
                                   Context& ctx,
                                   std::string const& suffix) override;

  void ReplaceInstallPrefix(std::string& input) const override;

  /** Generate the relative import prefix.  */
  virtual void GenerateImportPrefix(std::ostream&);

  /** Generate the relative import prefix.  */
  virtual void LoadConfigFiles(std::ostream&);

  virtual void CleanupTemporaryVariables(std::ostream&);

  /** Generate a per-configuration file for the targets.  */
  virtual bool GenerateImportFileConfig(const std::string& config,
                                        Context& ctx);

  /** Fill in properties indicating installed file locations.  */
  void SetImportLocationProperty(const std::string& config,
                                 std::string const& suffix,
                                 cmInstallTargetGenerator* itgen,
                                 ImportPropertyMap& properties,
                                 std::set<std::string>& importedLocations);

  void GenerateCommonFileHeader(std::ostream& os,
                                const std::string& config) const;

  CMakeInstallContext* MakeAndSetupContext(Context* preCreated) const override;
};
