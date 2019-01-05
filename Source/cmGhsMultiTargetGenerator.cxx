/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#include "cmGhsMultiTargetGenerator.h"

#include "cmComputeLinkInformation.h"
#include "cmGeneratedFileStream.h"
#include "cmGeneratorTarget.h"
#include "cmGlobalGhsMultiGenerator.h"
#include "cmLinkLineComputer.h"
#include "cmLocalGhsMultiGenerator.h"
#include "cmMakefile.h"
#include "cmSourceFile.h"
#include "cmTarget.h"
#include <assert.h>

std::string const cmGhsMultiTargetGenerator::DDOption("-dynamic");

cmGhsMultiTargetGenerator::cmGhsMultiTargetGenerator(cmGeneratorTarget* target)
  : GeneratorTarget(target)
  , LocalGenerator(
      static_cast<cmLocalGhsMultiGenerator*>(target->GetLocalGenerator()))
  , Makefile(target->Target->GetMakefile())
  , TargetGroup(DetermineIfTargetGroup(target))
  , DynamicDownload(false)
  , Name(target->GetName())
{
  // Store the configuration name that is being used
  if (const char* config = this->Makefile->GetDefinition("CMAKE_BUILD_TYPE")) {
    // Use the build type given by the user.
    this->ConfigName = config;
  } else {
    // No configuration type given.
    this->ConfigName.clear();
  }
}

cmGhsMultiTargetGenerator::~cmGhsMultiTargetGenerator()
{
}

void cmGhsMultiTargetGenerator::Generate()
{
  // Determine type of target for this project
  switch (this->GeneratorTarget->GetType()) {
    case cmStateEnums::EXECUTABLE: {
      // Get the name of the executable to generate.
      std::string targetName;
      std::string targetNameImport;
      std::string targetNamePDB;
      this->GeneratorTarget->GetExecutableNames(
        targetName, this->TargetNameReal, targetNameImport, targetNamePDB,
        this->ConfigName);
      if (cmGhsMultiTargetGenerator::DetermineIfTargetGroup(
            this->GeneratorTarget)) {
        this->TagType = GhsMultiGpj::INTERGRITY_APPLICATION;
      } else {
        this->TagType = GhsMultiGpj::PROGRAM;
      }
      break;
    }
    case cmStateEnums::STATIC_LIBRARY: {
      std::string targetName;
      std::string targetNameSO;
      std::string targetNameImport;
      std::string targetNamePDB;
      this->GeneratorTarget->GetLibraryNames(
        targetName, targetNameSO, this->TargetNameReal, targetNameImport,
        targetNamePDB, this->ConfigName);
      this->TagType = GhsMultiGpj::LIBRARY;
      break;
    }
    case cmStateEnums::SHARED_LIBRARY: {
      std::string msg = "add_library(<name> SHARED ...) not supported: ";
      msg += this->Name;
      cmSystemTools::Message(msg.c_str());
      return;
    }
    case cmStateEnums::OBJECT_LIBRARY: {
      std::string msg = "add_library(<name> OBJECT ...) not supported: ";
      msg += this->Name;
      cmSystemTools::Message(msg.c_str());
      return;
    }
    case cmStateEnums::MODULE_LIBRARY: {
      std::string msg = "add_library(<name> MODULE ...) not supported: ";
      msg += this->Name;
      cmSystemTools::Message(msg.c_str());
      return;
    }
    case cmStateEnums::UTILITY: {
      std::string msg = "add_custom_target(<name> ...) not supported: ";
      msg += this->Name;
      cmSystemTools::Message(msg.c_str());
      return;
    }
    default:
      return;
  }

  // Tell the global generator the name of the project file
  this->GeneratorTarget->Target->SetProperty("GENERATOR_FILE_NAME",
                                             this->Name.c_str());
  this->GeneratorTarget->Target->SetProperty(
    "GENERATOR_FILE_NAME_EXT", GhsMultiGpj::GetGpjTag(this->TagType));

  this->GenerateTarget();
}

