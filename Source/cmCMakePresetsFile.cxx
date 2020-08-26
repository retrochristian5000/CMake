/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#include "cmCMakePresetsFile.h"

#include <cstdlib>
#include <utility>

#ifndef CMAKE_BOOTSTRAP
#  include <functional>

#  include <cmext/string_view>

#  include <cm3p/json/reader.h>
#  include <cm3p/json/value.h>

#  include "cmsys/FStream.hxx"

#  include "cmJSONHelpers.h"
#endif

#include "cmStringAlgorithms.h"
#include "cmSystemTools.h"
#include "cmVersion.h"

#ifndef CMAKE_BOOTSTRAP
namespace {
constexpr int MIN_MAJOR_VERSION = 1;
constexpr int MAX_MAJOR_VERSION = 1;

constexpr std::pair<int, int> MIN_MAX_MINOR_VERSIONS[] = {
  { 0, 0 },
};

struct FileVersion
{
  int Major = 1;
  int Minor = 0;
};

struct CMakeVersion
{
  unsigned int Major = 0;
  unsigned int Minor = 0;
  unsigned int Patch = 0;
};

struct RootPresets
{
  CMakeVersion CMakeMinimumRequired;
  std::vector<cmCMakePresetsFile::UnexpandedPreset> Presets;
};

using ReadFileResult = cmCMakePresetsFile::ReadFileResult;
using Variable = cmCMakePresetsFile::Variable;
using UnexpandedPreset = cmCMakePresetsFile::UnexpandedPreset;

cmJSONHelper<std::nullptr_t, ReadFileResult> VendorHelper(ReadFileResult error)
{
  return [error](std::nullptr_t& /*out*/,
                 const Json::Value* value) -> ReadFileResult {
    if (!value) {
      return ReadFileResult::READ_OK;
    }

    if (!value->isObject()) {
      return error;
    }

    return ReadFileResult::READ_OK;
  };
}

auto const VersionIntHelper = cmJSONIntHelper<ReadFileResult>(
  ReadFileResult::READ_OK, ReadFileResult::INVALID_VERSION);

auto const VersionHelper = cmJSONRequiredHelper<FileVersion, ReadFileResult>(
  ReadFileResult::NO_VERSION,
  cmJSONObjectHelper<FileVersion, ReadFileResult>(
    ReadFileResult::READ_OK, ReadFileResult::INVALID_VERSION, false)
    .Bind("major"_s, &FileVersion::Major, VersionIntHelper)
    .Bind("minor"_s, &FileVersion::Minor, VersionIntHelper));

auto const RootVersionHelper =
  cmJSONObjectHelper<FileVersion, ReadFileResult>(ReadFileResult::READ_OK,
                                                  ReadFileResult::INVALID_ROOT)
    .Bind("version"_s, VersionHelper, false);

auto const VariableStringHelper = cmJSONStringHelper<ReadFileResult>(
  ReadFileResult::READ_OK, ReadFileResult::INVALID_VARIABLE);

auto const VariableObjectHelper =
  cmJSONObjectHelper<Variable, ReadFileResult>(
    ReadFileResult::READ_OK, ReadFileResult::INVALID_VARIABLE, false)
    .Bind("type"_s, &Variable::Type, VariableStringHelper, false)
    .Bind("value"_s, &Variable::Value, VariableStringHelper);

ReadFileResult VariableHelper(Variable& out, const Json::Value* value)
{
  if (value->isString()) {
    out.Type.clear();
    out.Value = value->asString();
    return ReadFileResult::READ_OK;
  }
  if (value->isObject()) {
    return VariableObjectHelper(out, value);
  }
  return ReadFileResult::INVALID_VARIABLE;
}

auto const VariablesHelper = cmJSONMapHelper<Variable, ReadFileResult>(
  ReadFileResult::READ_OK, ReadFileResult::INVALID_PRESET, VariableHelper);

auto const PresetStringHelper = cmJSONStringHelper<ReadFileResult>(
  ReadFileResult::READ_OK, ReadFileResult::INVALID_PRESET);

auto const EnvironmentHelper = cmJSONMapHelper<std::string, ReadFileResult>(
  ReadFileResult::READ_OK, ReadFileResult::INVALID_PRESET, PresetStringHelper);

auto const PresetVectorStringHelper =
  cmJSONVectorHelper<std::string, ReadFileResult>(
    ReadFileResult::READ_OK, ReadFileResult::INVALID_PRESET,
    PresetStringHelper);

ReadFileResult PresetInheritsHelper(std::vector<std::string>& out,
                                    const Json::Value* value)
{
  out.clear();
  if (!value) {
    return ReadFileResult::READ_OK;
  }

  if (value->isString()) {
    out.push_back(value->asString());
    return ReadFileResult::READ_OK;
  }

  return PresetVectorStringHelper(out, value);
}

auto const PresetBoolHelper = cmJSONBoolHelper<ReadFileResult>(
  ReadFileResult::READ_OK, ReadFileResult::INVALID_PRESET);

auto const PresetOptionalBoolHelper =
  cmJSONOptionalHelper<bool, ReadFileResult>(ReadFileResult::READ_OK,
                                             PresetBoolHelper);

auto const PresetWarningsHelper =
  cmJSONObjectHelper<UnexpandedPreset, ReadFileResult>(
    ReadFileResult::READ_OK, ReadFileResult::INVALID_PRESET, false)
    .Bind("dev"_s, &UnexpandedPreset::WarnDev, PresetOptionalBoolHelper, false)
    .Bind("deprecated"_s, &UnexpandedPreset::WarnDeprecated,
          PresetOptionalBoolHelper, false)
    .Bind("uninitialized"_s, &UnexpandedPreset::WarnUninitialized,
          PresetOptionalBoolHelper, false)
    .Bind("unusedCli"_s, &UnexpandedPreset::WarnUnusedCli,
          PresetOptionalBoolHelper, false)
    .Bind("systemVars"_s, &UnexpandedPreset::WarnSystemVars,
          PresetOptionalBoolHelper, false);

auto const PresetErrorsHelper =
  cmJSONObjectHelper<UnexpandedPreset, ReadFileResult>(
    ReadFileResult::READ_OK, ReadFileResult::INVALID_PRESET, false)
    .Bind("dev"_s, &UnexpandedPreset::ErrorDev, PresetOptionalBoolHelper,
          false)
    .Bind("deprecated"_s, &UnexpandedPreset::ErrorDeprecated,
          PresetOptionalBoolHelper, false);

auto const PresetDebugHelper =
  cmJSONObjectHelper<UnexpandedPreset, ReadFileResult>(
    ReadFileResult::READ_OK, ReadFileResult::INVALID_PRESET, false)
    .Bind("output"_s, &UnexpandedPreset::DebugOutput, PresetOptionalBoolHelper,
          false)
    .Bind("tryCompile"_s, &UnexpandedPreset::DebugTryCompile,
          PresetOptionalBoolHelper, false)
    .Bind("find"_s, &UnexpandedPreset::DebugFind, PresetOptionalBoolHelper,
          false);

auto const PresetHelper =
  cmJSONObjectHelper<UnexpandedPreset, ReadFileResult>(
    ReadFileResult::READ_OK, ReadFileResult::INVALID_PRESET, false)
    .Bind("name"_s, &UnexpandedPreset::Name, PresetStringHelper)
    .Bind("inherits"_s, &UnexpandedPreset::Inherits, PresetInheritsHelper,
          false)
    .Bind("hidden"_s, &UnexpandedPreset::Hidden, PresetBoolHelper, false)
    .Bind<std::nullptr_t>("vendor"_s, nullptr,
                          VendorHelper(ReadFileResult::INVALID_PRESET), false)
    .Bind("description"_s, &UnexpandedPreset::Description, PresetStringHelper,
          false)
    .Bind("longDescription"_s, &UnexpandedPreset::LongDescription,
          PresetStringHelper, false)
    .Bind("generator"_s, &UnexpandedPreset::Generator, PresetStringHelper,
          false)
    .Bind("architecture"_s, &UnexpandedPreset::Architecture,
          PresetStringHelper, false)
    .Bind("toolset"_s, &UnexpandedPreset::Toolset, PresetStringHelper, false)
    .Bind("binaryDir"_s, &UnexpandedPreset::BinaryDir, PresetStringHelper,
          false)
    .Bind<std::string>("cmakeExecutable"_s, nullptr, PresetStringHelper, false)
    .Bind("variables"_s, &UnexpandedPreset::Variables, VariablesHelper, false)
    .Bind("environment"_s, &UnexpandedPreset::Environment, EnvironmentHelper,
          false)
    .Bind("warnings"_s, PresetWarningsHelper, false)
    .Bind("errors"_s, PresetErrorsHelper, false)
    .Bind("debug"_s, PresetDebugHelper, false);

auto const PresetsHelper =
  cmJSONVectorHelper<UnexpandedPreset, ReadFileResult>(
    ReadFileResult::READ_OK, ReadFileResult::INVALID_PRESETS, PresetHelper);

auto const CMakeVersionUIntHelper = cmJSONUIntHelper<ReadFileResult>(
  ReadFileResult::READ_OK, ReadFileResult::INVALID_VERSION);

auto const CMakeVersionHelper =
  cmJSONObjectHelper<CMakeVersion, ReadFileResult>(
    ReadFileResult::READ_OK, ReadFileResult::INVALID_CMAKE_VERSION, false)
    .Bind("major"_s, &CMakeVersion::Major, CMakeVersionUIntHelper, false)
    .Bind("minor"_s, &CMakeVersion::Minor, CMakeVersionUIntHelper, false)
    .Bind("patch"_s, &CMakeVersion::Patch, CMakeVersionUIntHelper, false);

auto const RootPresetsHelper =
  cmJSONObjectHelper<RootPresets, ReadFileResult>(
    ReadFileResult::READ_OK, ReadFileResult::INVALID_ROOT, false)
    .Bind<FileVersion>("version"_s, nullptr, VersionHelper)
    .Bind("configurePresets"_s, &RootPresets::Presets, PresetsHelper, false)
    .Bind("cmakeMinimumRequired"_s, &RootPresets::CMakeMinimumRequired,
          CMakeVersionHelper, false)
    .Bind<std::nullptr_t>("vendor"_s, nullptr,
                          VendorHelper(ReadFileResult::INVALID_ROOT), false);

enum class CycleStatus
{
  Unvisited,
  InProgress,
  Verified,
};

void InheritString(std::string& child, const std::string& parent)
{
  if (child.empty()) {
    child = parent;
  }
}

void InheritOptionalBool(cm::optional<bool>& child,
                         const cm::optional<bool>& parent)
{
  if (!child) {
    child = parent;
  }
}

/**
 * Check preset inheritance for cycles (using a DAG check algorithm) while
 * also bubbling up fields through the inheritance hierarchy, then verify
 * that each preset has the required fields, either directly or through
 * inheritance.
 */
ReadFileResult VisitPreset(std::map<std::string, UnexpandedPreset>& presets,
                           UnexpandedPreset& preset,
                           std::map<std::string, CycleStatus> cycleStatus)
{
  switch (cycleStatus[preset.Name]) {
    case CycleStatus::InProgress:
      return ReadFileResult::CYCLIC_PRESET_INHERITANCE;
    case CycleStatus::Verified:
      return ReadFileResult::READ_OK;
    default:
      break;
  }

  cycleStatus[preset.Name] = CycleStatus::InProgress;

  for (auto const& i : preset.Inherits) {
    auto parent = presets.find(i);
    if (parent == presets.end()) {
      return ReadFileResult::INVALID_PRESET;
    }

    if (!preset.User && parent->second.User) {
      return ReadFileResult::USER_PRESET_INHERITANCE;
    }

    auto result = VisitPreset(presets, parent->second, cycleStatus);
    if (result != ReadFileResult::READ_OK) {
      return result;
    }

    InheritString(preset.Generator, parent->second.Generator);
    InheritString(preset.Architecture, parent->second.Architecture);
    InheritString(preset.Toolset, parent->second.Toolset);
    InheritString(preset.BinaryDir, parent->second.BinaryDir);
    InheritOptionalBool(preset.WarnDev, parent->second.WarnDev);
    InheritOptionalBool(preset.ErrorDev, parent->second.ErrorDev);
    InheritOptionalBool(preset.WarnDeprecated, parent->second.WarnDeprecated);
    InheritOptionalBool(preset.ErrorDeprecated,
                        parent->second.ErrorDeprecated);
    InheritOptionalBool(preset.WarnUninitialized,
                        parent->second.WarnUninitialized);
    InheritOptionalBool(preset.WarnUnusedCli, parent->second.WarnUnusedCli);
    InheritOptionalBool(preset.WarnSystemVars, parent->second.WarnSystemVars);
    for (auto const& v : parent->second.Variables) {
      preset.Variables.insert(v);
    }
    for (auto const& v : parent->second.Environment) {
      preset.Environment.insert(v);
    }
  }

  if (!preset.Hidden) {
    if (preset.Generator.empty()) {
      return ReadFileResult::INVALID_PRESET;
    }
    if (preset.BinaryDir.empty()) {
      return ReadFileResult::INVALID_PRESET;
    }
    if (preset.WarnDev == false && preset.ErrorDev == true) {
      return ReadFileResult::INVALID_PRESET;
    }
    if (preset.WarnDeprecated == false && preset.ErrorDeprecated == true) {
      return ReadFileResult::INVALID_PRESET;
    }
  }

  cycleStatus[preset.Name] = CycleStatus::Verified;
  return ReadFileResult::READ_OK;
}

ReadFileResult ComputePresetInheritance(
  std::map<std::string, UnexpandedPreset>& presets)
{
  std::map<std::string, CycleStatus> cycleStatus;
  for (auto const& it : presets) {
    cycleStatus[it.first] = CycleStatus::Unvisited;
  }

  for (auto& it : presets) {
    auto result = VisitPreset(presets, it.second, cycleStatus);
    if (result != ReadFileResult::READ_OK) {
      return result;
    }
  }

  return ReadFileResult::READ_OK;
}
}

