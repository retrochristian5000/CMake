/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#include "cmExportFileGenerator.h"

#include <algorithm>
#include <cassert>
#include <cstring>
#include <sstream>
#include <utility>

#include <cm/memory>

#include "cmsys/FStream.hxx"

#include "cmComputeLinkInformation.h"
#include "cmGeneratedFileStream.h"
#include "cmGeneratorTarget.h"
#include "cmGlobalGenerator.h"
#include "cmLinkItem.h"
#include "cmLocalGenerator.h"
#include "cmMakefile.h"
#include "cmMessageType.h"
#include "cmPolicies.h"
#include "cmProperty.h"
#include "cmPropertyMap.h"
#include "cmStateTypes.h"
#include "cmStringAlgorithms.h"
#include "cmSystemTools.h"
#include "cmTarget.h"
#include "cmake.h"

cmExportFileGenerator::cmExportFileGenerator()
{
  this->AppendMode = false;
  this->ExportOld = false;
}

void cmExportFileGenerator::AddConfiguration(const std::string& config)
{
  this->Configurations.push_back(config);
}

void cmExportFileGenerator::SetExportFile(const char* mainFile)
{
  this->MainImportFile = mainFile;
  this->FileDir = cmSystemTools::GetFilenamePath(this->MainImportFile);
  this->FileBase =
    cmSystemTools::GetFilenameWithoutLastExtension(this->MainImportFile);
  this->FileExt =
    cmSystemTools::GetFilenameLastExtension(this->MainImportFile);
}

const std::string& cmExportFileGenerator::GetMainExportFileName() const
{
  return this->MainImportFile;
}

bool cmExportFileGenerator::GenerateImportFile()
{
  // Open the output file to generate it.
  std::unique_ptr<cmsys::ofstream> foutPtr;
  if (this->AppendMode) {
    // Open for append.
    auto openmodeApp = std::ios::app;
    foutPtr = cm::make_unique<cmsys::ofstream>(this->MainImportFile.c_str(),
                                               openmodeApp);
  } else {
    // Generate atomically and with copy-if-different.
    std::unique_ptr<cmGeneratedFileStream> ap(
      new cmGeneratedFileStream(this->MainImportFile, true));
    ap->SetCopyIfDifferent(true);
    foutPtr = std::move(ap);
  }
  if (!foutPtr || !*foutPtr) {
    std::string se = cmSystemTools::GetLastSystemError();
    std::ostringstream e;
    e << "cannot write to file \"" << this->MainImportFile << "\": " << se;
    cmSystemTools::Error(e.str());
    return false;
  }
  std::ostream& os = *foutPtr;

  // Create all the imported targets.
  bool result = this->GenerateMainFile(os);

  return result;
}

void cmExportFileGenerator::GenerateImportConfig(std::ostream& os,
                                                 std::string const& config,
                                                 Context& ctx)
{
  // Construct the property configuration suffix.
  std::string suffix = "_";
  if (!config.empty()) {
    suffix += cmSystemTools::UpperCase(config);
  } else {
    suffix += "NOCONFIG";
  }

  // Generate the per-config target information.
  this->GenerateImportTargetsConfig(os, config, ctx, suffix);
}

void cmExportFileGenerator::PopulateInterfaceProperty(
  const std::string& propName, cmGeneratorTarget const* target,
  ImportPropertyMap& properties) const
{
  cmProp input = target->GetProperty(propName);
  if (input) {
    properties[propName] = *input;
  }
}

void cmExportFileGenerator::PopulateInterfaceProperty(
  const std::string& propName, const std::string& outputName,
  cmGeneratorTarget const* target,
  cmGeneratorExpression::PreprocessContext preprocessRule,
  ImportPropertyMap& properties, Context& ctx) const
{
  cmProp input = target->GetProperty(propName);
  if (input) {
    if (input->empty()) {
      // Set to empty
      properties[outputName].clear();
      return;
    }

    std::string prepro =
      cmGeneratorExpression::Preprocess(*input, preprocessRule);
    if (!prepro.empty()) {
      this->ResolveTargetsInGeneratorExpressions(prepro, target, ctx);
      properties[outputName] = prepro;
    }
  }
}