void cmGhsMultiTargetGenerator::GenerateTarget()
{
  // Skip if empty or not included in build
  if (!this->GetSources().empty() && this->IncludeThisTarget()) {

    // Open the filestream in copy-if-different mode.
    std::string fname = this->LocalGenerator->GetCurrentBinaryDirectory();
    fname += "/";
    fname += this->Name;
    fname += cmGlobalGhsMultiGenerator::FILE_EXTENSION;
    cmGeneratedFileStream fout(fname.c_str());
    fout.SetCopyIfDifferent(true);

    this->GetGlobalGenerator()->WriteFileHeader(fout);
    GhsMultiGpj::WriteGpjTag(this->TagType, fout);

    std::string config = this->Makefile->GetSafeDefinition("CMAKE_BUILD_TYPE");
    if (0 == config.length()) {
      config = "RELEASE";
    }
    const std::string language(
      this->GeneratorTarget->GetLinkerLanguage(config));
    config = cmSystemTools::UpperCase(config);
    this->DynamicDownload = this->DetermineIfDynamicDownload(config, language);
    if (this->DynamicDownload) {
      fout << "#component integrity_dynamic_download" << std::endl;
    }
    bool const notKernel = this->IsNotKernel(config, language);
    this->WriteTargetSpecifics(fout, config, notKernel);
    this->SetCompilerFlags(config, language, notKernel);
    this->WriteCompilerFlags(fout, config, language);
    this->WriteCompilerDefinitions(fout, config, language);
    this->WriteIncludes(fout, config, language);
    this->WriteTargetLinkLine(fout, config);
    this->WriteCustomCommands(fout);
    this->WriteSources(fout);

    fout.Close();
  }
}

bool cmGhsMultiTargetGenerator::IncludeThisTarget()
{
  bool output = true;
  char const* excludeFromAll =
    this->GeneratorTarget->GetProperty("EXCLUDE_FROM_ALL");
  if (NULL != excludeFromAll && '1' == excludeFromAll[0] &&
      '\0' == excludeFromAll[1]) {
    output = false;
  }
  return output;
}

std::vector<cmSourceFile*> cmGhsMultiTargetGenerator::GetSources() const
{
  std::vector<cmSourceFile*> output;
  std::string config = this->Makefile->GetSafeDefinition("CMAKE_BUILD_TYPE");
  this->GeneratorTarget->GetSourceFiles(output, config);
  return output;
}

cmGlobalGhsMultiGenerator* cmGhsMultiTargetGenerator::GetGlobalGenerator()
  const
{
  return static_cast<cmGlobalGhsMultiGenerator*>(
    this->LocalGenerator->GetGlobalGenerator());
}

void cmGhsMultiTargetGenerator::WriteTargetSpecifics(std::ostream& fout,
                                                     const std::string& config,
                                                     bool const notKernel)
{
  std::string outpath;
  std::string rootpath = this->LocalGenerator->GetCurrentBinaryDirectory();

  // set target binary file destination
  outpath = this->GeneratorTarget->GetDirectory(config);
  outpath = this->LocalGenerator->ConvertToRelativePath(rootpath, outpath);
  fout << "    :binDirRelative=\"" << outpath << "\"" << std::endl;
  fout << "    -o \"" << this->TargetNameReal << "\"" << std::endl;

  // set target object file destination
  outpath = this->LocalGenerator->GetTargetDirectory(this->GeneratorTarget);
  fout << "    :outputDirRelative=\"" << outpath << "\"" << std::endl;
}

void cmGhsMultiTargetGenerator::SetCompilerFlags(std::string const& config,
                                                 const std::string& language,
                                                 bool const notKernel)
{
  std::map<std::string, std::string>::iterator i =
    this->FlagsByLanguage.find(language);
  if (i == this->FlagsByLanguage.end()) {
    std::string flags;
    const char* lang = language.c_str();

    if (notKernel) {
      this->LocalGenerator->AddLanguageFlags(flags, this->GeneratorTarget,
                                             lang, config);
    } else {
      this->LocalGenerator->AddLanguageFlags(flags, this->GeneratorTarget,
                                             lang + std::string("_GHS_KERNEL"),
                                             config);
    }
    this->LocalGenerator->AddCMP0018Flags(flags, this->GeneratorTarget, lang,
                                          config);
    this->LocalGenerator->AddVisibilityPresetFlags(
      flags, this->GeneratorTarget, lang);

    // Append old-style preprocessor definition flags.
    if (this->Makefile->GetDefineFlags() != " ") {
      this->LocalGenerator->AppendFlags(flags,
                                        this->Makefile->GetDefineFlags());
    }

    // Add target-specific flags.
    this->LocalGenerator->AddCompileOptions(flags, this->GeneratorTarget, lang,
                                            config);

    std::map<std::string, std::string>::value_type entry(language, flags);
    i = this->FlagsByLanguage.insert(entry).first;
  }
}

