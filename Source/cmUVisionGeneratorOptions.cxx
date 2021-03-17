/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#include "cmUVisionGeneratorOptions.h"

#include <utility>

#include <cm3p/json/reader.h>

#include "cmsys/FStream.hxx"
#include "cmsys/RegularExpression.hxx"

#include "cmGeneratorExpression.h"
#include "cmGeneratorExpressionDAGChecker.h"
#include "cmMakefile.h"
#include "cmSourceFile.h"

static std::map<std::string, cmUVFlagTable> loadedFlagTables;

static cmUVFlagTable const& cmLoadFlagTable(std::string const& tableName);

cmUVisionGeneratorOptions::cmUVisionGeneratorOptions(cmGeneratorTarget* target,
                                                     std::string config)
  : LocalGenerator(
      dynamic_cast<cmLocalUVisionGenerator*>(target->GetLocalGenerator()))
  , Target(target)
  , Config(std::move(config))
{
  ComputeTargetOptions();
}

cmUVisionGeneratorOptions::~cmUVisionGeneratorOptions() = default;

static std::string cmLoadFlagTableString(Json::Value entry, const char* field)
{
  if (entry.isMember(field)) {
    const auto string = entry[field];
    if (string.isConvertibleTo(Json::ValueType::stringValue)) {
      return string.asString();
    }
  }
  return "";
}

static unsigned int cmLoadFlagTableOption(Json::Value entry, const char* field)
{
  unsigned int value = cmUVFlagTableOption::None;
  if (entry.isMember(field)) {
    auto options = entry[field];
    if (options.isArray()) {
      for (auto const& option : options) {
        if (!option.isConvertibleTo(Json::ValueType::stringValue)) {
          continue;
        }
        std::string o = option.asString();
        if (o == "ValueInNext") {
          value |= cmUVFlagTableOption::ValueInNext;
        } else if (o == "RegexInFlag") {
          value |= cmUVFlagTableOption::RegexInFlag;
        } else if (o == "RegexInValue") {
          value |= cmUVFlagTableOption::RegexInValue;
        } else if (o == "ValueIsPath") {
          value |= cmUVFlagTableOption::ValueIsPath;
        } else if (o == "ValueIsDirectory") {
          value |= cmUVFlagTableOption::ValueIsDirectory;
        } else if (o == "OnlyIfPresent") {
          value |= cmUVFlagTableOption::OnlyIfPresent;
        } else if (o == "ArmAds") {
          value |= cmUVFlagTableOption::ArmAds;
        } else if (o == "UvOpt") {
          value |= cmUVFlagTableOption::UvOpt;
        } else if (o == "Ignore") {
          value |= cmUVFlagTableOption::Ignore;
        }
      }
    }
  }
  return value;
}