bool cmExportFileGenerator::PopulateInterfaceLinkLibrariesProperty(
  cmGeneratorTarget const* target,
  cmGeneratorExpression::PreprocessContext preprocessRule,
  ImportPropertyMap& properties, Context& ctx) const
{
  if (!target->IsLinkable()) {
    return false;
  }
  cmProp input = target->GetProperty("INTERFACE_LINK_LIBRARIES");
  if (input) {
    std::string prepro =
      cmGeneratorExpression::Preprocess(*input, preprocessRule);
    if (!prepro.empty()) {
      this->ResolveTargetsInGeneratorExpressions(prepro, target, ctx,
                                                 ReplaceFreeTargets);
      properties["INTERFACE_LINK_LIBRARIES"] = prepro;
      return true;
    }
  }
  return false;
}

static bool isSubDirectory(std::string const& a, std::string const& b)
{
  return (cmSystemTools::ComparePath(a, b) ||
          cmSystemTools::IsSubDirectory(a, b));
}

static bool checkInterfaceDirs(const std::string& prepro,
                               cmGeneratorTarget const* target,
                               const std::string& prop)
{
  std::string const& installDir =
    target->Makefile->GetSafeDefinition("CMAKE_INSTALL_PREFIX");
  std::string const& topSourceDir =
    target->GetLocalGenerator()->GetSourceDirectory();
  std::string const& topBinaryDir =
    target->GetLocalGenerator()->GetBinaryDirectory();

  std::vector<std::string> parts;
  cmGeneratorExpression::Split(prepro, parts);

  const bool inSourceBuild = topSourceDir == topBinaryDir;

  bool hadFatalError = false;

  for (std::string const& li : parts) {
    size_t genexPos = cmGeneratorExpression::Find(li);
    if (genexPos == 0) {
      continue;
    }
    if (cmHasLiteralPrefix(li, "${_IMPORT_PREFIX}")) {
      continue;
    }
    MessageType messageType = MessageType::FATAL_ERROR;
    std::ostringstream e;
    if (genexPos != std::string::npos) {
      if (prop == "INTERFACE_INCLUDE_DIRECTORIES") {
        switch (target->GetPolicyStatusCMP0041()) {
          case cmPolicies::WARN:
            messageType = MessageType::WARNING;
            e << cmPolicies::GetPolicyWarning(cmPolicies::CMP0041) << "\n";
            break;
          case cmPolicies::OLD:
            continue;
          case cmPolicies::REQUIRED_IF_USED:
          case cmPolicies::REQUIRED_ALWAYS:
          case cmPolicies::NEW:
            hadFatalError = true;
            break; // Issue fatal message.
        }
      } else {
        hadFatalError = true;
      }
    }
    if (!cmSystemTools::FileIsFullPath(li)) {
      /* clang-format off */
      e << "Target \"" << target->GetName() << "\" " << prop <<
           " property contains relative path:\n"
           "  \"" << li << "\"";
      /* clang-format on */
      target->GetLocalGenerator()->IssueMessage(messageType, e.str());
    }
    bool inBinary = isSubDirectory(li, topBinaryDir);
    bool inSource = isSubDirectory(li, topSourceDir);
    if (isSubDirectory(li, installDir)) {
      // The include directory is inside the install tree.  If the
      // install tree is not inside the source tree or build tree then
      // fall through to the checks below that the include directory is not
      // also inside the source tree or build tree.
      bool shouldContinue =
        (!inBinary || isSubDirectory(installDir, topBinaryDir)) &&
        (!inSource || isSubDirectory(installDir, topSourceDir));

      if (prop == "INTERFACE_INCLUDE_DIRECTORIES") {
        if (!shouldContinue) {
          switch (target->GetPolicyStatusCMP0052()) {
            case cmPolicies::WARN: {
              std::ostringstream s;
              s << cmPolicies::GetPolicyWarning(cmPolicies::CMP0052) << "\n";
              s << "Directory:\n    \"" << li
                << "\"\nin "
                   "INTERFACE_INCLUDE_DIRECTORIES of target \""
                << target->GetName()
                << "\" is a subdirectory of the install "
                   "directory:\n    \""
                << installDir
                << "\"\nhowever it is also "
                   "a subdirectory of the "
                << (inBinary ? "build" : "source") << " tree:\n    \""
                << (inBinary ? topBinaryDir : topSourceDir) << "\"\n";
              target->GetLocalGenerator()->IssueMessage(
                MessageType::AUTHOR_WARNING, s.str());
              CM_FALLTHROUGH;
            }
            case cmPolicies::OLD:
              shouldContinue = true;
              break;
            case cmPolicies::REQUIRED_ALWAYS:
            case cmPolicies::REQUIRED_IF_USED:
            case cmPolicies::NEW:
              break;
          }
        }
      }
      if (shouldContinue) {
        continue;
      }
    }
    if (inBinary) {
      /* clang-format off */
      e << "Target \"" << target->GetName() << "\" " << prop <<
           " property contains path:\n"
           "  \"" << li << "\"\nwhich is prefixed in the build directory.";
      /* clang-format on */
      target->GetLocalGenerator()->IssueMessage(messageType, e.str());
    }
    if (!inSourceBuild) {
      if (inSource) {
        e << "Target \"" << target->GetName() << "\" " << prop
          << " property contains path:\n"
             "  \""
          << li << "\"\nwhich is prefixed in the source directory.";
        target->GetLocalGenerator()->IssueMessage(messageType, e.str());
      }
    }
  }
  return !hadFatalError;
}

