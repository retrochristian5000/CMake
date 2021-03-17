#include "cmUVisionTargetGenerator.h"

#include "cmComputeLinkInformation.h"
#include "cmCustomCommand.h"
#include "cmCustomCommandGenerator.h"
#include "cmGeneratedFileStream.h"
#include "cmGlobalUVisionGenerator.h"
#include "cmLocalUVisionGenerator.h"
#include "cmMakefile.h"
#include "cmSourceFile.h"
#include "cmUVisionGeneratorOptions.h"

cmUVisionTargetGenerator::cmUVisionTargetGenerator(cmGeneratorTarget* target)
  : Target(target)
  , LocalGenerator(
      dynamic_cast<cmLocalUVisionGenerator*>(target->GetLocalGenerator()))
  , GlobalGenerator(
      dynamic_cast<cmGlobalUVisionGenerator*>(target->GetGlobalGenerator()))
  , Name(target->GetName())
  , Makefile(target->Target->GetMakefile())
  , Configs(target->Target->GetMakefile()->GetGeneratorConfigs(
      cmMakefile::OnlyMultiConfig))
  , BinaryDirectory(LocalGenerator->GetCurrentBinaryDirectory())
{
}

cmUVisionTargetGenerator::~cmUVisionTargetGenerator() = default;

void cmUVisionTargetGenerator::Generate()
{
  this->TmpDirectory =
    this->Makefile->GetCurrentBinaryDirectory() + "/CMakeFiles/uVision";
  cmSystemTools::MakeDirectory(this->TmpDirectory);

  this->ComputeTargetSources();

  // Generate the base project file
  if (!this->GenerateProjectFile()) {
    cmSystemTools::RemoveADirectory(this->TmpDirectory);
    return;
  }

  // Load the project file
  cmXMLEditorDocument project(ProjectFile, "  ");
  cmXMLEditorElement* projectTargets = project.GetElement("/Project/Targets");
  cmXMLEditorElement* firstTarget = projectTargets->GetElement("Target");

  SetToolset(firstTarget);

  std::map<std::string, cmUVisionGeneratorOptions*> generatorOptions;

  for (size_t ci = 0; ci < Configs.size(); ci++) {
    cmXMLEditorElement* targetElement;
    if (ci == 0) {
      targetElement = firstTarget;
    } else {
      targetElement = projectTargets->AddChild(*firstTarget);
      targetElement->SetTextContent("TargetName", Configs[ci]);
    }

    generatorOptions[Configs[ci]] =
      new cmUVisionGeneratorOptions(Target, Configs[ci]);

    cmXMLEditorElement* targetArmAds =
      targetElement->GetElement("TargetOption/TargetArmAds");
    auto targetOptions = generatorOptions[Configs[ci]]->GetTargetOptions();
    for (auto const& option : targetOptions) {
      if (!(option.Options & cmUVFlagTableOption::UvOpt)) {
        const bool create =
          !(option.Options & cmUVFlagTableOption::OnlyIfPresent);
        if (option.Options & cmUVFlagTableOption::ArmAds) {
          targetArmAds->SetTextContent(option.Path, option.Value, create);
        } else {
          targetElement->SetTextContent(option.Path, option.Value, create);
        }
      }
    }

    this->SetTargetOptions(Configs[ci], targetElement);
    this->GenerateSourceGroups(targetElement, ci,
                               generatorOptions[Configs[ci]]);
  }

  if (Target->GetType() != cmStateEnums::UTILITY) {
    this->GenerateRTEs(project.GetElement("/Project"));
  }

  // Save the project file
  project.Save();

  // Force the generation of the options file
  GenerateOptionsFile();

  cmXMLEditorDocument options(OptionsFile, "  ");
  for (auto [config, generatorOptions] : generatorOptions) {
    cmXMLEditorElement* targetElement =
      options.GetElement("/ProjectOpt/Target[TargetName='" + config + "']");
    if (targetElement) {
      auto targetOptions = generatorOptions->GetTargetOptions();
      for (auto const& option : targetOptions) {
        if (option.Options & cmUVFlagTableOption::UvOpt) {
          const bool create =
            !(option.Options & cmUVFlagTableOption::OnlyIfPresent);
          targetElement->SetTextContent(option.Path, option.Value, create);
        }
      }
    }
  }
  options.Save();

  cmSystemTools::RemoveADirectory(this->TmpDirectory);
}

