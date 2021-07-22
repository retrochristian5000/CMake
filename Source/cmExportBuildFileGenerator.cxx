/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#include "cmExportBuildFileGenerator.h"

#include <algorithm>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <utility>

#include <cmext/algorithm>

#include "cmExportSet.h"
#include "cmExportUtility.h"
#include "cmGeneratorExpression.h"
#include "cmGeneratorTarget.h"
#include "cmGlobalGenerator.h"
#include "cmLocalGenerator.h"
#include "cmMakefile.h"
#include "cmMessageType.h"
#include "cmPolicies.h"
#include "cmStateTypes.h"
#include "cmStringAlgorithms.h"
#include "cmTarget.h"
#include "cmTargetExport.h"
#include "cmake.h"

class cmSourceFile;

cmExportBuildFileGenerator::cmExportBuildFileGenerator()
{
  this->LG = nullptr;
  this->ExportSet = nullptr;
}

void cmExportBuildFileGenerator::Compute(cmLocalGenerator* lg)
{
  this->LG = lg;
  if (this->ExportSet) {
    this->ExportSet->Compute(lg);
  }
}

bool cmExportBuildFileGenerator::GenerateMainFile(std::ostream& os)
{
  std::vector<cmGeneratorTarget*> exportedTargets;
  if (!this->GetTargets(exportedTargets)) {
    return false;
  }

  Context ctx{ exportedTargets, {} };

  this->GenerateImportHeaderCode(os, ctx);

  // Create all the imported targets.
  for (cmGeneratorTarget const* gt : ctx.ExportedTargets) {
    this->GenerateImportTargetCode(os, gt, this->GetExportTargetType(gt));

    gt->Target->AppendBuildInterfaceIncludes();

    ImportPropertyMap properties;

    this->PopulateInterfaceProperty("INTERFACE_INCLUDE_DIRECTORIES", gt,
                                    cmGeneratorExpression::BuildInterface,
                                    properties, ctx);
    this->PopulateInterfaceProperty("INTERFACE_SOURCES", gt,
                                    cmGeneratorExpression::BuildInterface,
                                    properties, ctx);
    this->PopulateInterfaceProperty("INTERFACE_COMPILE_DEFINITIONS", gt,
                                    cmGeneratorExpression::BuildInterface,
                                    properties, ctx);
    this->PopulateInterfaceProperty("INTERFACE_COMPILE_OPTIONS", gt,
                                    cmGeneratorExpression::BuildInterface,
                                    properties, ctx);
    this->PopulateInterfaceProperty("INTERFACE_PRECOMPILE_HEADERS", gt,
                                    cmGeneratorExpression::BuildInterface,
                                    properties, ctx);
    this->PopulateInterfaceProperty("INTERFACE_AUTOUIC_OPTIONS", gt,
                                    cmGeneratorExpression::BuildInterface,
                                    properties, ctx);
    this->PopulateInterfaceProperty("INTERFACE_COMPILE_FEATURES", gt,
                                    cmGeneratorExpression::BuildInterface,
                                    properties, ctx);
    this->PopulateInterfaceProperty("INTERFACE_LINK_OPTIONS", gt,
                                    cmGeneratorExpression::BuildInterface,
                                    properties, ctx);
    this->PopulateInterfaceProperty("INTERFACE_LINK_DIRECTORIES", gt,
                                    cmGeneratorExpression::BuildInterface,
                                    properties, ctx);
    this->PopulateInterfaceProperty("INTERFACE_LINK_DEPENDS", gt,
                                    cmGeneratorExpression::BuildInterface,
                                    properties, ctx);
    this->PopulateInterfaceProperty("INTERFACE_POSITION_INDEPENDENT_CODE", gt,
                                    properties);

    std::string errorMessage;
    if (!this->PopulateExportProperties(gt, properties, errorMessage)) {
      this->LG->GetGlobalGenerator()->GetCMakeInstance()->IssueMessage(
        MessageType::FATAL_ERROR, errorMessage,
        this->LG->GetMakefile()->GetBacktrace());
      return false;
    }

    const bool newCMP0022Behavior =
      gt->GetPolicyStatusCMP0022() != cmPolicies::WARN &&
      gt->GetPolicyStatusCMP0022() != cmPolicies::OLD;
    if (newCMP0022Behavior) {
      this->PopulateInterfaceLinkLibrariesProperty(
        gt, cmGeneratorExpression::BuildInterface, properties, ctx);
    }
    this->PopulateCompatibleInterfaceProperties(gt, properties);

    this->GenerateInterfaceProperties(gt, os, properties);
  }

  // Generate import file content for each configuration.
  for (std::string const& c : this->Configurations) {
    this->GenerateImportConfig(os, c, ctx);
  }

  return GenerateImportFooterCode(os, ctx);
}