static void prefixItems(std::string& exportDirs)
{
  std::vector<std::string> entries;
  cmGeneratorExpression::Split(exportDirs, entries);
  exportDirs.clear();
  const char* sep = "";
  for (std::string const& e : entries) {
    exportDirs += sep;
    sep = ";";
    if (!cmSystemTools::FileIsFullPath(e) &&
        e.find("${_IMPORT_PREFIX}") == std::string::npos) {
      exportDirs += "${_IMPORT_PREFIX}/";
    }
    exportDirs += e;
  }
}

void cmExportFileGenerator::PopulateSourcesInterface(
  cmGeneratorTarget const* gt,
  cmGeneratorExpression::PreprocessContext preprocessRule,
  ImportPropertyMap& properties, Context& ctx) const
{
  assert(preprocessRule == cmGeneratorExpression::InstallInterface);

  const char* propName = "INTERFACE_SOURCES";
  cmProp input = gt->GetProperty(propName);

  if (!input) {
    return;
  }

  if (input->empty()) {
    properties[propName].clear();
    return;
  }

  std::string prepro =
    cmGeneratorExpression::Preprocess(*input, preprocessRule, true);
  if (!prepro.empty()) {
    this->ResolveTargetsInGeneratorExpressions(prepro, gt, ctx);

    if (!checkInterfaceDirs(prepro, gt, propName)) {
      return;
    }
    properties[propName] = prepro;
  }
}

void cmExportFileGenerator::PopulateIncludeDirectoriesInterface(
  cmGeneratorTarget const* target,
  cmGeneratorExpression::PreprocessContext preprocessRule,
  ImportPropertyMap& properties, Context& ctx) const
{
  assert(preprocessRule == cmGeneratorExpression::InstallInterface);

  cmGeneratorExpression ge;

  std::string dirs = cmGeneratorExpression::Preprocess(
    cmJoin(target->Target->GetInstallIncludeDirectoriesEntries(), ";"),
    preprocessRule, true);
  this->ReplaceInstallPrefix(dirs);
  std::unique_ptr<cmCompiledGeneratorExpression> cge = ge.Parse(dirs);
  std::string installIncludeDirs =
    cge->Evaluate(target->GetLocalGenerator(), "", target);

  if (cge->GetHadContextSensitiveCondition()) {
    cmLocalGenerator* lg = target->GetLocalGenerator();
    std::ostringstream e;
    e << "Target \"" << target->GetName()
      << "\" is installed with "
         "INCLUDES DESTINATION set to a context sensitive path.  Paths which "
         "depend on the configuration, policy values or the link interface "
         "are "
         "not supported.  Consider using target_include_directories instead.";
    lg->IssueMessage(MessageType::FATAL_ERROR, e.str());
    return;
  }

  const char* propName = "INTERFACE_INCLUDE_DIRECTORIES";
  cmProp input = target->GetProperty(propName);
  if (!input && installIncludeDirs.empty()) {
    return;
  }
  if ((input && input->empty() && installIncludeDirs.empty())) {
    // Set to empty
    properties[propName].clear();
    return;
  }

  prefixItems(installIncludeDirs);

  std::string includes = input ? cmStrCat(*input, ';', installIncludeDirs)
                               : std::move(installIncludeDirs);
  std::string prepro =
    cmGeneratorExpression::Preprocess(includes, preprocessRule, true);
  if (!prepro.empty()) {
    this->ResolveTargetsInGeneratorExpressions(prepro, target, ctx);

    if (!checkInterfaceDirs(prepro, target, propName)) {
      return;
    }
    properties[propName] = prepro;
  }
}

