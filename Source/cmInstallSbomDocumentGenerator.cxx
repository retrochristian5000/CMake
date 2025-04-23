/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file LICENSE.rst or https://cmake.org/licensing for details.  */
#include "cmInstallSbomDocumentGenerator.h"

#include <functional>
#include <ostream>
#include <typeindex>
#include <unordered_map>
#include <utility>

#include <cm/memory>

#include "cmInstallFilesGenerator.h"
#include "cmListFileCache.h"
#include "cmLocalGenerator.h"
#include "cmMakefile.h"
#include "cmMessageType.h"
#include "cmMessenger.h"
#include "cmSbom.h"
#include "cmScriptGenerator.h"
#include "cmStateSnapshot.h"
#include "cmStringAlgorithms.h"
#include "cmSystemTools.h"
#include "cmake.h"

class cmInstallDirectoryGenerator;
class cmInstallTargetGenerator;

namespace {

// Helps with obtaining SBOM information from CMake install generators
class InstallSbomHelper
{
public:
  InstallSbomHelper(std::string const& config, std::string const& component,
                    bool allComponents, cmLocalGenerator* lg,
                    cmSbom::Document& doc, cmSbom::Package* package);

  /// @brief Visit an install generator and record artifacts in the SBOM
  bool AddInstallGeneratorArtifacts(cmInstallGenerator* ieGen);

  // Specializations for types of install generators:

  void AddInstallTargetGeneratorArtifacts(cmInstallTargetGenerator* itGen);
  void AddInstallFilesGeneratorArtifacts(cmInstallFilesGenerator* ifGen);
  void AddInstallDirectoryGeneratorArtifacts(
    cmInstallDirectoryGenerator* idGen);

private:
  bool InstallsWithLocalComponent(cmInstallGenerator* installGen);

  std::string const& Config;
  std::string const& Component;
  bool AllComponents;

  cmLocalGenerator* LocalGenerator;
  cmSbom::Document& Document;
  cmSbom::Package* Package;

  std::unordered_map<std::type_index, std::function<void(cmInstallGenerator*)>>
    InstallSbomGeneratorMap;
};

InstallSbomHelper::InstallSbomHelper(std::string const& config,
                                     std::string const& component,
                                     bool allComponents, cmLocalGenerator* lg,
                                     cmSbom::Document& doc,
                                     cmSbom::Package* package)
  : Config(config)
  , Component(component)
  , AllComponents(allComponents)
  , LocalGenerator(lg)
  , Document(doc)
  , Package(package)
{
  this->InstallSbomGeneratorMap[typeid(cmInstallFilesGenerator)] =
    [this](cmInstallGenerator* installGen) {
      auto* ifGen = dynamic_cast<cmInstallFilesGenerator*>(installGen);
      if (!ifGen) {
        return;
      }
      this->AddInstallFilesGeneratorArtifacts(ifGen);
    };
}

bool InstallSbomHelper::InstallsWithLocalComponent(
  cmInstallGenerator* installGen)
{
  auto componentsEq = [](std::string const& lhs, std::string const& rhs) {
    auto isUnspecifiedComponent = [](std::string const& str) {
      return str == "Unspecified" || str.empty();
    };
    return lhs == rhs ||
      (isUnspecifiedComponent(lhs) && isUnspecifiedComponent(rhs));
  };

  if (this->AllComponents) {
    return !installGen->GetExcludeFromAll();
  }
  return componentsEq(installGen->GetComponent(), this->Component);
}

bool InstallSbomHelper::AddInstallGeneratorArtifacts(
  cmInstallGenerator* installGen)
{
  // Skip generators excluded from the selected component without error
  if (!this->InstallsWithLocalComponent(installGen)) {
    return true;
  }

  auto it = this->InstallSbomGeneratorMap.find(typeid(*installGen));
  if (it != this->InstallSbomGeneratorMap.end()) {
    it->second(installGen);
    return true;
  }
  // Unrecognized generator type
  return false;
}

void InstallSbomHelper::AddInstallFilesGeneratorArtifacts(
  cmInstallFilesGenerator* ifGen)
{
  std::vector<std::string> files = ifGen->GetFiles(this->Config);
  if (files.empty()) {
    return;
  }
  std::string destination = ifGen->GetDestination(this->Config);
  std::string const& rename = ifGen->GetRename(this->Config);
  for (auto const& file : files) {
    std::string fileName;
    if (!rename.empty()) {
      fileName = rename;
    } else {
      fileName = cmSystemTools::GetFilenameName(file);
    }
    std::string installedFilePath = cmStrCat(destination, "/", fileName);

    auto* fileElement =
      this->Document.AddRootElement(cm::make_unique<cmSbom::File>(
        installedFilePath, cmSbom::File::FileKind::File));
    this->Document.AddElement(cm::make_unique<cmSbom::Relationship>(
      cmSbom::RelationshipType::Contains, this->Package, fileElement));
  }
}

}

cmInstallSbomDocumentGenerator::cmInstallSbomDocumentGenerator(
  std::string output_format, std::string destination,
  std::vector<std::string> const& configurations, std::string component,
  MessageLevel message, bool exclude_from_all, bool all_components,
  cmListFileBacktrace backtrace)
  : cmInstallGenerator(destination, configurations, std::move(component),
                       message, exclude_from_all, all_components, backtrace)
  , cmSbomDocumentGenerator(std::move(output_format))
{
  this->ActionsPerConfig = true;
  this->Config = configurations.empty() ? "" : configurations[0];
}