static cmUVFlagTable const& cmLoadFlagTable(std::string const& tableName)
{
  std::string flagJsonPath = cmSystemTools::GetCMakeRoot() +
    "/Templates/UVision/FlagTables/" + tableName + ".json";

  Json::Reader reader;
  cmsys::ifstream stream;
  auto savedFlagIterator = loadedFlagTables.find(flagJsonPath);
  if (savedFlagIterator != loadedFlagTables.end()) {
    return savedFlagIterator->second;
  }

  cmUVFlagTable flagTable;
  stream.open(flagJsonPath.c_str(), std::ios_base::in);
  if (stream) {
    Json::Value flags;
    if (reader.parse(stream, flags, false) && flags.isArray()) {
      for (auto const& flag : flags) {
        cmUVFlagTableEntry flagTableEntry;
        flagTableEntry.Path = cmLoadFlagTableString(flag, "path");
        flagTableEntry.Default = cmLoadFlagTableString(flag, "default");
        flagTableEntry.Options = cmLoadFlagTableOption(flag, "options");

        if (flag.isMember("modifiers")) {
          for (auto const& mapping : flag["modifiers"]) {
            cmUVFlagTableModifier flagTableModifier;
            flagTableModifier.Flag = cmLoadFlagTableString(mapping, "flag");
            flagTableModifier.Property =
              cmLoadFlagTableString(mapping, "property");
            flagTableModifier.Value = cmLoadFlagTableString(mapping, "value");
            flagTableModifier.Set = cmLoadFlagTableString(mapping, "set");
            flagTableModifier.Options =
              cmLoadFlagTableOption(mapping, "options");
            if (!flagTableModifier.Flag.empty() &&
                !flagTableModifier.Value.empty())
              flagTableModifier.Options |= cmUVFlagTableOption::ValueInNext;
            flagTableEntry.Modifiers.push_back(flagTableModifier);
          }
        }
        flagTable.push_back(flagTableEntry);
      }
    }
  } else {
    cmSystemTools::Error("Flag table " + flagJsonPath + " not found!");
    cmSystemTools::SetFatalErrorOccured();
  }

  auto errors = reader.getFormatedErrorMessages();
  if (!errors.empty()) {
    cmSystemTools::Error("Error in flag table " + flagJsonPath + ":\n" +
                         errors);
    cmSystemTools::SetFatalErrorOccured();
  }

  loadedFlagTables[flagJsonPath] = flagTable;
  return loadedFlagTables[flagJsonPath];
}

void cmUVisionGeneratorOptions::ComputeTargetOptions()
{
  std::string config = Config;
  cmGeneratorTarget* target = Target;
  std::function<std::string(std::string)> getTargetProperty =
    [target, config](std::string const& p) -> std::string {
    cmProp value = target->GetProperty(p);
    if (!value) {
      value = target->Makefile->GetDefinition(p);
    }

    if (value) {
      cmGeneratorExpression ge(target->GetBacktrace());
      auto cge = ge.Parse(cmToCStr(value));
      return cge->Evaluate(target->GetLocalGenerator(), config);
    }
    return "";
  };

  std::vector<std::string> flagsCCxx;

  ProcessFlagTable(cmLoadFlagTable("Global"), nullptr, getTargetProperty,
                   TargetOptions);

  std::set<std::string> languages;
  Target->GetLanguages(languages, Config);
  for (auto const& language : languages) {
    std::string compilerId = Target->Makefile->GetSafeDefinition(
      "CMAKE_" + language + "_COMPILER_ID");
    if (!compilerId.empty()) {
      std::string flagsString;
      LocalGenerator->GetTargetCompileFlags(Target, Config, language,
                                            flagsString, "");
      std::vector<std::string> flags;
      cmSystemTools::ParseWindowsCommandLine(flagsString.c_str(), flags);

      std::string flagTableLanguage = language;
      if (flagTableLanguage == "C") {
        flagTableLanguage = "CXX";
      }

      cmUVFlagTable const& flagTable =
        cmLoadFlagTable(compilerId + "_" + flagTableLanguage);

      ProcessFlagTable(flagTable, &flags, getTargetProperty, TargetOptions);

      std::string ads = (language == "ASM") ? "Aads" : "Cads";
      std::string various = ads + "/VariousControls";

      if (!flags.empty()) {
        SetTargetOption(various + "/MiscControls", cmJoin(flags, " "),
                        cmUVFlagTableOption::ArmAds);
      }

      std::set<std::string> defines;
      LocalGenerator->GetTargetDefines(Target, Config, language, defines);
      if (!defines.empty()) {
        SetTargetOption(various + "/Define", cmJoin(defines, " "),
                        cmUVFlagTableOption::ArmAds);
      }

      std::vector<std::string> includePaths;
      LocalGenerator->GetIncludeDirectoriesImplicit(includePaths, Target,
                                                    language, Config);
      if (!includePaths.empty()) {
        SetTargetOption(various + "/IncludePath", cmJoin(includePaths, ";"),
                        cmUVFlagTableOption::ArmAds);
      }
    }
  }

  std::vector<std::string> linkerFlags;
  Target->GetLinkOptions(linkerFlags, Config,
                         Target->GetLinkerLanguage(Config));
  if (Target->GetType() == cmStateEnums::UTILITY) {
    // Workaround for utility targets to skip the actual linking
    linkerFlags.push_back("--version_number");
  }
  cmUVFlagTable const& linkerFlagTable = cmLoadFlagTable("ARMLink");
  ProcessFlagTable(linkerFlagTable, &linkerFlags, getTargetProperty,
                   TargetOptions);
  if (!linkerFlags.empty()) {
    SetTargetOption("LDads/Misc", cmJoin(linkerFlags, " "),
                    cmUVFlagTableOption::ArmAds);
  }
}