void cmExportFileGenerator::PopulateLinkDependsInterface(
  cmGeneratorTarget const* gt,
  cmGeneratorExpression::PreprocessContext preprocessRule,
  ImportPropertyMap& properties, Context& ctx) const
{
  assert(preprocessRule == cmGeneratorExpression::InstallInterface);

  const char* propName = "INTERFACE_LINK_DEPENDS";
  cmProp input = gt->GetProperty(propName);

  if (!input) {
    return;
  }

  if (input->empty()) {
    properties[propName].clear();
    return;
  }

  std::string prepro =
    cmGeneratorExpression::Preprocess(*input, preprocessRule, true);
  if (!prepro.empty()) {
    this->ResolveTargetsInGeneratorExpressions(prepro, gt, ctx);

    if (!checkInterfaceDirs(prepro, gt, propName)) {
      return;
    }
    properties[propName] = prepro;
  }
}

void cmExportFileGenerator::PopulateLinkDirectoriesInterface(
  cmGeneratorTarget const* gt,
  cmGeneratorExpression::PreprocessContext preprocessRule,
  ImportPropertyMap& properties, Context& ctx) const
{
  assert(preprocessRule == cmGeneratorExpression::InstallInterface);

  const char* propName = "INTERFACE_LINK_DIRECTORIES";
  cmProp input = gt->GetProperty(propName);

  if (!input) {
    return;
  }

  if (input->empty()) {
    properties[propName].clear();
    return;
  }

  std::string prepro =
    cmGeneratorExpression::Preprocess(*input, preprocessRule, true);
  if (!prepro.empty()) {
    this->ResolveTargetsInGeneratorExpressions(prepro, gt, ctx);

    if (!checkInterfaceDirs(prepro, gt, propName)) {
      return;
    }
    properties[propName] = prepro;
  }
}

void cmExportFileGenerator::PopulateInterfaceProperty(
  const std::string& propName, cmGeneratorTarget const* target,
  cmGeneratorExpression::PreprocessContext preprocessRule,
  ImportPropertyMap& properties, Context& ctx) const
{
  this->PopulateInterfaceProperty(propName, propName, target, preprocessRule,
                                  properties, ctx);
}

void getPropertyContents(cmGeneratorTarget const* tgt, const std::string& prop,
                         std::set<std::string>& ifaceProperties)
{
  cmProp p = tgt->GetProperty(prop);
  if (!p) {
    return;
  }
  std::vector<std::string> content = cmExpandedList(*p);
  ifaceProperties.insert(content.begin(), content.end());
}

void getCompatibleInterfaceProperties(cmGeneratorTarget const* target,
                                      std::set<std::string>& ifaceProperties,
                                      const std::string& config)
{
  if (target->GetType() == cmStateEnums::OBJECT_LIBRARY) {
    // object libraries have no link information, so nothing to compute
    return;
  }

  cmComputeLinkInformation* info = target->GetLinkInformation(config);

  if (!info) {
    cmLocalGenerator* lg = target->GetLocalGenerator();
    std::ostringstream e;
    e << "Exporting the target \"" << target->GetName()
      << "\" is not "
         "allowed since its linker language cannot be determined";
    lg->IssueMessage(MessageType::FATAL_ERROR, e.str());
    return;
  }

  const cmComputeLinkInformation::ItemVector& deps = info->GetItems();

  for (auto const& dep : deps) {
    if (!dep.Target) {
      continue;
    }
    getPropertyContents(dep.Target, "COMPATIBLE_INTERFACE_BOOL",
                        ifaceProperties);
    getPropertyContents(dep.Target, "COMPATIBLE_INTERFACE_STRING",
                        ifaceProperties);
    getPropertyContents(dep.Target, "COMPATIBLE_INTERFACE_NUMBER_MIN",
                        ifaceProperties);
    getPropertyContents(dep.Target, "COMPATIBLE_INTERFACE_NUMBER_MAX",
                        ifaceProperties);
  }
}

void cmExportFileGenerator::PopulateCompatibleInterfaceProperties(
  cmGeneratorTarget const* gtarget, ImportPropertyMap& properties) const
{
  this->PopulateInterfaceProperty("COMPATIBLE_INTERFACE_BOOL", gtarget,
                                  properties);
  this->PopulateInterfaceProperty("COMPATIBLE_INTERFACE_STRING", gtarget,
                                  properties);
  this->PopulateInterfaceProperty("COMPATIBLE_INTERFACE_NUMBER_MIN", gtarget,
                                  properties);
  this->PopulateInterfaceProperty("COMPATIBLE_INTERFACE_NUMBER_MAX", gtarget,
                                  properties);

  std::set<std::string> ifaceProperties;

  getPropertyContents(gtarget, "COMPATIBLE_INTERFACE_BOOL", ifaceProperties);
  getPropertyContents(gtarget, "COMPATIBLE_INTERFACE_STRING", ifaceProperties);
  getPropertyContents(gtarget, "COMPATIBLE_INTERFACE_NUMBER_MIN",
                      ifaceProperties);
  getPropertyContents(gtarget, "COMPATIBLE_INTERFACE_NUMBER_MAX",
                      ifaceProperties);

  if (gtarget->GetType() != cmStateEnums::INTERFACE_LIBRARY) {
    std::vector<std::string> configNames =
      gtarget->Target->GetMakefile()->GetGeneratorConfigs(
        cmMakefile::IncludeEmptyConfig);

    for (std::string const& cn : configNames) {
      getCompatibleInterfaceProperties(gtarget, ifaceProperties, cn);
    }
  }

  for (std::string const& ip : ifaceProperties) {
    this->PopulateInterfaceProperty("INTERFACE_" + ip, gtarget, properties);
  }
}