void cmExportBuildFileGenerator::GenerateImportTargetCode(
  std::ostream& os, const cmGeneratorTarget* target,
  cmStateEnums::TargetType targetType)
{
}

bool cmExportBuildFileGenerator::GenerateImportHeaderCode(
  std::ostream& os, cmExportFileGenerator::Context& ctx)
{
  std::string expectedTargets;
  std::string sep;
  bool generatedInterfaceRequired = false;
  for (cmGeneratorTarget const* te : ctx.ExportedTargets) {
    expectedTargets += sep + this->Namespace + te->GetExportName();
    sep = " ";
    generatedInterfaceRequired |=
      this->GetExportTargetType(te) == cmStateEnums::INTERFACE_LIBRARY;
  }

  if (generatedInterfaceRequired) {
    Exporting::CMakeConfig::GenerateRequiredCMakeVersion(os, "3.0.0");
  }
  Exporting::CMakeConfig::GenerateExpectedTargetsCode(os, expectedTargets);
  return true;
}

bool cmExportBuildFileGenerator::GenerateImportFooterCode(
  std::ostream& os, cmExportFileGenerator::Context& ctx)
{
  Exporting::CMakeConfig::GenerateMissingTargetsCheckCode(os,
                                                          ctx.MissingTargets);
  return true;
}

void cmExportBuildFileGenerator::GenerateImportTargetsConfig(
  std::ostream& os, const std::string& config, Context& ctx,
  std::string const& suffix)
{
  std::vector<cmGeneratorTarget*> exportedTargets;
  // TODO: refactor to make this call unneeded
  if (!this->GetTargets(exportedTargets)) {
    return;
  }

  for (cmGeneratorTarget* target : exportedTargets) {
    // Collect import properties for this target.
    ImportPropertyMap properties;

    if (this->GetExportTargetType(target) != cmStateEnums::INTERFACE_LIBRARY) {
      this->SetImportLocationProperty(config, suffix, target, properties);
    }
    if (!properties.empty()) {
      // Get the rest of the target details.
      if (this->GetExportTargetType(target) !=
          cmStateEnums::INTERFACE_LIBRARY) {
        this->SetImportDetailProperties(config, suffix, target, properties,
                                        ctx);
        this->SetImportLinkInterface(config, suffix,
                                     cmGeneratorExpression::BuildInterface,
                                     target, properties, ctx);
      }

      // TODO: PUBLIC_HEADER_LOCATION
      // This should wait until the build feature propagation stuff
      // is done.  Then this can be a propagated include directory.
      // this->GenerateImportProperty(config, te->HeaderGenerator,
      //                              properties);

      // Generate code in the export file.
      Exporting::CMakeConfig::GenerateImportPropertyCode(
        os, config, { this->Namespace, *target }, properties);
    }
  }
}

cmStateEnums::TargetType cmExportBuildFileGenerator::GetExportTargetType(
  cmGeneratorTarget const* target) const
{
  cmStateEnums::TargetType targetType = target->GetType();
  // An object library exports as an interface library if we cannot
  // tell clients where to find the objects.  This is sufficient
  // to support transitive usage requirements on other targets that
  // use the object library.
  if (targetType == cmStateEnums::OBJECT_LIBRARY &&
      !this->LG->GetGlobalGenerator()->HasKnownObjectFileLocation(nullptr)) {
    targetType = cmStateEnums::INTERFACE_LIBRARY;
  }
  return targetType;
}

void cmExportBuildFileGenerator::AppendTargets(
  std::vector<std::string> const& targets)
{
  std::copy(targets.begin(), targets.end(), std::back_inserter(this->Targets));
}

void cmExportBuildFileGenerator::SetTargets(
  std::vector<std::string> const& targets)
{
  this->Targets = targets;
}

void cmExportBuildFileGenerator::SetExportSet(cmExportSet* exportSet)
{
  this->ExportSet = exportSet;
}

