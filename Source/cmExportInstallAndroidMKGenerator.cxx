/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#include <cstddef>
#include <memory>
#include <ostream>
#include <vector>

#include "cmExportInstallAndroidMKGenerator.h"
#include "cmExportSet.h"
#include "cmExportUtility.h"
#include "cmGeneratorTarget.h"
#include "cmInstallExportGenerator.h"
#include "cmInstallTargetGenerator.h"
#include "cmStateTypes.h"
#include "cmStringAlgorithms.h"
#include "cmSystemTools.h"
#include "cmTarget.h"
#include "cmTargetExport.h"

cmExportInstallAndroidMKGenerator::cmExportInstallAndroidMKGenerator(
  cmInstallExportGenerator* iegen)
  : cmExportInstallFileGenerator(iegen)
{
}

bool cmExportInstallAndroidMKGenerator::GenerateImportHeaderCode(
  std::ostream& os, Context&)
{
  std::string installDir = this->IEGen->GetDestination();
  os << "LOCAL_PATH := $(call my-dir)\n";
  size_t numDotDot = cmSystemTools::CountChar(installDir.c_str(), '/');
  numDotDot += installDir.empty() ? 0 : 1;
  std::string path;
  for (size_t n = 0; n < numDotDot; n++) {
    path += "/..";
  }
  os << "_IMPORT_PREFIX := "
     << "$(LOCAL_PATH)" << path << "\n\n";
  for (std::unique_ptr<cmTargetExport> const& te :
       this->IEGen->GetExportSet()->GetTargetExports()) {
    // Collect import properties for this target.
    if (te->Target->GetType() == cmStateEnums::INTERFACE_LIBRARY) {
      continue;
    }
    std::string dest;
    if (te->LibraryGenerator) {
      dest = te->LibraryGenerator->GetDestination("");
    }
    if (te->ArchiveGenerator) {
      dest = te->ArchiveGenerator->GetDestination("");
    }
    te->Target->Target->SetProperty("__dest", dest);
  }

  return true;
}

bool cmExportInstallAndroidMKGenerator::GenerateImportFooterCode(std::ostream&,
                                                                 Context&)
{
  return true;
}

bool cmExportInstallAndroidMKGenerator::GenerateTargetExportCode(
  std::ostream& os, Context& ctx, const cmTargetExport& target,
  cmStateEnums::TargetType targetType)
{
  this->GenerateImportTargetCode(os, target.Target, targetType);
  return base::GenerateTargetExportCode(os, ctx, target, targetType);
}

void cmExportInstallAndroidMKGenerator::GenerateImportTargetCode(
  std::ostream& os, cmGeneratorTarget const* target,
  cmStateEnums::TargetType /*targetType*/)
{
  std::string targetName = cmStrCat(this->Namespace, target->GetExportName());
  os << "include $(CLEAR_VARS)\n";
  os << "LOCAL_MODULE := ";
  os << targetName << "\n";
  os << "LOCAL_SRC_FILES := $(_IMPORT_PREFIX)/";
  os << target->Target->GetSafeProperty("__dest") << "/";
  std::string config;
  if (!this->Configurations.empty()) {
    config = this->Configurations[0];
  }
  os << target->GetFullName(config) << "\n";
}

void cmExportInstallAndroidMKGenerator::GenerateInterfaceProperties(
  cmGeneratorTarget const* target, std::ostream& os,
  const ImportPropertyMap& properties) const
{
  std::string config;
  if (!this->Configurations.empty()) {
    config = this->Configurations[0];
  }
  Exporting::AndroidMK::GenerateInterfaceProperties(
    *target, os, properties, Exporting::GenerateType::Install, config);
}

bool cmExportInstallAndroidMKGenerator::
  GeneratedFilesRequireVariableExpansion() const
{
  return false;
}

void cmExportInstallAndroidMKGenerator::GenerateImportTargetsConfig(
  std::ostream& /*os*/, std::string const& /*config*/,
  cmExportFileGenerator::Context& /*ctx*/, std::string const& /*suffix*/)
{
}