void cmUVisionTargetGenerator::ComputeTargetSources()
{
  auto allConfigSources = Target->GetAllConfigSources();
  std::map<cmSourceFile const*, size_t> index;

  // Add CMakeLists.txt to sources
  const auto makefile =
    this->Makefile->GetCurrentSourceDirectory() + "/CMakeLists.txt";
  cmSourceFile* sourceFile = Makefile->GetOrCreateSource(
    makefile, false, cmSourceFileLocationKind::Known);
  sourceFile->ResolveFullPath();

  auto mi = index.find(sourceFile);
  if (mi == index.end()) {
    cmGeneratorTarget::AllConfigSource acs;
    acs.Source = sourceFile;
    acs.Kind = cmGeneratorTarget::SourceKind::SourceKindExtra;
    allConfigSources.push_back(std::move(acs));
    mi = index.emplace(sourceFile, allConfigSources.size() - 1).first;
  }
  for (size_t ci = 0; ci < Configs.size(); ++ci) {
    allConfigSources[mi->second].Configs.push_back(ci);
  }

  // Assign source groups
  SourceGroups = this->Makefile->GetSourceGroups();
  for (auto source : allConfigSources) {
    std::string const& path = source.Source->ResolveFullPath();
    cmSourceGroup* sourceGroup = Makefile->FindSourceGroup(path, SourceGroups);
    sourceGroup->AssignSource(source.Source);
    GroupedSources[sourceGroup->GetFullName()].push_back(std::move(source));
  }

  // Convert linked libraries to source files
  for (auto& config : Configs) {
    auto* const linkInfo = Target->GetLinkInformation(config);
    if (linkInfo) {
      for (auto const& linkItem : linkInfo->GetItems()) {
        if (!linkItem.Target) {
          AddConfigSourceWithDisplayNameAndGroup(
            linkItem.Value.Value, cmGeneratorTarget::SourceKindExternalObject,
            "", config, UV_LIBRARIES_GROUP);
        } else if (linkItem.Target && !linkItem.Target->IsImported()) {
          const auto displayName =
            linkItem.Target->GetName() + " (" + config + ")";
          AddConfigSourceWithDisplayNameAndGroup(
            linkItem.Value.Value, cmGeneratorTarget::SourceKindExternalObject,
            displayName, config, UV_REFERENCES_GROUP);
        }
      }
    }
  }
}

bool cmUVisionTargetGenerator::GenerateProjectFile()
{
  std::string targetDevice = Target->GetSafeProperty("CMAKE_UV_DEVICE");
  if (targetDevice.empty()) {
    targetDevice =
      Target->GetGlobalGenerator()->GetSafeGlobalSetting("CMAKE_UV_DEVICE");
  }

  if (targetDevice.empty()) {
    cmSystemTools::Error("CMAKE_UV_DEVICE not set for target " + this->Name);
    return false;
  }

  const std::string projectFileBase = this->BinaryDirectory + "/" + this->Name;
  this->ProjectFile = projectFileBase + ".uvprojx";
  this->OptionsFile = projectFileBase + ".uvoptx";

  int retVal = GenerateUVisionProject(targetDevice);
  if (retVal == 12) {
    // 12 == Device not found. Try it with a non-x project
    this->ProjectFile = projectFileBase + ".uvproj";
    this->OptionsFile = projectFileBase + ".uvopt";
    retVal = GenerateUVisionProject(targetDevice);
  }

  if (retVal <= 1) {
    this->Target->Target->SetProperty("UV_PROJECT", this->ProjectFile.c_str());
  } else if (retVal == 12) {
    cmSystemTools::Error("Device with name \"" + targetDevice +
                         "\" not found in device database!");
  } else if (retVal > 1) {
    cmSystemTools::Error("Could not create project \"" + ProjectFile +
                         "\" (ERRORLEVEL " + std::to_string(retVal) + ")!\n");
  }

  return retVal <= 1;
}