void cmExportBuildFileGenerator::SetImportLocationProperty(
  const std::string& config, std::string const& suffix,
  cmGeneratorTarget* target, ImportPropertyMap& properties)
{
  // Get the makefile in which to lookup target information.
  cmMakefile* mf = target->Makefile;

  if (target->GetType() == cmStateEnums::OBJECT_LIBRARY) {
    std::string prop = cmStrCat("IMPORTED_OBJECTS", suffix);

    // Compute all the object files inside this target and setup
    // IMPORTED_OBJECTS as a list of object files
    std::vector<cmSourceFile const*> objectSources;
    target->GetObjectSources(objectSources, config);
    std::string const obj_dir = target->GetObjectDirectory(config);
    std::vector<std::string> objects;
    for (cmSourceFile const* sf : objectSources) {
      const std::string& obj = target->GetObjectName(sf);
      objects.push_back(obj_dir + obj);
    }

    // Store the property.
    properties[prop] = cmJoin(objects, ";");
  } else {
    // Add the main target file.
    {
      std::string prop = cmStrCat("IMPORTED_LOCATION", suffix);
      std::string value;
      if (target->IsAppBundleOnApple()) {
        value =
          target->GetFullPath(config, cmStateEnums::RuntimeBinaryArtifact);
      } else {
        value = target->GetFullPath(config,
                                    cmStateEnums::RuntimeBinaryArtifact, true);
      }
      properties[prop] = value;
    }

    // Add the import library for windows DLLs.
    if (target->HasImportLibrary(config)) {
      std::string prop = cmStrCat("IMPORTED_IMPLIB", suffix);
      std::string value =
        target->GetFullPath(config, cmStateEnums::ImportLibraryArtifact);
      if (mf->GetDefinition("CMAKE_IMPORT_LIBRARY_SUFFIX")) {
        target->GetImplibGNUtoMS(config, value, value,
                                 "${CMAKE_IMPORT_LIBRARY_SUFFIX}");
      }
      properties[prop] = value;
    }
  }
}

void cmExportBuildFileGenerator::HandleMissingTarget(
  std::string& link_libs, std::vector<std::string>& missingTargets,
  cmGeneratorTarget const* depender, cmGeneratorTarget const* dependee) const
{
  // The target is not in the export.
  if (!this->AppendMode) {
    const std::string name = dependee->GetName();
    cmGlobalGenerator* gg =
      dependee->GetLocalGenerator()->GetGlobalGenerator();
    auto exportInfo = this->FindBuildExportInfo(gg, name);
    std::vector<std::string> const& exportFiles = exportInfo.first;

    if (exportFiles.size() == 1) {
      std::string missingTarget = exportInfo.second;

      missingTarget += dependee->GetExportName();
      link_libs += missingTarget;
      missingTargets.push_back(std::move(missingTarget));
      return;
    }
    // We are not appending, so all exported targets should be
    // known here.  This is probably user-error.
    this->ComplainAboutMissingTarget(depender, dependee, exportFiles);
  }
  // Assume the target will be exported by another command.
  // Append it with the export namespace.
  link_libs += this->Namespace;
  link_libs += dependee->GetExportName();
}

void cmExportBuildFileGenerator::GetTargets(
  std::vector<std::string>& targets) const
{
  if (this->ExportSet) {
    for (std::unique_ptr<cmTargetExport> const& te :
         this->ExportSet->GetTargetExports()) {
      if (te->NamelinkOnly) {
        continue;
      }
      targets.push_back(te->TargetName);
    }
    return;
  }
  targets = this->Targets;
}

std::pair<std::vector<std::string>, std::string>
cmExportBuildFileGenerator::FindBuildExportInfo(cmGlobalGenerator* gg,
                                                const std::string& name) const
{
  std::vector<std::string> exportFiles;
  std::string ns;

  auto& exportSets = gg->GetBuildExportSets();

  for (auto const& exp : exportSets) {
    const auto& exportSet = exp.second;
    std::vector<std::string> targets;
    exportSet->GetTargets(targets);
    if (cm::contains(targets, name)) {
      exportFiles.push_back(exp.first);
      ns = exportSet->GetNamespace();
    }
  }

  return { exportFiles, ns };
}

std::string cmExportBuildFileGenerator::InstallNameDir(
  cmGeneratorTarget const* target, const std::string& config)
{
  std::string install_name_dir;

  cmMakefile* mf = target->Target->GetMakefile();
  if (mf->IsOn("CMAKE_PLATFORM_HAS_INSTALLNAME")) {
    install_name_dir = target->GetInstallNameDirForBuildTree(config);
  }

  return install_name_dir;
}

bool cmExportBuildFileGenerator::GetTargets(
  std::vector<cmGeneratorTarget*>& targets) const
{
  std::vector<std::string> targetNames;
  this->GetTargets(targetNames);
  std::set<cmGeneratorTarget const*> uniqueTargets;

  for (std::string const& tei : targetNames) {
    cmGeneratorTarget* te = this->LG->FindGeneratorTargetToUse(tei);
    if (!uniqueTargets.insert(te).second) {
      std::ostringstream e;
      e << "given target \"" << te->GetName() << "\" more than once.";
      this->LG->GetGlobalGenerator()->GetCMakeInstance()->IssueMessage(
        MessageType::FATAL_ERROR, e.str(),
        this->LG->GetMakefile()->GetBacktrace());
      return false;
    } else {
      targets.push_back(te);
    }
  }
  return true;
}

void cmExportBuildFileGenerator::InvocationSyntax(std::ostream& os) const
{
  os << "export(...) called with";
}