std::string cmCMakePresetsFile::GetFilename(const std::string& sourceDir)
{
  return cmStrCat(sourceDir, "/CMakePresets.json");
}

std::string cmCMakePresetsFile::GetUserFilename(const std::string& sourceDir)
{
  return cmStrCat(sourceDir, "/CMakeUserPresets.json");
}

cmCMakePresetsFile::ReadFileResult cmCMakePresetsFile::ReadProjectPresets(
  const std::string& sourceDir, bool allowNoFiles)
{
  bool haveOneFile = false;
  this->SourceDir = sourceDir;
  this->Presets.clear();
  this->PresetOrder.clear();

  std::vector<std::string> presetOrder;
  std::map<std::string, UnexpandedPreset> presetMap;

  std::string filename = GetUserFilename(this->SourceDir);
  if (cmSystemTools::FileExists(filename)) {
    auto result = this->ReadJSONFile(filename, presetOrder, presetMap, true);
    if (result != ReadFileResult::READ_OK) {
      return result;
    }
    haveOneFile = true;
  }

  filename = GetFilename(this->SourceDir);
  if (cmSystemTools::FileExists(filename)) {
    auto result = this->ReadJSONFile(filename, presetOrder, presetMap, false);
    if (result != ReadFileResult::READ_OK) {
      return result;
    }
    haveOneFile = true;
  }

  if (!haveOneFile) {
    return allowNoFiles ? ReadFileResult::READ_OK
                        : ReadFileResult::FILE_NOT_FOUND;
  }

  auto result = ComputePresetInheritance(presetMap);
  if (result != ReadFileResult::READ_OK) {
    return result;
  }

  this->PresetOrder = std::move(presetOrder);
  this->Presets = std::move(presetMap);
  return ReadFileResult::READ_OK;
}