int cmUVisionTargetGenerator::GenerateUVisionProject(
  std::string const& targetDevice)
{
  const std::string tmpProjectFile = this->TmpDirectory + "/" +
    cmSystemTools::GetFilenameName(this->ProjectFile);

  std::vector<std::string> generateProjectCommand;
  generateProjectCommand.push_back(
    Makefile->GetSafeDefinition("CMAKE_MAKE_PROGRAM"));
  generateProjectCommand.push_back(tmpProjectFile);
  generateProjectCommand.emplace_back("-n");
  generateProjectCommand.push_back(targetDevice);
  generateProjectCommand.emplace_back("-t");
  generateProjectCommand.push_back(Configs.front());
  generateProjectCommand.emplace_back("-j0");

  int retVal;
  cmSystemTools::RunSingleCommand(generateProjectCommand, nullptr, nullptr,
                                  &retVal, nullptr,
                                  cmSystemTools::OUTPUT_NONE);

  if (retVal <= 1) {
    cmSystemTools::RenameFile(tmpProjectFile, this->ProjectFile);
  } else if (retVal == 11) {
    cmSystemTools::Error("Could not open file \"" + tmpProjectFile +
                         "\" for writing!");
  }

  return retVal;
}

bool cmUVisionTargetGenerator::GenerateOptionsFile()
{
  // Cleaning the project, with a non-existent target forces the generation
  // of the options file, without doing anything else
  std::vector<std::string> generateOptionsFileCommand;
  generateOptionsFileCommand.push_back(
    Makefile->GetSafeDefinition("CMAKE_MAKE_PROGRAM"));
  generateOptionsFileCommand.emplace_back("-c");
  generateOptionsFileCommand.push_back(this->ProjectFile);
  generateOptionsFileCommand.emplace_back("-t\"ForceOptionsFile\"");
  generateOptionsFileCommand.emplace_back("-j0");

  int retVal;
  cmSystemTools::RunSingleCommand(generateOptionsFileCommand, nullptr, nullptr,
                                  &retVal, nullptr,
                                  cmSystemTools::OUTPUT_NONE);

  return cmSystemTools::FileExists(this->OptionsFile);
}

void cmUVisionTargetGenerator::SetToolset(
  cmXMLEditorElement* targetElement) const
{
  const std::string toolset =
    Makefile->GetSafeDefinition("CMAKE_GENERATOR_TOOLSET");
  if (toolset == "GNU") {
    targetElement->SetTextContent("ToolsetNumber", "0x3");
    targetElement->SetTextContent("ToolsetName", "ARM-GNU");
  } else {
    const std::string versionString =
      GlobalGenerator->GetToolsetVersionString(Makefile);
    targetElement->SetTextContent("pArmCC", versionString);
    targetElement->SetTextContent("ToolsetNumber", "0x4");
    targetElement->SetTextContent("ToolsetName", "ARM-ADS");
  }
}

void cmUVisionTargetGenerator::SetTargetOptions(
  std::string const& config, cmXMLEditorElement* projectTarget)
{
  cmXMLEditorElement* commonOption =
    projectTarget->GetElement("TargetOption/TargetCommonOption");
  if (!commonOption) {
    cmSystemTools::Error(
      "TargetCommonOption XML element not found in project file");
    return;
  }

  cmStateEnums::TargetType targetType = Target->GetType();
  bool executable = targetType == cmStateEnums::EXECUTABLE ||
    targetType == cmStateEnums::UTILITY;
  commonOption->SetTextContent("CreateExecutable", executable ? "1" : "0");
  commonOption->SetTextContent("CreateLib", !executable ? "1" : "0");

  std::string outputDirectory;
  std::string outputName;

  if (targetType == cmStateEnums::UTILITY) {
    // Utility target
    outputDirectory = this->BinaryDirectory + "/CMakeFiles/CustomTargets/" +
      Target->GetName() + "/Build/";
    outputName = Target->GetName();
  } else {
    // Set output name
    if (executable) {
      outputName = cmSystemTools::GetFilenameName(
        Target->GetExecutableNames(config).Output);
    } else {
      outputName =
        cmSystemTools::GetFilenameName(Target->GetLibraryNames(config).Output);
    }
    outputDirectory = cmSystemTools::RelativePath(
      this->BinaryDirectory, Target->GetDirectory(config));
  }

  // Set output directory
  if (outputDirectory.empty()) {
    outputDirectory = ".";
  }
  // uVision expects the output directory to end with a slash
  if (outputDirectory.back() != '\\') {
    outputDirectory += '\\';
  }
  std::replace(outputDirectory.begin(), outputDirectory.end(), '/', '\\');
  commonOption->SetTextContent("OutputDirectory", outputDirectory);
  // Set output name
  commonOption->SetTextContent("OutputName", outputName);

  // Check, if the configuration should be included in the default build
  cmProp excludeFromDefaultBuild =
    Target->GetFeature("EXCLUDE_FROM_DEFAULT_BUILD", config);
  bool includeInDefaultBuild = cmIsOff(excludeFromDefaultBuild);
  commonOption->SetTextContent("SelectedForBatchBuild",
                               includeInDefaultBuild ? "1" : "0");

  SetBuildEvents(commonOption, config);
}