bool cmInstallSbomDocumentGenerator::Compute(cmLocalGenerator* lg)
{
  this->LocalGenerator = lg;
  return cmInstallGenerator::Compute(lg);
}

void cmInstallSbomDocumentGenerator::GenerateScriptForConfig(
  std::ostream& os, std::string const& config, Indent indent)
{
  // Figure out the destination path
  std::string destDir;
  if (this->Destination.empty()) {
    destDir = "${CMAKE_INSTALL_PREFIX}";
  } else {
    destDir = ConvertToAbsoluteDestination(this->Destination);
  }
  std::string projectName =
    this->LocalGenerator->GetStateSnapshot().GetProjectName();
  std::string destPrefix = cmStrCat(destDir, "/", projectName, "Sbom");
  std::string destSuffix;
  if (config.empty()) {
    destSuffix += "-${CMAKE_SBOM_INSTALL_CONFIG_NAME}";
  } else {
    destSuffix += "-" + config;
  }
  destSuffix += ".spdx.json";

  auto getDest = [&](std::string const& component, bool allComponents) {
    if (allComponents) {
      return cmStrCat(destPrefix, destSuffix);
    }
    return cmStrCat(destPrefix, "-", component, destSuffix);
  };

  std::vector<std::unique_ptr<cmInstallGenerator>> const& installGenerators =
    this->LocalGenerator->GetMakefile()->GetInstallGenerators();

  // Generate one SBOM for the project's full installation if not excluded from
  // all
  if (!this->GetExcludeFromAll()) {
    cmSbom::Document fullInstallSbom =
      this->BuildSbomForInstallConfig(config, true, installGenerators);

    os << indent << "if(NOT CMAKE_INSTALL_COMPONENT)\n";
    this->GenerateSbomInstaller(os, fullInstallSbom, getDest("", true),
                                indent.Next());
    os << indent << "endif()\n";
  }

  // Generate one SBOM for single component installation
  cmSbom::Document componentSbom =
    this->BuildSbomForInstallConfig(config, false, installGenerators);

  os << indent << "if(CMAKE_INSTALL_COMPONENT STREQUAL \""
     << this->GetComponent() << "\")\n";
  this->GenerateSbomInstaller(
    os, componentSbom, getDest(this->GetComponent(), false), indent.Next());
  os << indent << "endif()\n";
}

cmSbom::Document cmInstallSbomDocumentGenerator::BuildSbomForInstallConfig(
  std::string const& config, bool allComponents,
  std::vector<std::unique_ptr<cmInstallGenerator>> const& gens)
{
  cmSbom::Document doc;

  std::string packageName =
    this->LocalGenerator->GetStateSnapshot().GetProjectName();

  // List the root of the package being installed
  auto* package =
    doc.AddRootElement(cm::make_unique<cmSbom::Package>(packageName));

  // Note the "describes" relationship between the package and the document
  doc.AddElement(cm::make_unique<cmSbom::Relationship>(
    cmSbom::RelationshipType::Describes, &doc, package));

  InstallSbomHelper helper(config, this->GetComponent(), allComponents,
                           this->LocalGenerator, doc, package);

  // Add contents of the package
  bool success = true;
  for (auto const& gen : gens) {
    if (!gen->InstallsForConfig(config)) {
      continue;
    }
    success = helper.AddInstallGeneratorArtifacts(gen.get()) && success;
  }
  if (!success) {
    this->ReportIncomplete();
  }
  return doc;
}

bool cmInstallSbomDocumentGenerator::GenerateSbomInstaller(
  std::ostream& os, cmSbom::Document& doc, std::string const& dest,
  Indent indent)
{
  os << indent << "if(NOT CMAKE_INSTALL_CONFIG_NAME)\n";
  os << indent.Next() << "set(CMAKE_SBOM_INSTALL_CONFIG_NAME \"noconfig\")\n";
  os << indent << "else()\n";
  os << indent.Next()
     << "set(CMAKE_SBOM_INSTALL_CONFIG_NAME "
        "\"${CMAKE_INSTALL_CONFIG_NAME}\")\n";
  os << indent << "endif()\n";

  os << indent << "set(CMAKE_SBOM_INSTALL_RESULT_PATH \"" << dest << "\")\n";
  os << indent
     << "cmake_path(ABSOLUTE_PATH \"CMAKE_SBOM_INSTALL_RESULT_PATH\")\n";
  os << indent
     << R"(message(STATUS "Installing: " "${CMAKE_SBOM_INSTALL_RESULT_PATH}"))"
     << '\n';
  os << indent << "file(WRITE \"" << dest << "\" [==[\n";
  bool success = this->WriteSbom(os, doc);
  os << indent << "]==])\n";

  if (!success) {
    this->LocalGenerator->IssueMessage(
      MessageType::INTERNAL_ERROR,
      cmStrCat("Failed to generate SBOM installer for config ", this->Config));
    return false;
  }

  os << indent << "string(TIMESTAMP CMAKE_SBOM_GENERATOR_TIMESTAMP UTC)\n";
  os << indent << "configure_file(\"" << dest << "\" \"" << dest
     << "\" @ONLY)\n";
  return true;
}

void cmInstallSbomDocumentGenerator::ReportIncomplete()
{
  if (!this->SbomBuildIncomplete) {
    this->LocalGenerator->GetCMakeInstance()->GetMessenger()->IssueMessage(
      MessageType::WARNING,
      cmStrCat("Experimental SBOM information is incomplete in project ",
               this->LocalGenerator->GetStateSnapshot().GetProjectName(), "."),
      this->GetBacktrace());
  }
  this->SbomBuildIncomplete = true;
}
