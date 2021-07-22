/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#include <algorithm>
#include <iterator>
#include <map>
#include <sstream>
#include <vector>

#include "cmExportInstallCMakeConfigFileGenerator.h"
#include "cmExportUtility.h"
#include "cmGeneratedFileStream.h"
#include "cmGeneratorExpression.h"
#include "cmGeneratorTarget.h"
#include "cmInstallExportGenerator.h"
#include "cmInstallTargetGenerator.h"
#include "cmLocalGenerator.h"
#include "cmMakefile.h"
#include "cmPolicies.h"
#include "cmStateTypes.h"
#include "cmStringAlgorithms.h"
#include "cmSystemTools.h"
#include "cmTargetExport.h"

cmExportInstallCMakeConfigFileGenerator::
  cmExportInstallCMakeConfigFileGenerator(cmInstallExportGenerator* iegen)
  : cmExportInstallFileGenerator{ iegen }
{
}

bool cmExportInstallCMakeConfigFileGenerator::GenerateTargetExportCode(
  std::ostream& os, Context& ctx, const cmTargetExport& target,
  cmStateEnums::TargetType targetType)
{
  cmGeneratorTarget* gt = target.Target;

  requiresConfigFiles =
    requiresConfigFiles || targetType != cmStateEnums::INTERFACE_LIBRARY;

  this->GenerateImportTargetCode(os, gt, targetType);

  const bool newCMP0022Behavior =
    gt->GetPolicyStatusCMP0022() != cmPolicies::WARN &&
    gt->GetPolicyStatusCMP0022() != cmPolicies::OLD;

  ImportPropertyMap properties = this->CommonInterfaceProperties(
    target.Target, cmGeneratorExpression::InstallInterface, ctx,
    newCMP0022Behavior);
  this->PopulateSourcesInterface(gt, cmGeneratorExpression::InstallInterface,
                                 properties, ctx);

  std::string errorMessage;
  if (!this->PopulateExportProperties(gt, properties, errorMessage)) {
    cmSystemTools::Error(errorMessage);
    return false;
  }

  if (newCMP0022Behavior) {
    if (properties.count("INTERFACE_LINK_LIBRARIES") && !this->ExportOld) {
      require2_8_12 = true;
    }
  }
  if (targetType == cmStateEnums::INTERFACE_LIBRARY) {
    require3_0_0 = true;
  }
  if (gt->GetProperty("INTERFACE_SOURCES")) {
    // We can only generate INTERFACE_SOURCES in CMake 3.3, but CMake 3.1
    // can consume them.
    require3_1_0 = true;
  }

  this->PopulateInterfaceProperty("INTERFACE_POSITION_INDEPENDENT_CODE", gt,
                                  properties);

  this->PopulateCompatibleInterfaceProperties(gt, properties);

  this->GenerateInterfaceProperties(gt, os, properties);

  return true;
}

// bool cmExportInstallCMakeConfigFileGenerator::GenerateMainFile(
//   std::ostream& os)
// // {

//   std::vector<std::string> missingTargets;

// Create all the imported targets.
/*  for (cmTargetExport* te : allTargets) {
    cmGeneratorTarget* gt = te->Target;
    cmStateEnums::TargetType targetType = this->GetExportTargetType(te);

    requiresConfigFiles =
      requiresConfigFiles || targetType != cmStateEnums::INTERFACE_LIBRARY;

    this->GenerateImportTargetCode(os, gt, targetType);

    ImportPropertyMap properties = CommonInterfaceProperties(
      te->Target, cmGeneratorExpression::InstallInterface, missingTargets);
    this->PopulateSourcesInterface(te->Target,
                                   cmGeneratorExpression::InstallInterface,
                                   properties, missingTargets);

    std::string errorMessage;
    if (!this->PopulateExportProperties(gt, properties, errorMessage)) {
      cmSystemTools::Error(errorMessage);
      return false;
    }

    const bool newCMP0022Behavior =
      gt->GetPolicyStatusCMP0022() != cmPolicies::WARN &&
      gt->GetPolicyStatusCMP0022() != cmPolicies::OLD;
    if (newCMP0022Behavior) {
      if (this->PopulateInterfaceLinkLibrariesProperty(
            gt, cmGeneratorExpression::InstallInterface, properties,
            missingTargets) &&
          !this->ExportOld) {
        require2_8_12 = true;
      }
    }
    if (targetType == cmStateEnums::INTERFACE_LIBRARY) {
      require3_0_0 = true;
    }
    if (gt->GetProperty("INTERFACE_SOURCES")) {
      // We can only generate INTERFACE_SOURCES in CMake 3.3, but CMake 3.1
      // can consume them.
      require3_1_0 = true;
    }

    this->PopulateInterfaceProperty("INTERFACE_POSITION_INDEPENDENT_CODE", gt,
                                    properties);

    this->PopulateCompatibleInterfaceProperties(gt, properties);

    this->GenerateInterfaceProperties(gt, os, properties);
  }
  */