void cmUVisionTargetGenerator::SetBuildEvents(cmXMLEditorElement* commonOption,
                                              std::string const& config)
{
  const auto preBuildCommands = Target->GetPreBuildCommands();
  const auto preLinkCommands = Target->GetPreLinkCommands();
  const auto postBuildCommands = Target->GetPostBuildCommands();

  if (!preBuildCommands.empty()) {
    SetBuildEvent("PreBuild", commonOption->GetElement("BeforeMake"), config,
                  preBuildCommands);
  }

  if (!preLinkCommands.empty() && !postBuildCommands.empty()) {
    cmSystemTools::Error("uVision only supports PreLink or PostBuild "
                         "commands, not both simultaneously.");
  } else if (!preLinkCommands.empty()) {
    SetBuildEvent("PreLink", commonOption->GetElement("AfterMake"), config,
                  preLinkCommands);
  } else if (!postBuildCommands.empty()) {
    SetBuildEvent("PostBuild", commonOption->GetElement("AfterMake"), config,
                  postBuildCommands);
    OptionsValues["TargetOption/OPTTT/RunAbUc"] = "1";
  }
}

void cmUVisionTargetGenerator::SetBuildEvent(
  std::string const& buildEvent, cmXMLEditorElement* uvBuildEvent,
  std::string const& config, std::vector<cmCustomCommand> const& commands)
{
  const std::string fileName = this->BinaryDirectory +
    "/CMakeFiles/BuildEvents/" + buildEvent + "_" + config + ".bat";
  const std::string command =
    GenerateCustomCommandBatchFile(commands, config, fileName);

  uvBuildEvent->SetTextContent("UserProg1Name", command);
  uvBuildEvent->SetTextContent("RunUserProg1", "1");
  uvBuildEvent->SetTextContent("nStopA1X", "2"); // stop on exit code > 0

  // Add build event batch file to the sources
  const std::string displayName = buildEvent + " (" + config + ")";
  AddConfigSourceWithDisplayNameAndGroup(
    fileName, cmGeneratorTarget::SourceKindExtra, displayName, config,
    UV_BUILD_EVENTS_GROUP);
}

static std::string EscapeCommandComment(std::string const& comment)
{
  std::string escapedComment;
  for (char c : comment) {
    switch (c) {
      case '\n':
        escapedComment += '\n';
        escapedComment += "echo ";
        break;
      case '"': /* no break */
      case '|': /* no break */
      case '&': /* no break */
      case '<': /* no break */
      case '>': /* no break */
      case '^':
        escapedComment += '^'; /* no break */
        CM_FALLTHROUGH;
      default:
        escapedComment += c;
        break;
    }
  }
  return escapedComment;
}

