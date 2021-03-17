/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#include "cmGlobalUVisionGenerator.h"

#include "cmsys/FStream.hxx"

#include "cmDocumentationEntry.h"
#include "cmGeneratedFileStream.h"
#include "cmGeneratorTarget.h"
#include "cmLocalUVisionGenerator.h"
#include "cmMakefile.h"
#include "cmXMLEditor.h"
#include "cmake.h"

cmGlobalUVisionGenerator::cmGlobalUVisionGenerator(cmake* cm)
  : cmGlobalGenerator(cm)
{
  this->FindMakeProgramFile = "CMakeUVisionFindMake.cmake";
}

cmGlobalUVisionGenerator::~cmGlobalUVisionGenerator() = default;

void cmGlobalUVisionGenerator::GetDocumentation(cmDocumentationEntry& entry)
{
  entry.Name = cmGlobalUVisionGenerator::GetActualName();
  entry.Brief = "Generates uVision Project files.";
}

bool cmGlobalUVisionGenerator::SetGeneratorInstance(std::string const& i,
                                                    cmMakefile* mf)
{
  std::string genInstance = mf->GetSafeDefinition("CMAKE_GENERATOR_INSTANCE");
  if (i != genInstance) {
    this->CMakeInstance->AddCacheEntry("CMAKE_GENERATOR_INSTANCE", i.c_str(),
                                       "Generator instance identifier.",
                                       cmStateEnums::INTERNAL);
  }

  return true;
}

std::unique_ptr<cmLocalGenerator>
cmGlobalUVisionGenerator::CreateLocalGenerator(cmMakefile* mf)
{
  if (!mf->GetDefinition("CMAKE_CONFIGURATION_TYPES")) {
    mf->AddCacheDefinition(
      "CMAKE_CONFIGURATION_TYPES", "Debug;Release",
      "Semicolon separated list of supported configuration types.",
      cmStateEnums::STRING);
  }

  mf->GetOrCreateSourceGroup(UV_CMAKE_GROUP)->SetGroupRegex("CMakeLists.txt$");
  mf->GetOrCreateSourceGroup(UV_LIBRARIES_GROUP)
    ->SetGroupRegex("\\.(lib|Lib|LIB)$");
  mf->GetOrCreateSourceGroup(UV_SCATTER_FILES_GROUP)
    ->SetGroupRegex("\\.(sct|Sct|SCT)$");

  return std::unique_ptr<cmLocalGenerator>(
    cm::make_unique<cmLocalUVisionGenerator>(this, mf));
}

void cmGlobalUVisionGenerator::Generate()
{
  for (auto& it : this->ProjectMap) {
    this->CleanRTE(it.second[0]);
  }

  cmGlobalGenerator::Generate();

  if (!cmSystemTools::GetErrorOccuredFlag()) {
    for (auto& it : this->ProjectMap) {
      this->GenerateMultiProjectWorkspace(it.second[0], it.second);
    }
  }
}

void cmGlobalUVisionGenerator::CleanRTE(cmLocalGenerator* root)
{
  // Delete the RTE subfolder (we want uVision to rediscover RTEs)
  cmSystemTools::RepeatedRemoveDirectory(root->GetCurrentBinaryDirectory() +
                                         "/RTE");
}