void cmExportFileGenerator::GenerateInterfaceProperties(
  const cmGeneratorTarget* target, std::ostream& os,
  const ImportPropertyMap& properties) const
{
  if (!properties.empty()) {
    std::string targetName =
      cmStrCat(this->Namespace, target->GetExportName());
    os << "set_target_properties(" << targetName << " PROPERTIES\n";
    for (auto const& property : properties) {
      os << "  " << property.first << " "
         << Exporting::CMakeConfig::Escape(property.second) << "\n";
    }
    os << ")\n\n";
  }
}

bool cmExportFileGenerator::AddTargetNamespace(std::string& input,
                                               cmGeneratorTarget const* target,
                                               Context& ctx) const
{
  cmGeneratorTarget::TargetOrString resolved =
    target->ResolveTargetReference(input);

  cmGeneratorTarget* tgt = resolved.Target;
  if (!tgt) {
    input = resolved.String;
    return false;
  }

  if (tgt->IsImported()) {
    input = tgt->GetName();
    return true;
  }
  if (std::find(ctx.ExportedTargets.begin(), ctx.ExportedTargets.end(), tgt) !=
      ctx.ExportedTargets.end()) {
    input = this->Namespace + tgt->GetExportName();
  } else {
    std::string namespacedTarget;
    this->HandleMissingTarget(namespacedTarget, ctx.MissingTargets, target,
                              tgt);
    if (!namespacedTarget.empty()) {
      input = namespacedTarget;
    } else {
      input = tgt->GetName();
    }
  }
  return true;
}

void cmExportFileGenerator::ResolveTargetsInGeneratorExpressions(
  std::string& input, cmGeneratorTarget const* target, Context& ctx,
  FreeTargetsReplace replace) const
{
  if (replace == NoReplaceFreeTargets) {
    this->ResolveTargetsInGeneratorExpression(input, target, ctx);
    return;
  }
  std::vector<std::string> parts;
  cmGeneratorExpression::Split(input, parts);

  std::string sep;
  input.clear();
  for (std::string& li : parts) {
    if (cmHasLiteralPrefix(li, CMAKE_DIRECTORY_ID_SEP)) {
      continue;
    }
    if (cmGeneratorExpression::Find(li) == std::string::npos) {
      this->AddTargetNamespace(li, target, ctx);
    } else {
      this->ResolveTargetsInGeneratorExpression(li, target, ctx);
    }
    input += sep + li;
    sep = ";";
  }
}