const char* cmCMakePresetsFile::ResultToString(ReadFileResult result)
{
  switch (result) {
    case ReadFileResult::READ_OK:
      return "OK";
    case ReadFileResult::FILE_NOT_FOUND:
      return "File not found";
    case ReadFileResult::JSON_PARSE_ERROR:
      return "JSON parse error";
    case ReadFileResult::INVALID_ROOT:
      return "Invalid root object";
    case ReadFileResult::NO_VERSION:
      return "No \"version\" field";
    case ReadFileResult::INVALID_VERSION:
      return "Invalid \"version\" field";
    case ReadFileResult::UNRECOGNIZED_VERSION:
      return "Unrecognized \"version\" field";
    case ReadFileResult::INVALID_CMAKE_VERSION:
      return "Invalid \"cmakeMinimumRequired\" field";
    case ReadFileResult::UNRECOGNIZED_CMAKE_VERSION:
      return "\"cmakeMinimumRequired\" version too new";
    case ReadFileResult::INVALID_PRESETS:
      return "Invalid \"configurePresets\" field";
    case ReadFileResult::INVALID_PRESET:
      return "Invalid preset";
    case ReadFileResult::INVALID_VARIABLE:
      return "Invalid CMake variable definition";
    case ReadFileResult::DUPLICATE_PRESETS:
      return "Duplicate presets";
    case ReadFileResult::CYCLIC_PRESET_INHERITANCE:
      return "Cyclic preset inheritance";
    case ReadFileResult::USER_PRESET_INHERITANCE:
      return "Project preset inherits from user preset";
    default:
      return "Unknown error";
  }
}
#endif