void cmGlobalUVisionGenerator::GenerateMultiProjectWorkspace(
  cmLocalGenerator* root, std::vector<cmLocalGenerator*>& generators)
{
  std::string workspaceFile = root->GetCurrentBinaryDirectory() + "/" +
    root->GetProjectName() + ".uvmpw";
  cmGeneratedFileStream fs(workspaceFile);
  cmXMLWriter xml(fs);
  xml.SetIndentationElement("  ");

  xml.StartDocument();

  xml.StartElement("ProjectWorkspace");
  xml.Attribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
  xml.Attribute("xsi:noNamespaceSchemaLocation", "project_mpw.xsd");

  xml.Element("SchemaVersion", "1.0");
  xml.Element("Header", "### uVision Project, (C) Keil Software");
  xml.Element("WorkspaceName", root->GetProjectName());

  std::string startupProject =
    cmToCStrSafe(root->GetMakefile()->GetProperty("UV_STARTUP_PROJECT"));

  TargetDependSet projectTargets;
  TargetDependSet originalTargets;
  this->GetTargetSets(projectTargets, originalTargets, root, generators);
  OrderedTargetDependSet sortedProjectTargets(projectTargets, startupProject);
  std::vector<cmGeneratorTarget const*> targets;
  for (auto const& t : sortedProjectTargets) {
    if (!t->IsInBuildSystem()) {
      continue;
    }
    targets.push_back(t);
  }

  if (!targets.empty()) {
    if (startupProject.empty()) {
      // Determine a default startup project
      auto firstExecutable = std::find_if(
        targets.begin(), targets.end(), [startupProject](auto target) {
          return target->GetType() == cmStateEnums::EXECUTABLE;
        });
      if (firstExecutable != targets.end()) {
        startupProject = (*firstExecutable)->GetName();
      } else {
        startupProject = (*targets.rbegin())->GetName();
      }
    }

    std::vector<cmGeneratorTarget const*> build;
    std::vector<std::string> buildTargetNames;
    if (ComputeTargetBuildOrder(targets, build)) {
      std::string message = "The inter-target dependency graph for project [" +
        root->GetProjectName() + "] had a cycle.\n";
      cmSystemTools::Error(message);
    } else {
      for (cmGeneratorTarget const* target : build) {
        if (target->GetType() == cmStateEnums::INTERFACE_LIBRARY ||
            target->GetType() == cmStateEnums::MODULE_LIBRARY ||
            target->GetType() == cmStateEnums::SHARED_LIBRARY) {
          continue;
        }
        xml.StartElement("project");
        std::string projectPath = cmSystemTools::ForceToRelativePath(
          root->GetCurrentBinaryDirectory(),
          target->GetSafeProperty("UV_PROJECT"));
        xml.Element("PathAndName", "./" + projectPath);
        if (target->GetName() == startupProject) {
          xml.Element("NodeIsActive", "1");
        }
        xml.EndElement();

        buildTargetNames.push_back(target->GetName());
      }
    }

    CMakeInstance->AddCacheEntry(root->GetProjectName() + "_ALL_BUILD_TARGETS",
                                 cmJoin(buildTargetNames, ";").c_str(),
                                 "Default build targets",
                                 cmStateEnums::STATIC);
  }

  xml.EndElement();

  xml.EndDocument();
  fs.Close();
}

void cmGlobalUVisionGenerator::AppendDirectoryForConfig(
  std::string const& prefix, std::string const& config,
  std::string const& suffix, std::string& dir)
{
  if (!config.empty()) {
    dir += prefix;
    dir += config;
    dir += suffix;
  }
}

