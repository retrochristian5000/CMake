/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#include "cmExportInstallFileGenerator.h"

#include "cmExportSet.h"
#include "cmGeneratorTarget.h"
#include "cmGlobalGenerator.h"
#include "cmInstallExportGenerator.h"
#include "cmLocalGenerator.h"
#include "cmMakefile.h"
#include "cmTargetExport.h"

cmExportInstallFileGenerator::cmExportInstallFileGenerator(
  cmInstallExportGenerator* ieGen)
  : IEGen{ ieGen }
{
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
  const cmGeneratorTarget* depender, cmGeneratorTarget* dependee) const
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

void cmExportInstallFileGenerator::ComplainAboutMissingTarget(
  const cmGeneratorTarget* depender, cmGeneratorTarget* dependee,
  std::vector<std::string> const& exportFiles) const
{
  std::ostringstream e;
  e << "install(EXPORT \"" << this->IEGen->GetExportSet()->GetName()
    << "\" ...) "
    << "includes target \"" << depender->GetName()
    << "\" which requires target \"" << dependee->GetName() << "\" ";
  if (exportFiles.empty()) {
    e << "that is not in any export set.";
  } else {
    e << "that is not in this export set, but in multiple other export sets: "
      << cmJoin(exportFiles, ", ") << ".\n";
    e << "An exported target cannot depend upon another target which is "
         "exported multiple times. Consider consolidating the exports of the "
         "\""
      << dependee->GetName() << "\" target to a single export.";
  }
  cmSystemTools::Error(e.str());
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

cmExportFileGenerator::ImportPropertyMap
cmExportInstallFileGenerator::CommonInterfaceProperties(
  const cmTargetExport* te,
  cmGeneratorExpression::PreprocessContext preprocessRule,
  std::vector<std::string>& missingTargets) const
{
  ImportPropertyMap properties;
  this->PopulateCommonInterfaceProperties(te->Target, properties,
                                          te->InterfaceIncludeDirectories,
                                          preprocessRule, missingTargets);
  return properties;
}

cmExportFileGenerator::ImportPropertyMap
cmExportInstallFileGenerator::CommonInterfaceProperties(
  const cmGeneratorTarget* target,
  cmGeneratorExpression::PreprocessContext preprocessRule,
  std::vector<std::string>& missingTargets) const
{
  ImportPropertyMap properties;
  this->PopulateCommonInterfaceProperties(target, properties, {},
                                          preprocessRule, missingTargets);
  return properties;
}

void cmExportInstallFileGenerator::PopulateCommonInterfaceProperties(
  const cmGeneratorTarget* gt,
  cmExportFileGenerator::ImportPropertyMap& properties,
  const std::string& exportIncludeDirectories,
  cmGeneratorExpression::PreprocessContext preprocessRule,
  std::vector<std::string>& missingTargets) const
{
  this->PopulateIncludeDirectoriesInterface(
    gt, exportIncludeDirectories, preprocessRule, properties, missingTargets);
  this->PopulateInterfaceProperty("INTERFACE_SYSTEM_INCLUDE_DIRECTORIES", gt,
                                  preprocessRule, properties, missingTargets);
  this->PopulateInterfaceProperty("INTERFACE_COMPILE_DEFINITIONS", gt,
                                  preprocessRule, properties, missingTargets);
  this->PopulateInterfaceProperty("INTERFACE_COMPILE_OPTIONS", gt,
                                  preprocessRule, properties, missingTargets);
  this->PopulateInterfaceProperty("INTERFACE_PRECOMPILE_HEADERS", gt,
                                  preprocessRule, properties, missingTargets);
  this->PopulateInterfaceProperty("INTERFACE_AUTOUIC_OPTIONS", gt,
                                  preprocessRule, properties, missingTargets);
  this->PopulateInterfaceProperty("INTERFACE_COMPILE_FEATURES", gt,
                                  preprocessRule, properties, missingTargets);
  this->PopulateInterfaceProperty("INTERFACE_LINK_OPTIONS", gt, preprocessRule,
                                  properties, missingTargets);
  this->PopulateLinkDirectoriesInterface(gt, preprocessRule, properties,
                                         missingTargets);
  this->PopulateLinkDependsInterface(gt, preprocessRule, properties,
                                     missingTargets);
}