std::string cmGhsMultiTargetGenerator::GetDefines(const std::string& language,
                                                  std::string const& config)
{
  std::map<std::string, std::string>::iterator i =
    this->DefinesByLanguage.find(language);
  if (i == this->DefinesByLanguage.end()) {
    std::set<std::string> defines;
    const char* lang = language.c_str();
    // Add preprocessor definitions for this target and configuration.
    this->LocalGenerator->GetTargetDefines(this->GeneratorTarget, config,
                                           language, defines);

    std::string definesString;
    this->LocalGenerator->JoinDefines(defines, definesString, lang);

    std::map<std::string, std::string>::value_type entry(language,
                                                         definesString);
    i = this->DefinesByLanguage.insert(entry).first;
  }
  return i->second;
}

void cmGhsMultiTargetGenerator::WriteCompilerFlags(std::ostream& fout,
                                                   std::string const&,
                                                   const std::string& language)
{
  std::map<std::string, std::string>::iterator flagsByLangI =
    this->FlagsByLanguage.find(language);
  if (flagsByLangI != this->FlagsByLanguage.end()) {
    if (!flagsByLangI->second.empty()) {
      fout << "    " << flagsByLangI->second << std::endl;
    }
  }
}

void cmGhsMultiTargetGenerator::WriteCompilerDefinitions(
  std::ostream& fout, const std::string& config, const std::string& language)
{
  std::vector<std::string> compileDefinitions;
  this->GeneratorTarget->GetCompileDefinitions(compileDefinitions, config,
                                               language);
  for (std::vector<std::string>::const_iterator cdI =
         compileDefinitions.begin();
       cdI != compileDefinitions.end(); ++cdI) {
    fout << "    -D" << (*cdI) << std::endl;
  }
}

void cmGhsMultiTargetGenerator::WriteIncludes(std::ostream& fout,
                                              const std::string& config,
                                              const std::string& language)
{
  std::vector<std::string> includes;
  this->LocalGenerator->GetIncludeDirectories(includes, this->GeneratorTarget,
                                              language, config);

  for (std::vector<std::string>::const_iterator includes_i = includes.begin();
       includes_i != includes.end(); ++includes_i) {
    fout << "    -I\"" << *includes_i << "\"" << std::endl;
  }
}

void cmGhsMultiTargetGenerator::WriteTargetLinkLine(std::ostream& fout,
                                                    std::string const& config)
{
  if (this->TagType == GhsMultiGpj::INTERGRITY_APPLICATION) {
    return;
  }

  std::string linkLibraries;
  std::string flags;
  std::string linkFlags;
  std::string frameworkPath;
  std::string linkPath;

  std::unique_ptr<cmLinkLineComputer> linkLineComputer(
    this->GetGlobalGenerator()->CreateLinkLineComputer(
      this->LocalGenerator,
      this->LocalGenerator->GetStateSnapshot().GetDirectory()));

  this->LocalGenerator->GetTargetFlags(
    linkLineComputer.get(), config, linkLibraries, flags, linkFlags,
    frameworkPath, linkPath, this->GeneratorTarget);

  // write out link options
  std::vector<std::string> lopts =
    cmSystemTools::ParseArguments(linkFlags.c_str());
  for (auto& l : lopts) {
    fout << "    " << l << std::endl;
  }

  // write out link search paths
  // must be quoted for paths that contain spaces
  std::vector<std::string> lpath =
    cmSystemTools::ParseArguments(linkPath.c_str());
  for (auto& l : lpath) {
    fout << "    -L\"" << l << "\"" << std::endl;
  }

  // write out link libs
  // must be quoted for filepaths that contains spaces
  std::string cbd = this->LocalGenerator->GetCurrentBinaryDirectory();

  std::vector<std::string> llibs =
    cmSystemTools::ParseArguments(linkLibraries.c_str());
  for (auto& l : llibs) {
    if (l.compare(0, 2, "-l") == 0) {
      fout << "    \"" << l << "\"" << std::endl;
    } else {
      std::string rl = cmSystemTools::CollapseCombinedPath(cbd, l);
      fout << "    -l\"" << rl << "\"" << std::endl;
    }
  }
}