bool cmGlobalUVisionGenerator::SetGeneratorToolset(std::string const& ts, bool,
                                                   cmMakefile* mf)
{
  std::string toolSet = ts;
  if (toolSet.empty()) {
    toolSet = "ARMCLANG";
    cmSystemTools::Message("No toolset set, defaulting to \"ARMCLANG\".");

    mf->AddCacheDefinition("CMAKE_GENERATOR_TOOLSET", toolSet.c_str(),
                           "Name of generator toolset.",
                           cmStateEnums::INTERNAL);
  }

  if (!this->FindMakeProgram(mf)) {
    return false;
  }

  const std::string uv4Path = mf->GetSafeDefinition("CMAKE_MAKE_PROGRAM");
  const std::string keilPath = cmSystemTools::GetRealPath(uv4Path + "/../..");

  // Check for Tools.ini as a very simple check, if it is a Keil MDK
  if (!cmSystemTools::FileExists(keilPath + "/TOOLS.INI")) {
    cmSystemTools::Error("\"" + keilPath +
                         "\" is not a valid path to a Keil MDK!");
    return false;
  }

  if (toolSet == "GNU") {
    cmSystemTools::Error("The GNU toolset is not supported.");
    return false;
  } else {
    const std::string toolsetPath = keilPath + "/ARM/" + toolSet + "/bin";
    std::string compilerID;
    std::string cCompilerPath;
    if (cmSystemTools::FileExists(toolsetPath + "/armclang.exe")) {
      // ARMCLANG toolset
      compilerID = "ARMClang";
      cCompilerPath = toolsetPath + "/armclang.exe";
    } else if (cmSystemTools::FileExists(toolsetPath + "/armcc.exe")) {
      // ARMCC toolset
      compilerID = "ARMCC";
      cCompilerPath = toolsetPath + "/armcc.exe";
    } else {
      cmSystemTools::Error("Unknown toolset \"" + toolSet + "\"!");
      return false;
    }

    mf->AddDefinition("CMAKE_C_COMPILER", cCompilerPath);
    mf->AddDefinition("CMAKE_CXX_COMPILER", cCompilerPath);

    std::string asmCompilerID = mf->GetSafeDefinition("CMAKE_ASM_COMPILER_ID");
    if (asmCompilerID.empty()) {
      asmCompilerID = compilerID;
    }

    if (asmCompilerID == "ARMClang" && compilerID == "ARMClang") {
      // Use Clang
      mf->AddDefinition("CMAKE_ASM_COMPILER", cCompilerPath);
    } else if (asmCompilerID == "ARMCC") {
      // Use armasm
      const std::string asmCompilerPath = toolsetPath + "/armasm.exe";
      mf->AddDefinition("CMAKE_ASM_COMPILER", asmCompilerPath);
    } else {
      cmSystemTools::Error("Invalid CMAKE_ASM_COMPILER_ID \"" + asmCompilerID +
                           "\"!");
      return false;
    }
    mf->AddDefinition("CMAKE_ASM_COMPILER_FORCED", "TRUE");
  }

  return true;
}

std::string cmGlobalUVisionGenerator::GetToolsetVersionString(
  cmMakefile* mf) const
{
  std::string versionString;
  std::string compilerPath = mf->GetSafeDefinition("CMAKE_C_COMPILER");
  std::string toolset = mf->GetSafeDefinition("CMAKE_GENERATOR_TOOLSET");

  std::string commandOutput;
  if (cmSystemTools::RunSingleCommand(compilerPath + " --help", &commandOutput,
                                      nullptr, nullptr, nullptr,
                                      cmSystemTools::OUTPUT_NONE)) {
    cmsys::RegularExpression versionReg("Component: ARM Compiler ([^\r\n]+)");
    if (versionReg.find(commandOutput)) {
      std::string version = versionReg.match(1);

      cmsys::RegularExpression componentReg(
        "([0-9])\\.([0-9]+)(.([0-9]+))?(.*build ([0-9]+))?");
      if (componentReg.find(version)) {
        std::string majorVersion = componentReg.match(1);
        std::string minorVersion = componentReg.match(2);
        std::string buildNumber = componentReg.match(4);
        if (buildNumber.empty()) {
          buildNumber = componentReg.match(6);
        }
        if (buildNumber.length() < 4) {
          buildNumber =
            std::string(4 - buildNumber.length(), '0') + buildNumber;
        }
        versionString = majorVersion + minorVersion + buildNumber + "::V" +
          version + "::.\\" + toolset;
      } else {
        cmSystemTools::Error("Could not determine toolset version from \"" +
                             versionReg.match(1) + "\".");
      }
    }
  }

  if (versionString.empty()) {
    cmSystemTools::Error("Could not determine version of \"" + compilerPath +
                         "\"!");
  }

  return versionString;
}

void cmGlobalUVisionGenerator::EnableLanguage(
  std::vector<std::string> const& l, cmMakefile* mf, bool optional)
{
  mf->AddDefinition("CMAKE_SYSTEM_NAME", "uVision");
  mf->AddDefinition("CMAKE_CROSSCOMPILING", "1");
  mf->AddDefinition("CMAKE_TRY_COMPILE_TARGET_TYPE", "STATIC_LIBRARY");
  mf->AddDefinition("CMAKE_CXX_LINK_WITH_STANDARD_COMPILE_OPTION", "1");
  cmGlobalGenerator::EnableLanguage(l, mf, optional);
}