std::string cmUVisionTargetGenerator::GenerateCustomCommandBatchFile(
  std::vector<cmCustomCommand> const& commands, std::string const& config,
  std::string const& file) const
{
  cmGeneratedFileStream fs(file);
  // % = File name with extension (% omitted in batch file variables)
  // # = File name with extension and complete path specification
  // @ = File name without extension or path specification
  // $ = Path name of a file. Path names get extended with a backslash
  fs << "@echo off" << std::endl
     << "@set $K=%~1" << std::endl
     << "@set $J=%~2" << std::endl
     << "@set P=%~nx3" << std::endl
     << "@set #P=%~3" << std::endl
     << "@set @P=%~n3" << std::endl
     << "@set $P=%~dp3" << std::endl
     << "@set D=%~4" << std::endl
     << "@set L=%~nx5" << std::endl
     << "@set #L=%~dpnx5" << std::endl
     << "@set @L=%~n5" << std::endl
     << "@set $L=%~dp5" << std::endl
     << "@set H=%~nx6" << std::endl
     << "@set #H=%~dpnx6" << std::endl
     << "@set @H=%~n6" << std::endl
     << "@set $H=%~dp6" << std::endl;

  for (auto const& command : commands) {
    cmCustomCommandGenerator ccg(command, config, this->LocalGenerator);
    if (ccg.HasOnlyEmptyCommandLines()) {
      continue;
    }

    std::string workingDirectory = ccg.GetWorkingDirectory();
    if (!workingDirectory.empty()) {
      workingDirectory =
        cmSystemTools::ConvertToWindowsOutputPath(ccg.GetWorkingDirectory());
      fs << "@cd " << workingDirectory << std::endl;
    }

    const char* comment = ccg.GetComment();
    if (comment) {
      std::string escapedComment = EscapeCommandComment(comment);
      if (!escapedComment.empty()) {
        fs << "@echo " << escapedComment << std::endl;
      }
    }

    unsigned int numberOfCommands = ccg.GetNumberOfCommands();
    for (unsigned int i = 0; i < numberOfCommands; ++i) {
      std::string cmd = ccg.GetCommand(i);
      if (cmd.empty()) {
        continue;
      }
      ccg.AppendArguments(i, cmd);

      fs << cmd << std::endl;
      if (i < numberOfCommands - 1) {
        fs << "@if %errorlevel% neq 0 goto :cmEnd" << std::endl;
      }
    }
  }
  fs << ":cmEnd" << std::endl;
  fs << "@exit /b %errorlevel%" << std::endl;
  fs.Close();

  return "\"" + file + R"(" "$K" "$J" "#P" "$D" "#L" "#H")";
}

void cmUVisionTargetGenerator::AddConfigSourceWithDisplayNameAndGroup(
  std::string const& fileName, const cmGeneratorTarget::SourceKind sourceKind,
  std::string const& displayName, std::string const& config,
  std::string const& group)
{
  const auto configIndex = std::distance(
    Configs.begin(), std::find(Configs.begin(), Configs.end(), config));
  cmSourceFile* sourceFile = Makefile->GetOrCreateGeneratedSource(fileName);

  auto allConfigSource =
    std::find_if(GroupedSources[group].begin(), GroupedSources[group].end(),
                 [sourceFile](auto s) { return s.Source == sourceFile; });
  if (allConfigSource != GroupedSources[group].end()) {
    allConfigSource->Configs.push_back(configIndex);
  } else {
    cmGeneratorTarget::AllConfigSource acs;
    acs.Source = sourceFile;
    acs.Kind = sourceKind;
    acs.Configs.push_back(configIndex);
    if (!displayName.empty()) {
      acs.Source->SetProperty("UV_DISPLAY_NAME", displayName.c_str());
    }

    cmSourceGroup* sourceGroup = Makefile->GetOrCreateSourceGroup(group);
    sourceGroup->AssignSource(acs.Source);
    GroupedSources[group].push_back(std::move(acs));
  }
}

void cmUVisionTargetGenerator::GenerateSourceGroups(
  cmXMLEditorElement* targetElement, const size_t configIndex,
  cmUVisionGeneratorOptions* generatorOptions)
{
  // Generate source groups
  cmXMLEditorElement* groupsElement = targetElement->GetElement("Groups");
  if (!groupsElement) {
    cmSystemTools::Error("Groups XML element not found in project file");
    return;
  }
  groupsElement->GetChildren().clear();

  std::vector<std::string> sortedGroups;
  for (auto const& groupedSources : GroupedSources) {
    sortedGroups.push_back(groupedSources.first);
  }
  std::sort(sortedGroups.begin(), sortedGroups.end());

  for (auto const& sourceGroup : sortedGroups) {
    GenerateSourceGroup(groupsElement, sourceGroup, configIndex,
                        generatorOptions);
  }
}