void cmExportFileGenerator::ResolveTargetsInGeneratorExpression(
  std::string& input, cmGeneratorTarget const* target, Context& ctx) const
{
  std::string::size_type pos = 0;
  std::string::size_type lastPos = pos;

  while ((pos = input.find("$<TARGET_PROPERTY:", lastPos)) !=
         std::string::npos) {
    std::string::size_type nameStartPos =
      pos + sizeof("$<TARGET_PROPERTY:") - 1;
    std::string::size_type closePos = input.find('>', nameStartPos);
    std::string::size_type commaPos = input.find(',', nameStartPos);
    std::string::size_type nextOpenPos = input.find("$<", nameStartPos);
    if (commaPos == std::string::npos    // Implied 'this' target
        || closePos == std::string::npos // Incomplete expression.
        || closePos < commaPos           // Implied 'this' target
        || nextOpenPos < commaPos)       // Non-literal
    {
      lastPos = nameStartPos;
      continue;
    }

    std::string targetName =
      input.substr(nameStartPos, commaPos - nameStartPos);

    if (this->AddTargetNamespace(targetName, target, ctx)) {
      input.replace(nameStartPos, commaPos - nameStartPos, targetName);
    }
    lastPos = nameStartPos + targetName.size() + 1;
  }

  std::string errorString;
  pos = 0;
  lastPos = pos;
  while ((pos = input.find("$<TARGET_NAME:", lastPos)) != std::string::npos) {
    std::string::size_type nameStartPos = pos + sizeof("$<TARGET_NAME:") - 1;
    std::string::size_type endPos = input.find('>', nameStartPos);
    if (endPos == std::string::npos) {
      errorString = "$<TARGET_NAME:...> expression incomplete";
      break;
    }
    std::string targetName = input.substr(nameStartPos, endPos - nameStartPos);
    if (targetName.find("$<") != std::string::npos) {
      errorString = "$<TARGET_NAME:...> requires its parameter to be a "
                    "literal.";
      break;
    }
    if (!this->AddTargetNamespace(targetName, target, ctx)) {
      errorString = "$<TARGET_NAME:...> requires its parameter to be a "
                    "reachable target.";
      break;
    }
    input.replace(pos, endPos - pos + 1, targetName);
    lastPos = pos + targetName.size();
  }

  pos = 0;
  lastPos = pos;
  while (errorString.empty() &&
         (pos = input.find("$<LINK_ONLY:", lastPos)) != std::string::npos) {
    std::string::size_type nameStartPos = pos + sizeof("$<LINK_ONLY:") - 1;
    std::string::size_type endPos = input.find('>', nameStartPos);
    if (endPos == std::string::npos) {
      errorString = "$<LINK_ONLY:...> expression incomplete";
      break;
    }
    std::string libName = input.substr(nameStartPos, endPos - nameStartPos);
    if (cmGeneratorExpression::IsValidTargetName(libName) &&
        this->AddTargetNamespace(libName, target, ctx)) {
      input.replace(nameStartPos, endPos - nameStartPos, libName);
    }
    lastPos = nameStartPos + libName.size() + 1;
  }

  this->ReplaceInstallPrefix(input);

  if (!errorString.empty()) {
    target->GetLocalGenerator()->IssueMessage(MessageType::FATAL_ERROR,
                                              errorString);
  }
}

void cmExportFileGenerator::ReplaceInstallPrefix(std::string& /*unused*/) const
{
  // Do nothing
}

void cmExportFileGenerator::SetImportLinkInterface(
  const std::string& config, std::string const& suffix,
  cmGeneratorExpression::PreprocessContext preprocessRule,
  cmGeneratorTarget const* target, ImportPropertyMap& properties, Context& ctx)
{
  // Add the transitive link dependencies for this configuration.
  cmLinkInterface const* iface = target->GetLinkInterface(config, target);
  if (!iface) {
    return;
  }

  if (iface->ImplementationIsInterface) {
    // Policy CMP0022 must not be NEW.
    this->SetImportLinkProperty(
      suffix, target, "IMPORTED_LINK_INTERFACE_LIBRARIES", iface->Libraries,
      properties, ctx, ImportLinkPropertyTargetNames::Yes);
    return;
  }

  cmProp propContent;

  if (cmProp prop_suffixed =
        target->GetProperty("LINK_INTERFACE_LIBRARIES" + suffix)) {
    propContent = prop_suffixed;
  } else if (cmProp prop = target->GetProperty("LINK_INTERFACE_LIBRARIES")) {
    propContent = prop;
  } else {
    return;
  }

  const bool newCMP0022Behavior =
    target->GetPolicyStatusCMP0022() != cmPolicies::WARN &&
    target->GetPolicyStatusCMP0022() != cmPolicies::OLD;

  if (newCMP0022Behavior && !this->ExportOld) {
    cmLocalGenerator* lg = target->GetLocalGenerator();
    std::ostringstream e;
    e << "Target \"" << target->GetName()
      << "\" has policy CMP0022 enabled, "
         "but also has old-style LINK_INTERFACE_LIBRARIES properties "
         "populated, but it was exported without the "
         "EXPORT_LINK_INTERFACE_LIBRARIES to export the old-style properties";
    lg->IssueMessage(MessageType::FATAL_ERROR, e.str());
    return;
  }

  if (propContent->empty()) {
    properties["IMPORTED_LINK_INTERFACE_LIBRARIES" + suffix].clear();
    return;
  }

  std::string prepro =
    cmGeneratorExpression::Preprocess(*propContent, preprocessRule);
  if (!prepro.empty()) {
    this->ResolveTargetsInGeneratorExpressions(prepro, target, ctx,
                                               ReplaceFreeTargets);
    properties["IMPORTED_LINK_INTERFACE_LIBRARIES" + suffix] = prepro;
  }
}

