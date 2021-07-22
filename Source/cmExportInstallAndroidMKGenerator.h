/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#pragma once

#include "cmConfigure.h" // IWYU pragma: keep

#include <iosfwd>
#include <set>
#include <string>
#include <vector>

#include "cmExportFileGenerator.h"
#include "cmExportInstallFileGenerator.h"
#include "cmStateTypes.h"

class cmGeneratorTarget;
class cmInstallExportGenerator;

/** \class cmExportInstallAndroidMKGenerator
 * \brief Generate a file exporting targets from an install tree.
 *
 * cmExportInstallAndroidMKGenerator generates files exporting targets from
 * install an installation tree.  The files are placed in a temporary
 * location for installation by cmInstallExportGenerator.  The file format
 * is for the ndk build system and is a makefile fragment specifying prebuilt
 * libraries to the ndk build system.
 *
 * This is used to implement the INSTALL(EXPORT_ANDROID_MK) command.
 */
class cmExportInstallAndroidMKGenerator : public cmExportInstallFileGenerator
{
  using base = cmExportInstallFileGenerator;

public:
  /** Construct with the export installer that will install the
      files.  */
  cmExportInstallAndroidMKGenerator(cmInstallExportGenerator* iegen);

protected:
  // Implement virtual methods from the superclass.
  bool GenerateImportHeaderCode(std::ostream& os, Context& ctx) override;
  bool GenerateImportFooterCode(std::ostream& os, Context& ctx) override;
  bool GenerateTargetExportCode(std::ostream& os, Context& ctx,
                                const cmTargetExport& target,
                                cmStateEnums::TargetType targetType) override;
  void GenerateImportTargetCode(
    std::ostream& os, cmGeneratorTarget const* target,
    cmStateEnums::TargetType /*targetType*/) override;
  void GenerateInterfaceProperties(
    cmGeneratorTarget const* target, std::ostream& os,
    const ImportPropertyMap& properties) const override;
  bool GeneratedFilesRequireVariableExpansion() const override;

  void GenerateImportTargetsConfig(std::ostream& os, const std::string& config,
                                   cmExportFileGenerator::Context& ctx,
                                   const std::string& suffix) override;
};