cm::optional<cmCMakePresetsFile::ExpandedPreset>
cmCMakePresetsFile::ExpandMacros(const UnexpandedPreset& preset) const
{
  ExpandedPreset retval{ preset };

  std::string binaryDir = preset.BinaryDir;
  if (!this->ExpandMacrosInPlace(preset, binaryDir)) {
    return cm::nullopt;
  }
  if (!cmSystemTools::FileIsFullPath(binaryDir)) {
    binaryDir = cmStrCat(this->SourceDir, '/', binaryDir);
  }
  retval.BinaryDir = cmSystemTools::CollapseFullPath(binaryDir);
  cmSystemTools::ConvertToUnixSlashes(retval.BinaryDir);

  for (auto& variable : retval.Variables) {
    if (!this->ExpandMacrosInPlace(preset, variable.second.Value)) {
      return cm::nullopt;
    }
  }

  return retval;
}

bool cmCMakePresetsFile::ExpandMacrosInPlace(const Preset& preset,
                                             std::string& str) const
{
  auto expanded = this->ExpandMacros(preset, str);
  if (!expanded) {
    return false;
  }
  str = expanded.data();
  return true;
}

cm::String cmCMakePresetsFile::ExpandMacros(const Preset& preset,
                                            const std::string& str) const
{
  std::string result;
  std::string macroName;

  enum class State
  {
    Default,
    Dollar,
    MacroName,
  } state = State::Default;

  for (auto c : str) {
    switch (state) {
      case State::Default:
        if (c == '$') {
          state = State::Dollar;
        } else {
          result += c;
        }
        break;

      case State::Dollar:
        if (c == '{') {
          state = State::MacroName;
        } else {
          result += '$';
          result += c;
          state = State::Default;
        }
        break;

      case State::MacroName:
        if (c == '}') {
          auto value = this->ExpandMacro(preset, macroName);
          if (!value) {
            return cm::String{};
          }
          result += value;
          macroName.clear();
          state = State::Default;
        } else {
          macroName += c;
        }
        break;
    }
  }

  switch (state) {
    case State::Default:
      break;
    case State::Dollar:
      result += '$';
      break;
    case State::MacroName:
      return cm::String{};
  }

  return result;
}

