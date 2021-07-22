/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#pragma once

#include "cmConfigure.h" // IWYU pragma: keep

#include <iosfwd>
#include <string>
#include <vector>

#include "cmExportBuildFileGenerator.h"
#include "cmExportFileGenerator.h"
#include "cmStateTypes.h"

class cmGeneratorTarget;

/** \class cmExportBuildAndroidMKGenerator
 * \brief Generate a file exporting targets from a build tree.
 *
 * cmExportBuildAndroidMKGenerator generates a file exporting targets from
 * a build tree.  This exports the targets to the Android ndk build tool
 * makefile format for prebuilt libraries.
 *
 * This is used to implement the EXPORT() command.
 */
class cmExportBuildAndroidMKGenerator : public cmExportBuildFileGenerator
{
public:
  cmExportBuildAndroidMKGenerator();
  // this is so cmExportInstallAndroidMKGenerator can share this
  // function as they are almost the same

protected:
  // Implement virtual methods from the superclass.
  bool GenerateImportHeaderCode(std::ostream& os, Context& ctx) override;
  bool GenerateImportFooterCode(std::ostream&, Context&) override;
  void GenerateImportTargetCode(
    std::ostream& os, cmGeneratorTarget const* target,
    cmStateEnums::TargetType /*targetType*/) override;
  void GenerateImportTargetsConfig(std::ostream&, const std::string&,
                                   cmExportFileGenerator::Context&,
                                   const std::string&) override;
  void GenerateInterfaceProperties(
    cmGeneratorTarget const* target, std::ostream& os,
    const ImportPropertyMap& properties) const override;
};