std::string cmUVisionTargetGenerator::GetUVFileType(
  cmGeneratorTarget::AllConfigSource& sourceFile)
{
  std::string extension =
    cmSystemTools::LowerCase(sourceFile.Source->GetExtension());

  std::string fileType = "5"; // = header or text file
  if (Target->GetType() != cmStateEnums::UTILITY ||
      sourceFile.Kind == cmGeneratorTarget::SourceKindCustomCommand) {
    switch (sourceFile.Kind) {
      case cmGeneratorTarget::SourceKindObjectSource: {
        std::string language = sourceFile.Source->GetLanguage();
        if (language == "C") {
          fileType = "1"; // = C source
        } else if (language == "ASM") {
          fileType = "2"; // = ASM source
        } else if (language == "CXX") {
          fileType = "8"; // = C++ source
        }
        break;
      }
      case cmGeneratorTarget::SourceKindExtra: {
        if (extension == "lib") {
          fileType = "4"; // = Library file as source file
        }
        break;
      }
      case cmGeneratorTarget::SourceKindExternalObject: {
        if (extension == "lib") {
          fileType = "4"; // = Library file
        } else {
          fileType = "3"; // = Object file
        }
        break;
      }
      case cmGeneratorTarget::SourceKindCustomCommand: {
        fileType = "7"; // = Custom file
        break;
      }
      default:
        break;
    }
  }

  return fileType;
}

void cmUVisionTargetGenerator::GenerateSourceGroup(
  cmXMLEditorElement* groupsElement, std::string const& sourceGroup,
  size_t configIndex, cmUVisionGeneratorOptions* generatorOptions)
{
  auto sources = GroupedSources[sourceGroup];

  std::sort(sources.begin(), sources.end(), [](auto a, auto b) {
    return a.Source->GetLocation().GetName() <
      b.Source->GetLocation().GetName();
  });

  if (!sources.empty()) {
    cmXMLEditorElement* group = groupsElement->AddChild("Group");
    auto groupName = sourceGroup;
    if (groupName.empty()) {
      groupName = "Files";
    }
    group->AddChild("GroupName", groupName);

    cmXMLEditorElement* files = group->AddChild("Files");

    for (auto sourceFile : sources) {
      cmXMLEditorElement* file = files->AddChild("File");
      std::string fileName =
        sourceFile.Source->GetSafeProperty("UV_DISPLAY_NAME");
      if (fileName.empty()) {
        fileName = sourceFile.Source->GetLocation().GetName();
      }
      file->AddChild("FileName", fileName);
      file->AddChild("FileType", GetUVFileType(sourceFile));

      std::string filePath = cmSystemTools::RelativePath(
        this->BinaryDirectory, sourceFile.Source->ResolveFullPath());

      std::replace(filePath.begin(), filePath.end(), '/', '\\');
      file->AddChild("FilePath", filePath);

      if (std::find(sourceFile.Configs.begin(), sourceFile.Configs.end(),
                    configIndex) == sourceFile.Configs.end()) {
        file->SetTextContent("FileOption/CommonProperty/IncludeInBuild", "0",
                             true);
      }

      if (sourceFile.Kind == cmGeneratorTarget::SourceKindObjectSource) {
        auto fileOptions = generatorOptions->GetFileOptions(sourceFile.Source);

        if (!fileOptions.empty()) {
          std::string ads =
            (sourceFile.Source->GetLanguage() == "ASM") ? "Aads" : "Cads";
          std::string variousControls =
            "FileOption/FileArmAds/" + ads + "/VariousControls";
          // uVision expects these XML elements to be in exactly this order
          file->SetTextContent(variousControls + "/MiscControls", "", true);
          file->SetTextContent(variousControls + "/Define", "", true);
          file->SetTextContent(variousControls + "/Undefine", "", true);
          file->SetTextContent(variousControls + "/IncludePath", "", true);
        }

        for (auto const& fileOption : fileOptions) {
          if (!fileOption.Default) {
            bool create =
              !(fileOption.Options & cmUVFlagTableOption::OnlyIfPresent);
            file->SetTextContent(fileOption.Path, fileOption.Value, create);
          }
        }
      }

      auto* customCommand = sourceFile.Source->GetCustomCommand();
      if (customCommand) {
        std::string config = Configs[configIndex];
        const std::string commandFile = this->BinaryDirectory +
          "/CMakeFiles/CustomCommands/" + fileName +
          cmSystemTools::ComputeStringMD5(sourceFile.Source->GetFullPath()) +
          "_" + config + ".bat";
        std::vector<cmCustomCommand> commands;
        commands.push_back(*customCommand);
        const std::string command =
          GenerateCustomCommandBatchFile(commands, config, commandFile);
        file->SetTextContent("FileOption/CommonProperty/CustomArgument",
                             command);
        file->SetTextContent("FileOption/CommonProperty/StopOnExitCode", "1");
      }
    }
  }
}