void cmGhsMultiTargetGenerator::WriteCustomCommands(std::ostream& fout)
{
  WriteCustomCommandsHelper(fout, this->GeneratorTarget->GetPreBuildCommands(),
                            cmTarget::PRE_BUILD);
  WriteCustomCommandsHelper(
    fout, this->GeneratorTarget->GetPostBuildCommands(), cmTarget::POST_BUILD);
}

void cmGhsMultiTargetGenerator::WriteCustomCommandsHelper(
  std::ostream& fout, std::vector<cmCustomCommand> const& commandsSet,
  cmTarget::CustomCommandType const commandType)
{
  for (std::vector<cmCustomCommand>::const_iterator commandsSetI =
         commandsSet.begin();
       commandsSetI != commandsSet.end(); ++commandsSetI) {
    cmCustomCommandLines const& commands = commandsSetI->GetCommandLines();
    for (cmCustomCommandLines::const_iterator commandI = commands.begin();
         commandI != commands.end(); ++commandI) {
      switch (commandType) {
        case cmTarget::PRE_BUILD:
          fout << "    :preexecShellSafe=";
          break;
        case cmTarget::POST_BUILD:
          fout << "    :postexecShellSafe=";
          break;
        default:
          assert("Only pre and post are supported");
      }
      cmCustomCommandLine const& command = *commandI;
      for (cmCustomCommandLine::const_iterator commandLineI = command.begin();
           commandLineI != command.end(); ++commandLineI) {
        std::string subCommandE =
          this->LocalGenerator->EscapeForShell(*commandLineI, true);
        if (!command.empty()) {
          fout << (command.begin() == commandLineI ? "'" : " ");
          // Need to double escape backslashes
          cmSystemTools::ReplaceString(subCommandE, "\\", "\\\\");
        }
        fout << subCommandE;
      }
      if (!command.empty()) {
        fout << "'" << std::endl;
      }
    }
  }
}

std::map<const cmSourceFile*, std::string>
cmGhsMultiTargetGenerator::GetObjectNames(
  std::vector<cmSourceFile*>* const objectSources,
  cmLocalGhsMultiGenerator* const localGhsMultiGenerator,
  cmGeneratorTarget* const generatorTarget)
{
  std::map<std::string, std::vector<cmSourceFile*>> filenameToSource;
  std::map<cmSourceFile*, std::string> sourceToFilename;
  for (std::vector<cmSourceFile*>::const_iterator sf = objectSources->begin();
       sf != objectSources->end(); ++sf) {
    const std::string filename =
      cmSystemTools::GetFilenameName((*sf)->GetFullPath());
    const std::string lower_filename = cmSystemTools::LowerCase(filename);
    filenameToSource[lower_filename].push_back(*sf);
    sourceToFilename[*sf] = lower_filename;
  }

  std::vector<cmSourceFile*> duplicateSources;
  for (std::map<std::string, std::vector<cmSourceFile*>>::const_iterator
         msvSourceI = filenameToSource.begin();
       msvSourceI != filenameToSource.end(); ++msvSourceI) {
    if (msvSourceI->second.size() > 1) {
      duplicateSources.insert(duplicateSources.end(),
                              msvSourceI->second.begin(),
                              msvSourceI->second.end());
    }
  }

  std::map<const cmSourceFile*, std::string> objectNamesCorrected;

  for (std::vector<cmSourceFile*>::const_iterator sf =
         duplicateSources.begin();
       sf != duplicateSources.end(); ++sf) {
    std::string const longestObjectDirectory(
      cmGhsMultiTargetGenerator::ComputeLongestObjectDirectory(
        localGhsMultiGenerator, generatorTarget, *sf));
    std::string objFilenameName =
      localGhsMultiGenerator->GetObjectFileNameWithoutTarget(
        **sf, longestObjectDirectory);
    cmsys::SystemTools::ReplaceString(objFilenameName, "/", "_");
    objectNamesCorrected[*sf] = objFilenameName;
  }

  return objectNamesCorrected;
}