// }

bool cmExportInstallCMakeConfigFileGenerator::GenerateImportHeaderCode(
  std::ostream& os, Context& ctx)
{
  require2_8_12 = require3_0_0 = require3_1_0 = false;

  this->GenerateCommonFileHeader(os, ctx.Config);

  if (!ctx.ExportedTargets.empty()) {
    std::vector<std::string> expectedTargets;
    expectedTargets.reserve(ctx.ExportedTargets.size());
    std::transform(ctx.ExportedTargets.begin(), ctx.ExportedTargets.end(),
                   std::back_inserter(expectedTargets),
                   [this](cmGeneratorTarget const* gt) {
                     return this->Namespace + gt->GetExportName();
                   });
    Exporting::CMakeConfig::GenerateExpectedTargetsCode(
      os, cmJoin(expectedTargets, " "));
  }

  // Compute the relative import prefix for the file
  this->GenerateImportPrefix(os);

  return true;
}

bool cmExportInstallCMakeConfigFileGenerator::GenerateImportFooterCode(
  std::ostream& os, Context& ctx)
{
  namespace ExpCMCfg = Exporting::CMakeConfig;

  if (require3_1_0) {
    ExpCMCfg::GenerateRequiredCMakeVersion(os, "3.1.0");
  } else if (require3_0_0) {
    ExpCMCfg::GenerateRequiredCMakeVersion(os, "3.0.0");
  } else if (require2_8_12) {
    ExpCMCfg::GenerateRequiredCMakeVersion(os, "2.8.12");
  }

  this->LoadConfigFiles(os);

  this->CleanupTemporaryVariables(os);
  ExpCMCfg::GenerateImportedFileCheckLoop(os);

  bool result = true;
  // Generate an import file for each configuration.
  // Don't do this if we only export INTERFACE_LIBRARY targets.
  if (requiresConfigFiles) {
    for (std::string const& c : this->Configurations) {
      if (!this->GenerateImportFileConfig(c, ctx)) {
        result = false;
      }
    }
  }

  if (!result) {
    return false;
  }

  ExpCMCfg::GenerateMissingTargetsCheckCode(os, ctx.MissingTargets);

  // End with the import file footer.
  Exporting::CMakeConfig::GenerateImportFooterCode(os);
  Exporting::CMakeConfig::GeneratePolicyFooterCode(os);

  return true;
}

void cmExportInstallCMakeConfigFileGenerator::GenerateImportPrefix(
  std::ostream& os)
{
  // Set an _IMPORT_PREFIX variable for import location properties
  // to reference if they are relative to the install prefix.
  std::string installPrefix =
    this->IEGen->GetLocalGenerator()->GetMakefile()->GetSafeDefinition(
      "CMAKE_INSTALL_PREFIX");
  std::string const& expDest = this->IEGen->GetDestination();
  if (cmSystemTools::FileIsFullPath(expDest)) {
    // The export file is being installed to an absolute path so the
    // package is not relocatable.  Use the configured install prefix.
    /* clang-format off */
    os <<
      "# The installation prefix configured by this project.\n"
      "set(_IMPORT_PREFIX \"" << installPrefix << "\")\n"
      "\n";
    /* clang-format on */
  } else {
    // Add code to compute the installation prefix relative to the
    // import file location.
    std::string absDest = installPrefix + "/" + expDest;
    std::string absDestS = absDest + "/";
    os << "# Compute the installation prefix relative to this file.\n"
       << "get_filename_component(_IMPORT_PREFIX"
       << " \"${CMAKE_CURRENT_LIST_FILE}\" PATH)\n";
    if (cmHasLiteralPrefix(absDestS, "/lib/") ||
        cmHasLiteralPrefix(absDestS, "/lib64/") ||
        cmHasLiteralPrefix(absDestS, "/libx32/") ||
        cmHasLiteralPrefix(absDestS, "/usr/lib/") ||
        cmHasLiteralPrefix(absDestS, "/usr/lib64/") ||
        cmHasLiteralPrefix(absDestS, "/usr/libx32/")) {
      // Handle "/usr move" symlinks created by some Linux distros.
      /* clang-format off */
      os <<
        "# Use original install prefix when loaded through a\n"
        "# cross-prefix symbolic link such as /lib -> /usr/lib.\n"
        "get_filename_component(_realCurr \"${_IMPORT_PREFIX}\" REALPATH)\n"
        "get_filename_component(_realOrig \"" << absDest << "\" REALPATH)\n"
        "if(_realCurr STREQUAL _realOrig)\n"
        "  set(_IMPORT_PREFIX \"" << absDest << "\")\n"
        "endif()\n"
        "unset(_realOrig)\n"
        "unset(_realCurr)\n";
      /* clang-format on */
    }
    std::string dest = expDest;
    while (!dest.empty()) {
      os << "get_filename_component(_IMPORT_PREFIX \"${_IMPORT_PREFIX}\" "
            "PATH)\n";
      dest = cmSystemTools::GetFilenamePath(dest);
    }
    os << "if(_IMPORT_PREFIX STREQUAL \"/\")\n"
       << "  set(_IMPORT_PREFIX \"\")\n"
       << "endif()\n"
       << "\n";
  }
}