void cmUVisionTargetGenerator::ProcessRTETarget(
  const cmGeneratorTarget* target, const std::vector<size_t>* configs,
  std::vector<cmUVRTEComponent>* components,
  std::list<cmUVRTEPackage>* packages)
{
  const char* componentFormat =
    "<vendor>{.<bundle>}::<class>:<group>{.<subgroup>}{:<variant>}";

  cmsys::RegularExpression componentReg(
    "([^.:]*)(\\.[^:]+)?"  // Vendor{.Bundle}
    "::([^:]*)"            // ::Class
    ":([^.:]*)(\\.[^:]+)?" // :Group{.SubGroup}
    "(:[^:]*)?");          // :Variant (starts with a letter)

  std::string componentString = target->GetSafeProperty("UV_COMPONENT");
  if (!componentReg.find(componentString)) {
    cmSystemTools::Error("Invalid component string: \"" + componentString +
                         "\"\n" + componentFormat);
  }

  // Component
  cmUVRTEComponent component;
  component.Vendor = componentReg.match(1);
  std::string bundle = componentReg.match(2);
  if (bundle.length() > 1) {
    component.Bundle = bundle.substr(1);
  }
  component.Class = componentReg.match(3);
  component.Group = componentReg.match(4);
  std::string subGroup = componentReg.match(5);
  if (subGroup.length() > 1) {
    component.SubGroup = subGroup.substr(1);
  }
  std::string variant = componentReg.match(6);
  if (variant.length() > 1) {
    component.Variant = variant.substr(1);
  }
  component.Version = target->GetSafeProperty("UV_COMPONENT_VERSION");

  // Package
  const char* packFormat = "<vendor>.<pack>";
  cmsys::RegularExpression packReg("([^.]*)"      // Vendor
                                   "\\.([^.]*)"); // .Class
  std::string packString = target->GetSafeProperty("UV_PACK");
  if (!packReg.find(packString)) {
    cmSystemTools::Error("Invalid package string: \"" + packString + "\"\n" +
                         packFormat);
  }

  cmUVRTEPackage package;
  package.Vendor = packReg.match(1);
  package.Name = packReg.match(2);
  package.Version = target->GetSafeProperty("UV_PACK_VERSION");

  auto packageIt =
    std::find_if(packages->begin(), packages->end(), [package](auto p) {
      return p.Name == package.Name && p.Vendor == package.Vendor &&
        p.Version == package.Version;
    });
  if (packageIt != packages->end()) {
    component.Package = &*packageIt;
  } else {
    packages->push_back(package);
    component.Package = &packages->back();
  }
  component.Package->Configs.insert(configs->begin(), configs->end());

  // Add component to collection
  auto componentIt =
    std::find_if(components->begin(), components->end(), [component](auto c) {
      return c.Vendor == component.Vendor && c.Bundle == component.Bundle &&
        c.Class == component.Class && c.Group == component.Group &&
        c.SubGroup == component.SubGroup && c.Version == component.Version &&
        c.Package->Vendor == component.Package->Vendor &&
        c.Package->Name == component.Package->Name;
    });
  if (componentIt != components->end()) {
    componentIt->Configs.insert(configs->begin(), configs->end());
  } else {
    component.Configs.insert(configs->begin(), configs->end());
    components->push_back(component);
  }
}

