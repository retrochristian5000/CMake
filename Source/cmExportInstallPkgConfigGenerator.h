/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#pragma once

#include "cmConfigure.h" // IWYU pragma: keep

#include <iosfwd>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "cmExportFileGenerator.h"
#include "cmExportInstallFileGenerator.h"
#include "cmStateTypes.h"

class cmGeneratorTarget;
class cmInstallExportGenerator;

/** \class cmExportInstallPkgConfigGenerator
 * \brief Generate a file exporting targets from an install tree.
 *
 * cmExportInstallPkgConfigGenerator generates files exporting targets from
 * install an installation tree.  The files are placed in a temporary
 * location for installation by cmInstallExportGenerator.  The file format
 * is for the pkg-config.
 *
 * This is used to implement the INSTALL(EXPORT_PKG_CONFIG) command.
 */
class cmExportInstallPkgConfigGenerator : public cmExportInstallFileGenerator
{
public:
  /** Construct with the export installer that will install the
      files.  */
  cmExportInstallPkgConfigGenerator(cmInstallExportGenerator* iegen);

protected:
  void ExportTarget(std::ostream& os, const cmTargetExport* te,
                    const std::string& config);

  // Implement virtual methods from the superclass.
  bool GenerateMainFile(std::ostream& os) override;
  void GeneratePolicyHeaderCode(std::ostream& os) override;
  void GenerateImportHeaderCode(std::ostream& os,
                                const std::string& config) override;
  void GenerateImportFooterCode(std::ostream& os) override;
  void GeneratePolicyFooterCode(std::ostream& os) override;
  void GenerateImportTargetsConfig(
    std::ostream& os, const std::string& config, const std::string& suffix,
    std::vector<std::string>& missingTargets) override;

  bool GeneratedFilesRequireVariableExpansion() const override;

private:
  struct PkgConfigTargetData;

  void PopulateGetPkgConfigData(const cmGeneratorTarget* target,
                                const std::string& config,
                                PkgConfigTargetData& data) const;
  std::pair<std::vector<std::string>, std::vector<std::string>> SplitLinkLibs(
    const std::vector<std::string>& items) const;

  static void ReplaceImportPrefix(std::string& input);
  static std::string CompileFeatureToCompileFlag(
    const cmGeneratorTarget* target, const std::string& compileFeature);
  static std::vector<std::string> RemoveImplicitEntries(
    const cmGeneratorTarget* target, const std::string& entryName,
    const std::vector<std::string>& entries);
};