void cmExportInstallCMakeConfigFileGenerator::CleanupTemporaryVariables(
  std::ostream& os)
{
  /* clang-format off */
  os << "# Cleanup temporary variables.\n"
     << "set(_IMPORT_PREFIX)\n"
     << "\n";
  /* clang-format on */
}

void cmExportInstallCMakeConfigFileGenerator::LoadConfigFiles(std::ostream& os)
{
  // Now load per-configuration properties for them.
  /* clang-format off */
  os << "# Load information for each installed configuration.\n"
     << "get_filename_component(_DIR \"${CMAKE_CURRENT_LIST_FILE}\" PATH)\n"
     << "file(GLOB CONFIG_FILES \"${_DIR}/"
     << this->GetConfigImportFileGlob() << "\")\n"
     << "foreach(f ${CONFIG_FILES})\n"
     << "  include(${f})\n"
     << "endforeach()\n"
     << "\n";
  /* clang-format on */
}

void cmExportInstallCMakeConfigFileGenerator::ReplaceInstallPrefix(
  std::string& input) const
{
  cmGeneratorExpression::ReplaceInstallPrefix(input, "${_IMPORT_PREFIX}");
}

bool cmExportInstallCMakeConfigFileGenerator::GenerateImportFileConfig(
  const std::string& config, Context& ctx)
{
  // Skip configurations not enabled for this export.
  if (!this->IEGen->InstallsForConfig(config)) {
    return true;
  }

  // Construct the name of the file to generate.
  std::string fileName = cmStrCat(this->FileDir, '/', this->FileBase, '-');
  if (!config.empty()) {
    fileName += cmSystemTools::LowerCase(config);
  } else {
    fileName += "noconfig";
  }
  fileName += this->FileExt;

  // Open the output file to generate it.
  cmGeneratedFileStream exportFileStream(fileName, true);
  if (!exportFileStream) {
    std::string se = cmSystemTools::GetLastSystemError();
    std::ostringstream e;
    e << "cannot write to file \"" << fileName << "\": " << se;
    cmSystemTools::Error(e.str());
    return false;
  }
  exportFileStream.SetCopyIfDifferent(true);
  std::ostream& os = exportFileStream;

  // Start with the import file header.
  Exporting::CMakeConfig::GenerateImportHeaderCode(os, config);

  // Generate the per-config target information.
  this->GenerateImportConfig(os, config, ctx);

  // End with the import file footer.
  Exporting::CMakeConfig::GenerateImportFooterCode(os);

  // Record this per-config import file.
  this->SetImportFileForConfig(config, fileName);

  return true;
}