std::string cmGlobalUVisionGenerator::GetProjectNameForDirectory(
  std::string const& projectName, std::string const& projectDir) const
{
  auto cacheEntryKeys = this->CMakeInstance->GetState()->GetCacheEntryKeys();
  for (auto cacheEntryKey : cacheEntryKeys) {
    if (cmHasSuffix(cacheEntryKey, "_BINARY_DIR") &&
        cmSystemTools::ComparePath(
          *this->CMakeInstance->GetCacheDefinition(cacheEntryKey),
          projectDir)) {
      cmStripSuffixIfExists(cacheEntryKey, "_BINARY_DIR");
      return cacheEntryKey;
    }
  }
  return projectName;
}

std::vector<cmGlobalGenerator::GeneratedMakeCommand>
cmGlobalUVisionGenerator::GenerateBuildCommand(
  std::string const& makeProgram, std::string const& projectName,
  std::string const& projectDir, std::vector<std::string> const& targetNames,
  std::string const& config, bool /*fast*/, int /*jobs*/, bool /*verbose*/,
  std::vector<std::string> const& makeOptions = std::vector<std::string>())
{
  std::vector<cmGlobalGenerator::GeneratedMakeCommand> makeCommands;

  std::string uv4 = this->SelectMakeProgram(makeProgram);

  auto realProjectName = GetProjectNameForDirectory(projectName, projectDir);
  std::string workspaceFile = projectDir + "/" + realProjectName + ".uvmpw";
  if (!cmSystemTools::FileExists(workspaceFile)) {
    cmSystemTools::Error("Could not find " + workspaceFile);
    return makeCommands;
  }

  std::string logDir = projectDir + "/CMakeFiles/CMakeTmp/";
  cmSystemTools::MakeDirectory(logDir);

  bool clean = !targetNames.empty() && targetNames.front() == "clean";

  auto actualTargetNames = targetNames;
  if (targetNames.empty() || targetNames.front().empty() || clean) {
    const auto* allBuildTargets = this->CMakeInstance->GetCacheDefinition(
      realProjectName + "_ALL_BUILD_TARGETS");
    if (allBuildTargets) {
      cmExpandList(*allBuildTargets, actualTargetNames, false);
    }
  }

  // Get the project files for the targets from the multi project workspace
  std::vector<std::string> projectFiles;
  cmXMLEditorDocument workspace(workspaceFile);
  std::vector<cmXMLEditorElement*> projectElements;
  workspace.GetElements("/ProjectWorkspace/project/PathAndName",
                        projectElements);
  std::vector<std::string> targetsFound;
  for (auto* projectElement : projectElements) {
    std::string projectPath = projectElement->GetTextContent();
    for (std::string const& targetName : actualTargetNames) {
      if (cmSystemTools::GetFilenameWithoutLastExtension(projectPath) ==
          targetName) {
        projectFiles.push_back(projectDir + "/" + projectPath);
        targetsFound.push_back(targetName);
        break;
      }
    }
  }

  // Check for missing targets
  std::vector<std::string> targetsNotFound;
  std::sort(targetsFound.begin(), targetsFound.end());
  std::sort(actualTargetNames.begin(), actualTargetNames.end());
  std::set_difference(targetsFound.begin(), targetsFound.end(),
                      actualTargetNames.begin(), actualTargetNames.end(),
                      targetsNotFound.begin());
  if (!targetsNotFound.empty()) {
    cmSystemTools::Error("Could not find projects for targets: " +
                         cmJoin(targetsNotFound, ", "));
    return makeCommands;
  }

  // Add make commands for every project file
  for (auto const& projectFile : projectFiles) {
    std::string logFile;

    GeneratedMakeCommand makeCommand;
    makeCommand.Add("cmd", "/V:ON", "/C");
    makeCommand.Add(uv4);

    if (clean) {
      makeCommand.Add("-c", projectFile); // clean
      logFile = logDir + "clean_" +
        cmSystemTools::GetFilenameWithoutExtension(projectFile);
    } else {
      makeCommand.Add("-b", projectFile); // build
      logFile = logDir + "build_" +
        cmSystemTools::GetFilenameWithoutExtension(projectFile);

      if (!config.empty()) {
        makeCommand.Add("-t", config); // target (configuration)
        logFile += "_" + config;
      }
    }

    makeCommand.Add("-j0"); // hide IDE

    makeCommand.Add(makeOptions.begin(), makeOptions.end());

    logFile += ".txt";
    logFile = cmSystemTools::ConvertToWindowsOutputPath(logFile);
    makeCommand.Add("-o", logFile);

    makeCommand.Add("&", "set", "BUILD_RESULT=!errorlevel!");
    makeCommand.Add("&");
    makeCommand.Add(cmSystemTools::GetCMakeCommand(), "-E", "cat", logFile);
    makeCommand.Add("&", "exit", "!BUILD_RESULT!");

    makeCommands.push_back(std::move(makeCommand));
  }

  return makeCommands;
}

