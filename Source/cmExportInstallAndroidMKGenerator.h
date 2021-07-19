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
public:
  /** Construct with the export installer that will install the
      files.  */
  cmExportInstallAndroidMKGenerator(cmInstallExportGenerator* iegen);

protected:
  // Implement virtual methods from the superclass.
  void GenerateImportHeaderCode(std::ostream& os,
                                const std::string& config = "") override;
  void GenerateImportFooterCode(std::ostream& os) override;
  void GenerateImportTargetCode(
    std::ostream& os, cmGeneratorTarget const* target,
    cmStateEnums::TargetType /*targetType*/) override;
  void GenerateExpectedTargetsCode(
    std::ostream& os, const std::string& expectedTargets) override;
  void GenerateInterfaceProperties(
    cmGeneratorTarget const* target, std::ostream& os,
    const ImportPropertyMap& properties) const override;
  bool GeneratedFilesRequireVariableExpansion() const override;

  bool GenerateMainFile(std::ostream& os) override;
  void GenerateImportTargetsConfig(
    std::ostream& os, const std::string& config, const std::string& suffix,
    std::vector<std::string>& missingTargets) override;
};