void cmGhsMultiTargetGenerator::WriteSources(std::ostream& fout_proj)
{
  std::vector<cmSourceFile*> sources = this->GetSources();
  std::vector<cmSourceGroup> sourceGroups = this->Makefile->GetSourceGroups();

  /* collect which files are in which groups */
  std::map<cmSourceGroup*, std::vector<cmSourceFile*>> groupFiles;
  std::map<std::string, cmSourceGroup*> groupNames;

  for (auto& sf : sources) {
    cmSourceGroup* sourceGroup =
      this->Makefile->FindSourceGroup(sf->GetFullPath(), sourceGroups);
    groupFiles[sourceGroup].push_back(sf);
  }

  for (auto& g : groupFiles) {
    groupNames[g.first->GetFullName()] = g.first;
  }

  /* sort the group names and files within group names */
  const std::vector<std::pair<std::string, std::string>> standardGroups = {
    { "Header Files", CM_HEADER_REGEX },
    { "Source Files", CM_SOURCE_REGEX },
    { "CMake Rules", "\\.rule$" },
    { "Object Files", "\\.(lo|o|obj)$" },
    { "Object Libraries", "^MATCH_NO_SOURCES$" },
    { "Resources", CM_RESOURCE_REGEX },
    { "", "^.*$" }
  };

  /* 1) order standard groups first */
  std::vector<cmSourceGroup*> groupOrderVec;
  for (auto& gn : standardGroups) {
    cmSourceGroup* sg = groupNames[gn.first];
    if (sg) {
      groupOrderVec.push_back(sg);
    }
    groupNames.erase(gn.first);
  }

  /* 2) sort the files within each standard group */
  for (auto& g : groupOrderVec) {
    std::sort(groupFiles[g].begin(), groupFiles[g].end(),
              [](cmSourceFile* l, cmSourceFile* r) {
                return l->GetFullPath() < r->GetFullPath();
              });
  }

  /* 3) generate the regex to partition user source group files */
  std::vector<std::pair<std::string, cmsys::RegularExpression>> groupRegex;
  for (auto& gn : standardGroups) {
    groupRegex.push_back(
      std::make_pair(gn.first, cmsys::RegularExpression(gn.second)));
  }

  /* 4) process each user-defined group */
  for (auto& gn : groupNames) {
    /* 4a) add this group to the in-order list */
    groupOrderVec.push_back(gn.second);

    /* 4b) determine which partition each file belongs in */
    std::map<std::string, std::vector<cmSourceFile*>> temp;
    for (auto& sf : groupFiles[gn.second]) {
      // Now search for a group whose regex matches the file.
      for (auto& regex : groupRegex) {
        if (regex.second.find(sf->GetFullPath())) {
          temp[regex.first].push_back(sf);
          break;
        }
      }
    }

    /* 4c) sort each list of files */
    for (auto& vf : temp) {
      std::sort(vf.second.begin(), vf.second.end(),
                [](cmSourceFile* l, cmSourceFile* r) {
                  return l->GetFullPath() < r->GetFullPath();
                });
    }

    /* 4d) replace the unsorted list with the sorted version */
    auto& gfl = groupFiles[gn.second];
    gfl.clear();
    for (auto& sg : standardGroups) {
      gfl.insert(gfl.end(), temp[sg.first].begin(), temp[sg.first].end());
    }
  }

  /* get all the object names for these sources */
  std::map<const cmSourceFile*, std::string> objectNames =
    cmGhsMultiTargetGenerator::GetObjectNames(&sources, this->LocalGenerator,
                                              this->GeneratorTarget);

  /* write files into the proper group
   * -- standard groups go into main project
   *    unless FOLDER property is set.
   */
  std::vector<cmGeneratedFileStream*> gfiles;

  for (cmSourceGroup* sg : groupOrderVec) {
    std::ostream* fout;
    bool useFolders =
      cmSystemTools::IsOn(this->GeneratorTarget->GetProperty("FOLDER"));
    if ((useFolders && sg->GetFullName().empty()) ||
        (!useFolders &&
         groupNames.find(sg->GetFullName()) == groupNames.end())) {
      fout = &fout_proj;
    } else {
      // Open the filestream in copy-if-different mode.
      std::string gname = this->Name;
      gname += "_";
      gname += sg->GetFullName();
      cmsys::SystemTools::ReplaceString(gname, "\\", "_");
      std::string fname = this->LocalGenerator->GetCurrentBinaryDirectory();
      fname += "/";
      fname += gname;
      fname += cmGlobalGhsMultiGenerator::FILE_EXTENSION;
      cmGeneratedFileStream* f = new cmGeneratedFileStream(fname.c_str());
      f->SetCopyIfDifferent(true);
      gfiles.push_back(f);
      fout = f;
      this->GetGlobalGenerator()->WriteFileHeader(*f);
      GhsMultiGpj::WriteGpjTag(GhsMultiGpj::SUBPROJECT, *f);
      fout_proj << gname << cmGlobalGhsMultiGenerator::FILE_EXTENSION << " ";
      GhsMultiGpj::WriteGpjTag(GhsMultiGpj::SUBPROJECT, fout_proj);
    }

    /* output rule for each source file */
    for (const cmSourceFile* si : groupFiles[sg]) {

      // Convert filename to native system
      // WORKAROUND: GHS MULTI 6.1.4 and 6.1.6 are known to need backslash on
      // windows when opening some files from the search window.
      std::string fname(si->GetFullPath());
      cmSystemTools::ConvertToOutputSlashes(fname);
      *fout << fname << std::endl;

      if ("ld" != si->GetExtension() && "int" != si->GetExtension() &&
          "bsp" != si->GetExtension()) {
        this->WriteObjectLangOverride(*fout, si);
        if (objectNames.end() != objectNames.find(si)) {
          *fout << "    -o \"" << objectNames.find(si)->second << "\""
                << std::endl;
        }
      }
    }
  }

  for (cmGeneratedFileStream* f : gfiles) {
    f->Close();
  }
}