void cmUVisionTargetGenerator::GenerateComponents(
  cmXMLEditorElement* rteElement, std::vector<cmUVRTEComponent>& components)
{
  cmXMLEditorElement* componentsElement = rteElement->AddChild("components");
  for (auto const& component : components) {
    cmXMLEditorElement* componentElement =
      componentsElement->AddChild("component");
    componentElement->SetAttribute("Cclass", component.Class);
    if (!component.Bundle.empty()) {
      componentElement->SetAttribute("Cbundle", component.Bundle);
    }
    componentElement->SetAttribute("Cgroup", component.Group);
    if (!component.SubGroup.empty()) {
      componentElement->SetAttribute("Csub", component.SubGroup);
    }
    if (!component.Variant.empty()) {
      componentElement->SetAttribute("Cvariant", component.Variant);
    }
    componentElement->SetAttribute("Cvendor", component.Vendor);
    if (!component.Version.empty()) {
      componentElement->SetAttribute("Cversion", component.Version);
    }
    componentElement->SetAttribute("isTargetSpecific", "1");

    cmXMLEditorElement* packageElement = componentElement->AddChild("package");
    packageElement->SetAttribute("name", component.Package->Name);
    packageElement->SetAttribute("vendor", component.Package->Vendor);

    cmXMLEditorElement* targetInfosElement =
      componentElement->AddChild("targetInfos");
    for (size_t ci : component.Configs) {
      cmXMLEditorElement* targetInfoElement =
        targetInfosElement->AddChild("targetInfo");
      targetInfoElement->SetAttribute("name", Configs[ci]);
    }
  }
}

void cmUVisionTargetGenerator::GeneratePackages(
  cmXMLEditorElement* rteElement, std::list<cmUVRTEPackage>& packages)
{
  cmXMLEditorElement* packagesElement = rteElement->AddChild("packages");
  for (auto const& package : packages) {
    cmXMLEditorElement* packageElement = packagesElement->AddChild("package");
    packageElement->SetAttribute("name", package.Name);
    packageElement->SetAttribute("vendor", package.Vendor);
    if (!package.Version.empty()) {
      packageElement->SetAttribute("version", package.Version);
    }

    cmXMLEditorElement* targetInfosElement =
      packageElement->AddChild("targetInfos");

    for (size_t ci : package.Configs) {
      cmXMLEditorElement* targetInfoElement =
        targetInfosElement->AddChild("targetInfo");
      targetInfoElement->SetAttribute("name", Configs[ci]);
      if (!package.Version.empty()) {
        targetInfoElement->SetAttribute("versionMatchMode", "fixed");
      }
    }
  }
}

void cmUVisionTargetGenerator::GenerateRTEs(cmXMLEditorElement* projectElement)
{
  std::map<const cmGeneratorTarget*, std::vector<size_t>> rteTargets;
  for (size_t ci = 0; ci < Configs.size(); ++ci) {
    for (auto const& linkedLibrary :
         Target->GetLinkImplementationLibraries(Configs[ci])->Libraries) {
      if (linkedLibrary.Target != nullptr &&
          linkedLibrary.Target->GetType() == cmStateEnums::MODULE_LIBRARY &&
          linkedLibrary.Target->GetProperty("UV_COMPONENT")) {
        rteTargets[linkedLibrary.Target].push_back(ci);
      }
    }
  }

  std::vector<cmUVRTEComponent> components;
  std::list<cmUVRTEPackage> packages;
  if (!rteTargets.empty()) {
    for (auto const& [rteTarget, rteConfigs] : rteTargets) {
      ProcessRTETarget(rteTarget, &rteConfigs, &components, &packages);
    }
  }

  if (!components.empty()) {
    // Get or add the RTE element
    cmXMLEditorElement* rte = projectElement->GetElement("RTE", true);
    // Clear the children of the RTE element
    rte->GetChildren().clear();

    GenerateComponents(rte, components);
    GeneratePackages(rte, packages);
  }
}