cmUVOptionValues cmUVisionGeneratorOptions::GetFileOptions(
  const cmSourceFile* sourceFile)
{
  cmUVOptionValues fileOptions;

  const std::function<std::string(std::string)> getSourceFileProperty =
    [sourceFile](std::string const& p) -> std::string {
    return sourceFile->GetSafeProperty(p);
  };

  const std::string language = sourceFile->GetLanguage();

  const std::string ads = (language == "ASM") ? "Aads" : "Cads";
  const std::string various =
    "FileOption/FileArmAds/" + ads + "/VariousControls";

  const auto defines = sourceFile->GetCompileDefinitions();
  if (!defines.empty()) {
    SetOption(&fileOptions, various + "/Define", cmJoin(defines, " "));
  }

  const auto includes = sourceFile->GetIncludeDirectories();
  if (!includes.empty()) {
    SetOption(&fileOptions, various + "/IncludePath", cmJoin(includes, ";"));
  }

  std::vector<std::string> flags;
  for (auto const& compileOption : sourceFile->GetCompileOptions()) {
    cmSystemTools::ParseWindowsCommandLine(compileOption.Value.c_str(), flags);
  }
  if (!flags.empty()) {
    const std::string compilerId = Target->Makefile->GetSafeDefinition(
      "CMAKE_" + language + "_COMPILER_ID");
    cmUVFlagTable const& flagTable =
      cmLoadFlagTable(compilerId + "_" + language);
    ProcessFlagTable(flagTable, &flags, getSourceFileProperty, fileOptions);
  }

  if (!flags.empty()) {
    SetOption(&fileOptions, various + "/MiscControls", cmJoin(flags, " "));
  }

  return fileOptions;
}

bool cmUVisionGeneratorOptions::ProcessFlagTableModifier(
  cmUVFlagTableEntry const& flagEntry,
  cmUVFlagTableModifier const& flagModifier, std::string const& flag,
  std::string const& value, cmUVOptionValues& targetOptions)
{
  std::string newValue;

  cmsys::RegularExpression regex;

  bool flagIsMatch = false;
  if (flagModifier.Options & cmUVFlagTableOption::RegexInFlag) {
    regex.compile(flagModifier.Flag);
    if (regex.find(flag)) {
      flagIsMatch = true;
    }
  } else if (flagModifier.Flag == flag || flagModifier.Property == flag) {
    flagIsMatch = true;
  }
  // Not our flag
  if (!flagIsMatch) {
    return false;
  }

  if (flagEntry.Options & cmUVFlagTableOption::Ignore) {
    // Ignore flag
    return true;
  }

  if (flagModifier.Options & cmUVFlagTableOption::RegexInValue) {
    regex.compile(flagModifier.Value);
    if (!regex.find(value)) {
      return false;
    }
  }

  newValue = flagModifier.Set;

  if (regex.is_valid()) {
    // Handle regex flags/values
    if (newValue.empty()) {
      newValue = regex.match(1);
    } else {
      cmsys::RegularExpression replaceRegex("\\$([0-9]+)");
      while (replaceRegex.find(newValue)) {
        long index;
        cmStrToLong(replaceRegex.match(1), &index);
        newValue.replace(replaceRegex.start(), replaceRegex.end(),
                         regex.match(index));
      }
    }
  } else if (newValue.empty() && !value.empty()) {
    // No regex, but the value was provided in the next flag
    newValue = value;
  }

  if ((flagEntry.Options & cmUVFlagTableOption::ValueIsPath) ==
      cmUVFlagTableOption::ValueIsPath) {
    newValue = cmSystemTools::ConvertToWindowsOutputPath(newValue);
    if ((flagEntry.Options & cmUVFlagTableOption::ValueIsDirectory) ==
        cmUVFlagTableOption::ValueIsDirectory) {
      if (newValue.empty() || newValue[newValue.size() - 1] != '\\') {
        newValue += '\\';
      }
    }
  }

  SetOption(&targetOptions, flagEntry.Path, newValue, flagEntry.Options);

  return true;
}