void cmExportInstallCMakeConfigFileGenerator::GenerateImportTargetsConfig(
  std::ostream& os, std::string const& config, base::Context& ctx,
  std::string const& suffix)
{
  // FIXME installCtx contains config too. Is it the same as the config arg?
  Context& installCtx = static_cast<Context&>(ctx);
  // Add each target in the set to the export.
  for (cmTargetExport const* te : installCtx.TargetExports) {
    // Collect import properties for this target.
    cmStateEnums::TargetType const targetType =
      Exporting::GetExportTargetType(*te);
    if (targetType == cmStateEnums::INTERFACE_LIBRARY) {
      continue;
    }

    ImportPropertyMap properties;
    std::set<std::string> importedLocations;

    this->SetImportLocationProperty(config, suffix, te->ArchiveGenerator,
                                    properties, importedLocations);
    this->SetImportLocationProperty(config, suffix, te->LibraryGenerator,
                                    properties, importedLocations);
    this->SetImportLocationProperty(config, suffix, te->RuntimeGenerator,
                                    properties, importedLocations);
    this->SetImportLocationProperty(config, suffix, te->ObjectsGenerator,
                                    properties, importedLocations);
    this->SetImportLocationProperty(config, suffix, te->FrameworkGenerator,
                                    properties, importedLocations);
    this->SetImportLocationProperty(config, suffix, te->BundleGenerator,
                                    properties, importedLocations);

    // If any file location was set for the target add it to the
    // import file.
    if (!properties.empty()) {
      // Get the rest of the target details.
      cmGeneratorTarget* gtgt = te->Target;
      this->SetImportDetailProperties(config, suffix, gtgt, properties, ctx);

      this->SetImportLinkInterface(config, suffix,
                                   cmGeneratorExpression::InstallInterface,
                                   gtgt, properties, ctx);

      // TODO: PUBLIC_HEADER_LOCATION
      // This should wait until the build feature propagation stuff
      // is done.  Then this can be a propagated include directory.
      // this->GenerateImportProperty(config, te->HeaderGenerator,
      //                              properties);

      // Generate code in the export file.
      Exporting::NamespacedTarget const nstg{ this->Namespace, *gtgt };
      Exporting::CMakeConfig::GenerateImportPropertyCode(os, config, nstg,
                                                         properties);
      Exporting::CMakeConfig::GenerateImportedFileChecksCode(
        os, nstg, properties, importedLocations);
    }
  }
}

void cmExportInstallCMakeConfigFileGenerator::SetImportLocationProperty(
  const std::string& config, std::string const& suffix,
  cmInstallTargetGenerator* itgen, ImportPropertyMap& properties,
  std::set<std::string>& importedLocations)
{
  // Skip rules that do not match this configuration.
  if (!(itgen && itgen->InstallsForConfig(config))) {
    return;
  }

  // Get the target to be installed.
  cmGeneratorTarget* target = itgen->GetTarget();

  // Construct the installed location of the target.
  std::string dest = itgen->GetDestination(config);
  std::string value;
  if (!cmSystemTools::FileIsFullPath(dest)) {
    // The target is installed relative to the installation prefix.
    value = "${_IMPORT_PREFIX}/";
  }
  value += dest;
  value += "/";

  if (itgen->IsImportLibrary()) {
    // Construct the property name.
    std::string prop = cmStrCat("IMPORTED_IMPLIB", suffix);

    // Append the installed file name.
    value += cmInstallTargetGenerator::GetInstallFilename(
      target, config, cmInstallTargetGenerator::NameImplib);

    // Store the property.
    properties[prop] = value;
    importedLocations.insert(prop);
  } else if (itgen->GetTarget()->GetType() == cmStateEnums::OBJECT_LIBRARY) {
    // Construct the property name.
    std::string prop = cmStrCat("IMPORTED_OBJECTS", suffix);

    // Compute all the object files inside this target and setup
    // IMPORTED_OBJECTS as a list of object files
    std::vector<std::string> objects;
    itgen->GetInstallObjectNames(config, objects);
    for (std::string& obj : objects) {
      obj = cmStrCat(value, obj);
    }

    // Store the property.
    properties[prop] = cmJoin(objects, ";");
    importedLocations.insert(prop);
  } else {
    // Construct the property name.
    std::string prop = cmStrCat("IMPORTED_LOCATION", suffix);

    // Append the installed file name.
    if (target->IsAppBundleOnApple()) {
      value += cmInstallTargetGenerator::GetInstallFilename(target, config);
      value += ".app/Contents/MacOS/";
      value += cmInstallTargetGenerator::GetInstallFilename(target, config);
    } else {
      value += cmInstallTargetGenerator::GetInstallFilename(
        target, config, cmInstallTargetGenerator::NameReal);
    }

    // Store the property.
    properties[prop] = value;
    importedLocations.insert(prop);
  }
}

bool cmExportInstallCMakeConfigFileGenerator::
  GeneratedFilesRequireVariableExpansion() const
{
  return false;
}

void cmExportInstallCMakeConfigFileGenerator::GenerateCommonFileHeader(
  std::ostream& os, const std::string& config) const
{
  Exporting::CMakeConfig::GeneratePolicyHeaderCode(os);
  Exporting::CMakeConfig::GenerateImportHeaderCode(os, config);
}

void cmExportInstallCMakeConfigFileGenerator::GenerateImportTargetCode(
  std::ostream& os, const cmGeneratorTarget* target,
  cmStateEnums::TargetType targetType)
{
  Exporting::CMakeConfig::GenerateImportTargetCode(
    os, { this->Namespace, *target }, targetType);
}
