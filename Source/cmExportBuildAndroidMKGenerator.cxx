/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#include "cmExportBuildAndroidMKGenerator.h"

#include <sstream>
#include <vector>

#include "cmExportUtility.h"
#include "cmGeneratorTarget.h"
#include "cmStateTypes.h"
#include "cmStringAlgorithms.h"
#include "cmSystemTools.h"

cmExportBuildAndroidMKGenerator::cmExportBuildAndroidMKGenerator()
{
}

bool cmExportBuildAndroidMKGenerator::GenerateImportHeaderCode(
  std::ostream& os, Context&)
{
  os << "LOCAL_PATH := $(call my-dir)\n\n";
  return true;
}

void cmExportBuildAndroidMKGenerator::GenerateImportTargetCode(
  std::ostream& os, cmGeneratorTarget const* target,
  cmStateEnums::TargetType /*targetType*/)
{
  std::string targetName = cmStrCat(this->Namespace, target->GetExportName());
  os << "include $(CLEAR_VARS)\n";
  os << "LOCAL_MODULE := ";
  os << targetName << "\n";
  os << "LOCAL_SRC_FILES := ";
  std::string const noConfig; // FIXME: What config to use here?
  std::string path =
    cmSystemTools::ConvertToOutputPath(target->GetFullPath(noConfig));
  os << path << "\n";
}

void cmExportBuildAndroidMKGenerator::GenerateInterfaceProperties(
  const cmGeneratorTarget* target, std::ostream& os,
  const ImportPropertyMap& properties) const
{
  std::string config;
  if (!this->Configurations.empty()) {
    config = this->Configurations[0];
  }
  Exporting::AndroidMK::GenerateInterfaceProperties(
    *target, os, properties, Exporting::GenerateType::Build, config);
}

void cmExportBuildAndroidMKGenerator::GenerateImportTargetsConfig(
  std::ostream&, const std::string&, cmExportFileGenerator::Context&,
  const std::string&)
{
}

bool cmExportBuildAndroidMKGenerator::GenerateImportFooterCode(
  std::ostream&, cmExportFileGenerator::Context&)
{
  return true;
}
