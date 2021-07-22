/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#pragma once

#include "cmConfigure.h" // IWYU pragma: keep

#include <iosfwd>
#include <string>

#include "cmExportInstallFileGenerator.h"
#include "cmStateTypes.h"

class cmTargetExport;
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
  // Implement virtual methods from the superclass.
  bool GenerateImportHeaderCode(std::ostream& os,
                                InstallContext& ctx) override;
  bool GenerateImportFooterCode(std::ostream& os,
                                InstallContext& ctx) override;
  bool GenerateTargetExportCode(std::ostream& os, InstallContext& ctx,
                                cmTargetExport const& target,
                                cmStateEnums::TargetType targetType) override;
  void GenerateImportTargetCode(std::ostream& os,
                                cmGeneratorTarget const* target,
                                cmStateEnums::TargetType targetType) override;
  void GenerateImportTargetsConfig(std::ostream& os, std::string const& config,
                                   Context& ctx,
                                   std::string const& suffix) override;

  VariableExpansionSettings GeneratedFilesRequireVariableExpansion()
    const override;
};