void cmExportFileGenerator::SetImportDetailProperties(
  const std::string& config, std::string const& suffix,
  cmGeneratorTarget const* target, ImportPropertyMap& properties, Context& ctx)
{
  // Get the makefile in which to lookup target information.
  cmMakefile* mf = target->Makefile;

  // Add the soname for unix shared libraries.
  if (target->GetType() == cmStateEnums::SHARED_LIBRARY ||
      target->GetType() == cmStateEnums::MODULE_LIBRARY) {
    if (!target->IsDLLPlatform()) {
      std::string prop;
      std::string value;
      if (target->HasSOName(config)) {
        if (mf->IsOn("CMAKE_PLATFORM_HAS_INSTALLNAME")) {
          value = this->InstallNameDir(target, config);
        }
        prop = "IMPORTED_SONAME";
        value += target->GetSOName(config);
      } else {
        prop = "IMPORTED_NO_SONAME";
        value = "TRUE";
      }
      prop += suffix;
      properties[prop] = value;
    }
  }

  // Add the transitive link dependencies for this configuration.
  if (cmLinkInterface const* iface =
        target->GetLinkInterface(config, target)) {
    this->SetImportLinkProperty(
      suffix, target, "IMPORTED_LINK_INTERFACE_LANGUAGES", iface->Languages,
      properties, ctx, ImportLinkPropertyTargetNames::No);

    Context dummy{ {}, {} };
    this->SetImportLinkProperty(
      suffix, target, "IMPORTED_LINK_DEPENDENT_LIBRARIES", iface->SharedDeps,
      properties, dummy, ImportLinkPropertyTargetNames::Yes);
    if (iface->Multiplicity > 0) {
      std::string prop =
        cmStrCat("IMPORTED_LINK_INTERFACE_MULTIPLICITY", suffix);
      properties[prop] = std::to_string(iface->Multiplicity);
    }
  }

  // Add information if this target is a managed target
  if (target->GetManagedType(config) !=
      cmGeneratorTarget::ManagedType::Native) {
    std::string prop = cmStrCat("IMPORTED_COMMON_LANGUAGE_RUNTIME", suffix);
    std::string propval;
    if (cmProp p = target->GetProperty("COMMON_LANGUAGE_RUNTIME")) {
      propval = *p;
    } else if (target->IsCSharpOnly()) {
      // C# projects do not have the /clr flag, so we set the property
      // here to mark the target as (only) managed (i.e. no .lib file
      // to link to). Otherwise the  COMMON_LANGUAGE_RUNTIME target
      // property would have to be set manually for C# targets to make
      // exporting/importing work.
      propval = "CSharp";
    }
    properties[prop] = propval;
  }
}

static std::string const& asString(std::string const& l)
{
  return l;
}

static std::string const& asString(cmLinkItem const& l)
{
  return l.AsStr();
}

template <typename T>
void cmExportFileGenerator::SetImportLinkProperty(
  std::string const& suffix, cmGeneratorTarget const* target,
  const std::string& propName, std::vector<T> const& entries,
  ImportPropertyMap& properties, Context& ctx,
  ImportLinkPropertyTargetNames targetNames)
{
  // Skip the property if there are no entries.
  if (entries.empty()) {
    return;
  }

  // Construct the property value.
  std::string link_entries;
  const char* sep = "";
  for (T const& l : entries) {
    // Separate this from the previous entry.
    link_entries += sep;
    sep = ";";

    if (targetNames == ImportLinkPropertyTargetNames::Yes) {
      std::string temp = asString(l);
      this->AddTargetNamespace(temp, target, ctx);
      link_entries += temp;
    } else {
      link_entries += asString(l);
    }
  }

  // Store the property.
  std::string prop = cmStrCat(propName, suffix);
  properties[prop] = link_entries;
}

