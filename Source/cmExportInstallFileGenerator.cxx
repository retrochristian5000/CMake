/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#include "cmExportInstallFileGenerator.h"

#include <memory>
#include <set>
#include <sstream>

#include "cmExportSet.h"
#include "cmExportUtility.h"
#include "cmGeneratorExpression.h"
#include "cmGeneratorTarget.h"
#include "cmGlobalGenerator.h"
#include "cmInstallExportGenerator.h"
#include "cmLocalGenerator.h"
#include "cmMakefile.h"
#include "cmStringAlgorithms.h"
#include "cmSystemTools.h"
#include "cmTarget.h"
#include "cmTargetExport.h"

cmExportInstallFileGenerator::cmExportInstallFileGenerator(
  cmInstallExportGenerator* ieGen)
  : IEGen{ ieGen }
{
}

bool cmExportInstallFileGenerator::GenerateMainFile(std::ostream& os)
{
  const std::unique_ptr<InstallContext> ctx{ this->MakeAndSetupContext(
    nullptr) };

  if (!ctx) {
    return false;
  }

  if (!this->GenerateImportHeaderCode(os, *ctx)) {
    return false;
  }
  for (cmTargetExport const* te : ctx->TargetExports) {
    if (!this->GenerateTargetExportCode(os, *ctx, *te,
                                        Exporting::GetExportTargetType(*te))) {
      return false;
    }
  }
  return GenerateImportFooterCode(os, *ctx);
}

bool cmExportInstallFileGenerator::GenerateTargetExportCode(
  std::ostream& os, InstallContext& ctx, const cmTargetExport& target,
  cmStateEnums::TargetType /*targetType*/)
{
  ImportPropertyMap properties = this->CommonInterfaceProperties(
    target.Target, cmGeneratorExpression::InstallInterface, ctx, true);
  this->GenerateInterfaceProperties(target.Target, os, properties);
  return true;
}

std::string cmExportInstallFileGenerator::GetConfigImportFileGlob() const
{
  std::string glob = cmStrCat(this->FileBase, "-*", this->FileExt);
  return glob;
}

void cmExportInstallFileGenerator::SetImportFileForConfig(
  const std::string& config, const std::string& fileName)
{
  this->ConfigImportFiles[config] = fileName;
}

std::string cmExportInstallFileGenerator::InstallNameDir(
  const cmGeneratorTarget* target, const std::string& config)
{
  std::string install_name_dir;

  cmMakefile* mf = target->Target->GetMakefile();
  if (mf->IsOn("CMAKE_PLATFORM_HAS_INSTALLNAME")) {
    install_name_dir =
      target->GetInstallNameDirForInstallTree(config, "${_IMPORT_PREFIX}");
  }

  return install_name_dir;
}

void cmExportInstallFileGenerator::HandleMissingTarget(
  std::string& link_libs, std::vector<std::string>& missingTargets,
  cmGeneratorTarget const* depender, cmGeneratorTarget const* dependee) const
{
  const std::string name = dependee->GetName();
  cmGlobalGenerator* gg = dependee->GetLocalGenerator()->GetGlobalGenerator();
  auto exportInfo = this->FindNamespaces(gg, name);
  std::vector<std::string> const& exportFiles = exportInfo.first;
  if (exportFiles.size() == 1) {
    std::string missingTarget = exportInfo.second;

    missingTarget += dependee->GetExportName();
    link_libs += missingTarget;
    missingTargets.push_back(std::move(missingTarget));
  } else {
    // All exported targets should be known here and should be unique.
    // This is probably user-error.
    this->ComplainAboutMissingTarget(depender, dependee, exportFiles);
  }
}

std::pair<std::vector<std::string>, std::string>
cmExportInstallFileGenerator::FindNamespaces(cmGlobalGenerator* gg,
                                             const std::string& name) const
{
  std::vector<std::string> exportFiles;
  std::string ns;
  const cmExportSetMap& exportSets = gg->GetExportSets();

  for (auto const& expIt : exportSets) {
    const cmExportSet& exportSet = expIt.second;

    bool containsTarget = false;
    for (auto const& target : exportSet.GetTargetExports()) {
      if (name == target->TargetName) {
        containsTarget = true;
        break;
      }
    }

    if (containsTarget) {
      std::vector<cmInstallExportGenerator const*> const* installs =
        exportSet.GetInstallations();
      for (cmInstallExportGenerator const* install : *installs) {
        exportFiles.push_back(install->GetDestinationFile());
        ns = install->GetNamespace();
      }
    }
  }

  return { exportFiles, ns };
}

void cmExportInstallFileGenerator::InvocationSyntax(std::ostream& os) const
{
  os << "install(EXPORT \"" << this->IEGen->GetExportSet()->GetName()
     << "\" ...) includes";
}

cmListFileBacktrace cmExportInstallFileGenerator::GetBacktrace(
  cmMakefile const&) const
{
  return this->IEGen->GetBacktrace();
}

cmExportInstallFileGenerator::InstallContext*
cmExportInstallFileGenerator::MakeAndSetupContext(Context* preCreated) const
{
  std::vector<cmGeneratorTarget const*> exportedTargets;
  std::vector<cmTargetExport const*> allTargets;
  {
    std::set<cmGeneratorTarget*> uniqueExportedTargets;
    std::string expectedTargets;
    std::string sep;
    for (std::unique_ptr<cmTargetExport> const& te :
         this->IEGen->GetExportSet()->GetTargetExports()) {
      if (te->NamelinkOnly) {
        continue;
      }
      expectedTargets += sep + this->Namespace + te->Target->GetExportName();
      sep = " ";
      if (uniqueExportedTargets.insert(te->Target).second) {
        exportedTargets.push_back(te->Target);
        allTargets.push_back(te.get());
      } else {
        std::ostringstream e;
        e << "install(EXPORT \"" << this->IEGen->GetExportSet()->GetName()
          << "\" ...) "
          << "includes target \"" << te->Target->GetName()
          << "\" more than once in the export set.";
        cmSystemTools::Error(e.str());
        delete preCreated;
        return nullptr;
      }
    }
  }

  auto* result = this->NewOrPreCreated<InstallContext>(preCreated);

  result->ExportedTargets = std::move(exportedTargets);
  result->TargetExports = std::move(allTargets);
  result->Config =
    !this->Configurations.empty() ? this->Configurations[0] : std::string();
  return result;
}