bool cmGlobalUVisionGenerator::Open(std::string const& binDir,
                                    std::string const& projectName,
                                    bool /*dryRun*/)
{
  const std::string projectFile = binDir + "/" + projectName + ".uvmpw";
  const std::string uv4 = this->SelectMakeProgram("");
  return cmSystemTools::RunSingleCommand({ uv4, projectFile });
}

bool cmGlobalUVisionGenerator::TargetCompare::operator()(
  cmGeneratorTarget const* l, cmGeneratorTarget const* r) const
{
  // Make sure a given named target is ordered first,
  // e.g. to set ALL_BUILD as the default active project.
  // When the empty string is named this is a no-op.
  if (r->GetName() == this->First) {
    return false;
  }
  if (l->GetName() == this->First) {
    return true;
  }
  return l->GetName() < r->GetName();
}

cmGlobalUVisionGenerator::OrderedTargetDependSet::OrderedTargetDependSet(
  TargetDependSet const& targets, std::string const& first)
  : derived(TargetCompare(first))
{
  this->insert(targets.begin(), targets.end());
}

bool cmGlobalUVisionGenerator::ComputeTargetBuildOrder(
  cmGeneratorTarget const* tgt, std::vector<cmGeneratorTarget const*>& build)
{
  std::vector<cmGeneratorTarget const*> t{ tgt };
  return ComputeTargetBuildOrder(t, build);
}

bool cmGlobalUVisionGenerator::ComputeTargetBuildOrder(
  std::vector<cmGeneratorTarget const*>& tgt,
  std::vector<cmGeneratorTarget const*>& build)
{
  std::set<cmGeneratorTarget const*> temp;
  std::set<cmGeneratorTarget const*> perm;

  for (const auto* ti : tgt) {
    const bool r = VisitTarget(temp, perm, build, ti);
    if (r) {
      return r;
    }
  }
  return false;
}

bool cmGlobalUVisionGenerator::VisitTarget(
  std::set<cmGeneratorTarget const*>& temp,
  std::set<cmGeneratorTarget const*>& perm,
  std::vector<cmGeneratorTarget const*>& order, cmGeneratorTarget const* ti)
{
  /* check if permanent mark is set*/
  if (perm.find(ti) == perm.end()) {
    /* set temporary mark; check if revisit*/
    if (temp.insert(ti).second) {
      /* sort targets lexicographically to ensure that nodes are always visited
       * in the same order */
      OrderedTargetDependSet sortedTargets(this->GetTargetDirectDepends(ti),
                                           "");
      for (auto const& di : sortedTargets) {
        if (this->VisitTarget(temp, perm, order, di)) {
          return true;
        }
      }
      /* mark as complete; insert into beginning of list*/
      perm.insert(ti);
      order.push_back(ti);
      return false;
    }
    /* revisiting item - not a DAG */
    return true;
  }
  /* already complete */
  return false;
}