void cmUVisionGeneratorOptions::ProcessFlagTable(
  cmUVFlagTable const& flagTable, std::vector<std::string>* flags,
  std::function<std::string(std::string)> const& getProperty,
  cmUVOptionValues& optionValues)
{
  std::set<size_t> handledFlags;
  for (auto const& flagTableEntry : flagTable) {

    // Handle default values
    if (!flagTableEntry.Default.empty()) {
      SetOption(&optionValues, flagTableEntry.Path, flagTableEntry.Default,
                flagTableEntry.Options, true);
    }

    for (auto const& modifier : flagTableEntry.Modifiers) {
      if (!modifier.Property.empty()) {
        // The modifier handles a property, not a flag
        std::string value = getProperty(modifier.Property);
        if (!value.empty()) {
          if (cmIsOn(value)) {
            value = "1";
          } else if (cmIsOff(value)) {
            value = "0";
          }
          ProcessFlagTableModifier(flagTableEntry, modifier, modifier.Property,
                                   value, optionValues);
        }
      } else if (flags) {
        // The modifier handles a flag
        for (size_t fi = 0; fi < flags->size(); ++fi) {
          std::string currentFlag = flags->at(fi);
          std::string nextFlag;
          if ((modifier.Options & cmUVFlagTableOption::ValueInNext) ==
              cmUVFlagTableOption::ValueInNext) {
            // The modifier expects a value in the next flag
            if (fi == flags->size() - 1) {
              break;
            }
            nextFlag = flags->at(fi + 1);
          }

          if (ProcessFlagTableModifier(flagTableEntry, modifier, currentFlag,
                                       nextFlag, optionValues)) {
            handledFlags.insert(fi);
            if ((modifier.Options & cmUVFlagTableOption::ValueInNext) ==
                cmUVFlagTableOption::ValueInNext) {
              handledFlags.insert(fi + 1);
            }
          }
        }
      }
    }
  }

  if (flags && !flags->empty()) {
    for (int fi = flags->size() - 1; fi >= 0; fi--) {
      if (handledFlags.find(fi) != handledFlags.end()) {
        flags->erase(flags->begin() + fi);
      }
    }
  }
}

void cmUVisionGeneratorOptions::SetOption(cmUVOptionValues* optionValues,
                                          std::string path,
                                          std::string const& value,
                                          int options, bool isDefault)
{
  auto optionValue = std::find_if(optionValues->begin(), optionValues->end(),
                                  [path, options](auto optionValue) {
                                    return optionValue.Path == path &&
                                      optionValue.Options == options;
                                  });
  if (optionValue == optionValues->end() || !isDefault) {
    if (optionValue != optionValues->end()) {
      optionValues->erase(optionValue);
    }

    cmUVOptionValue newOptionValue;
    newOptionValue.Path = path;
    newOptionValue.Options = options;
    newOptionValue.Default = isDefault;
    newOptionValue.Value = value;
    optionValues->push_back(newOptionValue);
  }
}