void cmGhsMultiTargetGenerator::WriteObjectLangOverride(
  std::ostream& fout, const cmSourceFile* sourceFile)
{
  const char* rawLangProp = sourceFile->GetProperty("LANGUAGE");
  if (NULL != rawLangProp) {
    std::string sourceLangProp(rawLangProp);
    std::string extension(sourceFile->GetExtension());
    if ("CXX" == sourceLangProp && ("c" == extension || "C" == extension)) {
      fout << "    -dotciscxx" << std::endl;
    }
  }
}

std::string cmGhsMultiTargetGenerator::ComputeLongestObjectDirectory(
  cmLocalGhsMultiGenerator const* localGhsMultiGenerator,
  cmGeneratorTarget* const generatorTarget, cmSourceFile* const sourceFile)
{
  std::string dir_max;
  dir_max +=
    localGhsMultiGenerator->GetMakefile()->GetCurrentBinaryDirectory();
  dir_max += "/";
  dir_max += generatorTarget->Target->GetName();
  dir_max += "/";
  std::vector<cmSourceGroup> sourceGroups(
    localGhsMultiGenerator->GetMakefile()->GetSourceGroups());
  std::string const& sourceFullPath = sourceFile->GetFullPath();
  cmSourceGroup* sourceGroup =
    localGhsMultiGenerator->GetMakefile()->FindSourceGroup(sourceFullPath,
                                                           sourceGroups);
  std::string const& sgPath = sourceGroup->GetFullName();
  dir_max += sgPath;
  dir_max += "/Objs/libs/";
  dir_max += generatorTarget->Target->GetName();
  dir_max += "/";
  return dir_max;
}

bool cmGhsMultiTargetGenerator::IsNotKernel(std::string const& config,
                                            const std::string& language)
{
  bool output;
  std::vector<std::string> options;
  this->GeneratorTarget->GetCompileOptions(options, config, language);
  output =
    options.end() == std::find(options.begin(), options.end(), "-kernel");
  return output;
}

bool cmGhsMultiTargetGenerator::DetermineIfTargetGroup(
  const cmGeneratorTarget* target)
{
  bool output = false;
  std::vector<cmSourceFile*> sources;
  std::string config =
    target->Target->GetMakefile()->GetSafeDefinition("CMAKE_BUILD_TYPE");
  target->GetSourceFiles(sources, config);
  for (std::vector<cmSourceFile*>::const_iterator sources_i = sources.begin();
       sources.end() != sources_i; ++sources_i) {
    if ("int" == (*sources_i)->GetExtension()) {
      output = true;
    }
  }
  return output;
}

bool cmGhsMultiTargetGenerator::DetermineIfDynamicDownload(
  std::string const& config, const std::string& language)
{
  std::vector<std::string> options;
  bool output = false;
  this->GeneratorTarget->GetCompileOptions(options, config, language);
  for (std::vector<std::string>::const_iterator options_i = options.begin();
       options_i != options.end(); ++options_i) {
    std::string option = *options_i;
    if (this->DDOption == option) {
      output = true;
    }
  }
  return output;
}
