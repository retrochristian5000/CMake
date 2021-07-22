/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#include "cmExportInstallPkgConfigGenerator.h"

#include <sstream>

#include "cmExportSet.h"
#include "cmExportUtility.h"
#include "cmGeneratorExpression.h"
#include "cmGeneratorTarget.h"
#include "cmGlobalGenerator.h"
#include "cmInstallExportGenerator.h"
#include "cmMessageType.h"
#include "cmStateTypes.h"
#include "cmSystemTools.h"
#include "cmTargetExport.h"
#include "cmake.h"

namespace EPC = Exporting::PkgConfig;

cmExportInstallPkgConfigGenerator::cmExportInstallPkgConfigGenerator(
  cmInstallExportGenerator* iegen)
  : cmExportInstallFileGenerator(iegen)
{
}

bool cmExportInstallPkgConfigGenerator::GenerateTargetExportCode(
  std::ostream& os, InstallContext& ctx, const cmTargetExport& target,
  cmStateEnums::TargetType targetType)
{
  cmGeneratorTarget const* gt = target.Target;

  if (!Exporting::PkgConfig::CanBeExported(*gt, targetType)) {
    const std::string er =
      "Object libraries and targets with non-empty "
      "'INTERFACE_SOURCES' property can not be represented in pkg-config "
      "format and thus can not be exported to that format";
    gt->GetGlobalGenerator()->GetCMakeInstance()->IssueMessage(
      MessageType::FATAL_ERROR, er, gt->GetBacktrace());
    return false;
  }

  if (this->ExportOld) {
    // TODO
  }

  return Exporting::PkgConfig::GenerateTargetExportCode(
    os, *target.Target, targetType, ctx.Config,
    [this, &ctx](cmGeneratorTarget const& tgt) {
      ImportPropertyMap properties = this->CommonInterfaceProperties(
        &tgt, cmGeneratorExpression::InstallInterface, ctx, true);
      this->PopulateInterfaceProperty("INTERFACE_POSITION_INDEPENDENT_CODE",
                                      &tgt, properties);
      return properties;
    });
}

cmExportInstallFileGenerator::VariableExpansionSettings
cmExportInstallPkgConfigGenerator::GeneratedFilesRequireVariableExpansion()
  const
{
  return { true, true };
}

bool cmExportInstallPkgConfigGenerator::GenerateImportHeaderCode(
  std::ostream& os, InstallContext& ctx)
{
  if (ctx.TargetExports.size() > 1) {
    std::ostringstream e;
    e << "Export \"" << this->IEGen->GetExportSet()->GetName()
      << "\" contains " << ctx.TargetExports.size()
      << " targets. "
         "install(EXPORT ...) does not yet support multi-target "
         "export sets in the PKG_CONFIG_MODE";
    cmSystemTools::Error(e.str());
    return false;
  }

  os << "prefix=@CMAKE_INSTALL_PREFIX@" << std::endl;
  os << std::endl;

  EPC::GenerateExportMetadata(os, *ctx.TargetExports.front()->Target);

  return true;
}

bool cmExportInstallPkgConfigGenerator::GenerateImportFooterCode(
  std::ostream& /*os*/, InstallContext& /*ctx*/)
{
  return true;
}

void cmExportInstallPkgConfigGenerator::GenerateImportTargetCode(
  std::ostream&, cmGeneratorTarget const*, cmStateEnums::TargetType)
{
}

void cmExportInstallPkgConfigGenerator::GenerateImportTargetsConfig(
  std::ostream&, std::string const&, cmExportFileGenerator::Context&,
  std::string const&)
{
}