cm::String cmCMakePresetsFile::ExpandMacro(const Preset& preset,
                                           const std::string& macroName) const
{
  if (macroName == "sourceDir") {
    return this->SourceDir;
  }
  if (macroName == "sourceParentDir") {
    return cmSystemTools::GetParentDirectory(this->SourceDir);
  }
  if (macroName == "presetName") {
    return preset.Name;
  }
  if (macroName == "generator") {
    return preset.Generator;
  }
  if (macroName == "dollar") {
    return "$";
  }
  if (cmHasLiteralPrefix(macroName, "env.")) {
    const char* envName = macroName.c_str() + cmStrLen("env.");
    const char* value = std::getenv(envName);
    if (value) {
      return value;
    }
    auto it = preset.Environment.find(envName);
    if (it != preset.Environment.end()) {
      return it->second;
    }
    return "";
  }

  return cm::String{};
}

#ifndef CMAKE_BOOTSTRAP
cmCMakePresetsFile::ReadFileResult cmCMakePresetsFile::ReadJSONFile(
  const std::string& filename, std::vector<std::string>& presetOrder,
  std::map<std::string, UnexpandedPreset>& presetMap, bool user)
{
  cmsys::ifstream fin(filename.c_str());
  if (!fin) {
    return ReadFileResult::FILE_NOT_FOUND;
  }
  // If there's a BOM, toss it.
  cmsys::FStream::ReadBOM(fin);

  Json::Value root;
  Json::CharReaderBuilder builder;
  if (!Json::parseFromStream(builder, fin, &root, nullptr)) {
    return ReadFileResult::JSON_PARSE_ERROR;
  }

  FileVersion v;
  auto result = RootVersionHelper(v, &root);
  if (result != ReadFileResult::READ_OK) {
    return result;
  }
  if (v.Major < MIN_MAJOR_VERSION || v.Major > MAX_MAJOR_VERSION ||
      v.Minor < MIN_MAX_MINOR_VERSIONS[v.Major - MIN_MAJOR_VERSION].first ||
      v.Minor > MIN_MAX_MINOR_VERSIONS[v.Major - MIN_MAJOR_VERSION].second) {
    return ReadFileResult::UNRECOGNIZED_VERSION;
  }

  RootPresets presets;
  if ((result = RootPresetsHelper(presets, &root)) !=
      ReadFileResult::READ_OK) {
    return result;
  }

  unsigned int currentMajor = cmVersion::GetMajorVersion();
  unsigned int currentMinor = cmVersion::GetMinorVersion();
  unsigned int currentPatch = cmVersion::GetPatchVersion();
  auto const& required = presets.CMakeMinimumRequired;
  if (required.Major > currentMajor ||
      (required.Major == currentMajor &&
       (required.Minor > currentMinor ||
        (required.Minor == currentMinor &&
         (required.Patch > currentPatch))))) {
    return ReadFileResult::UNRECOGNIZED_CMAKE_VERSION;
  }

  for (auto& preset : presets.Presets) {
    preset.User = user;
    if (preset.Name.empty()) {
      return ReadFileResult::INVALID_PRESET;
    }
    if (!presetMap.insert({ preset.Name, preset }).second) {
      return ReadFileResult::DUPLICATE_PRESETS;
    }
    presetOrder.push_back(preset.Name);
  }

  return ReadFileResult::READ_OK;
}
#endif