bool cmExportFileGenerator::PopulateExportProperties(
  cmGeneratorTarget const* gte, ImportPropertyMap& properties,
  std::string& errorMessage) const
{
  const auto& targetProperties = gte->Target->GetProperties();
  if (cmProp exportProperties =
        targetProperties.GetPropertyValue("EXPORT_PROPERTIES")) {
    for (auto& prop : cmExpandedList(*exportProperties)) {
      /* Black list reserved properties */
      if (cmHasLiteralPrefix(prop, "IMPORTED_") ||
          cmHasLiteralPrefix(prop, "INTERFACE_")) {
        std::ostringstream e;
        e << "Target \"" << gte->Target->GetName() << "\" contains property \""
          << prop << "\" in EXPORT_PROPERTIES but IMPORTED_* and INTERFACE_* "
          << "properties are reserved.";
        errorMessage = e.str();
        return false;
      }
      cmProp propertyValue = targetProperties.GetPropertyValue(prop);
      if (!propertyValue) {
        // Asked to export a property that isn't defined on the target. Do not
        // consider this an error, there's just nothing to export.
        continue;
      }
      std::string evaluatedValue = cmGeneratorExpression::Preprocess(
        *propertyValue, cmGeneratorExpression::StripAllGeneratorExpressions);
      if (evaluatedValue != *propertyValue) {
        std::ostringstream e;
        e << "Target \"" << gte->Target->GetName() << "\" contains property \""
          << prop << "\" in EXPORT_PROPERTIES but this property contains a "
          << "generator expression. This is not allowed.";
        errorMessage = e.str();
        return false;
      }
      properties[prop] = *propertyValue;
    }
  }
  return true;
}

void cmExportFileGenerator::ComplainAboutMissingTarget(
  cmGeneratorTarget const* depender, cmGeneratorTarget const* dependee,
  const std::vector<std::string>& exportFiles) const
{
  std::set<cmGeneratorTarget const*>& complainedDependees =
    this->ComplainedTargets[depender];
  if (!complainedDependees.insert(dependee).second) {
    return;
  }
  std::ostringstream e;
  this->InvocationSyntax(e);
  e << " target \"" << depender->GetName() << "\" which requires target \""
    << dependee->GetName() << "\" ";
  if (exportFiles.empty()) {
    e << "that is not in any export set.";
  } else {
    e << "that is not in this export set, but in multiple other export sets: "
      << cmJoin(exportFiles, ", ")
      << ".\n"
         "An exported target cannot depend upon another target which is "
         "exported multiple times. Consider consolidating the exports of the "
         "\""
      << dependee->GetName() << "\" target to a single export.";
  }

  auto const* lg = depender->GetLocalGenerator();
  lg->GetGlobalGenerator()->GetCMakeInstance()->IssueMessage(
    MessageType::FATAL_ERROR, e.str(),
    this->GetBacktrace(*depender->Makefile));
}

cmListFileBacktrace cmExportFileGenerator::GetBacktrace(
  const cmMakefile& mf) const
{
  return mf.GetBacktrace();
}

cmExportFileGenerator::ImportPropertyMap
cmExportFileGenerator::CommonInterfaceProperties(
  cmGeneratorTarget const* target,
  cmGeneratorExpression::PreprocessContext preprocessRule, Context& ctx,
  bool includeInterfaceLinkLibraries) const
{
  ImportPropertyMap properties;
  this->PopulateCommonInterfaceProperties(target, properties, preprocessRule,
                                          ctx, includeInterfaceLinkLibraries);
  return properties;
}

void cmExportFileGenerator::PopulateCommonInterfaceProperties(
  cmGeneratorTarget const* gt,
  cmExportFileGenerator::ImportPropertyMap& properties,
  cmGeneratorExpression::PreprocessContext preprocessRule, Context& ctx,
  bool includeInterfaceLinkLibraries) const
{
  this->PopulateIncludeDirectoriesInterface(gt, preprocessRule, properties,
                                            ctx);
  this->PopulateInterfaceProperty("INTERFACE_SYSTEM_INCLUDE_DIRECTORIES", gt,
                                  preprocessRule, properties, ctx);
  this->PopulateInterfaceProperty("INTERFACE_COMPILE_DEFINITIONS", gt,
                                  preprocessRule, properties, ctx);
  this->PopulateInterfaceProperty("INTERFACE_COMPILE_OPTIONS", gt,
                                  preprocessRule, properties, ctx);
  this->PopulateInterfaceProperty("INTERFACE_PRECOMPILE_HEADERS", gt,
                                  preprocessRule, properties, ctx);
  this->PopulateInterfaceProperty("INTERFACE_AUTOUIC_OPTIONS", gt,
                                  preprocessRule, properties, ctx);
  this->PopulateInterfaceProperty("INTERFACE_COMPILE_FEATURES", gt,
                                  preprocessRule, properties, ctx);
  this->PopulateInterfaceProperty("INTERFACE_LINK_OPTIONS", gt, preprocessRule,
                                  properties, ctx);
  this->PopulateLinkDirectoriesInterface(gt, preprocessRule, properties, ctx);
  this->PopulateLinkDependsInterface(gt, preprocessRule, properties, ctx);
  if (includeInterfaceLinkLibraries) {
    this->PopulateInterfaceLinkLibrariesProperty(gt, preprocessRule,
                                                 properties, ctx);
  }
}
