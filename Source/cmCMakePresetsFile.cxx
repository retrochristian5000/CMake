/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#include "cmCMakePresetsFile.h"

#include <cstdlib>
#include <functional>
#include <utility>

#include <cmext/string_view>

#include <cm3p/json/reader.h>
#include <cm3p/json/value.h>

#include "cmsys/FStream.hxx"

#include "cmJSONHelpers.h"
#include "cmStringAlgorithms.h"
#include "cmSystemTools.h"
#include "cmVersion.h"

namespace {
enum class CycleStatus
{
  Unvisited,
  InProgress,
  Verified,
};

using ReadFileResult = cmCMakePresetsFile::ReadFileResult;
using CacheVariable = cmCMakePresetsFile::CacheVariable;
using ConfigurePreset = cmCMakePresetsFile::ConfigurePreset;
using BuildPreset = cmCMakePresetsFile::BuildPreset;
using TestPreset = cmCMakePresetsFile::TestPreset;
using ArchToolsetStrategy = cmCMakePresetsFile::ArchToolsetStrategy;

constexpr int MIN_VERSION = 1;
constexpr int MAX_VERSION = 1;

struct CMakeVersion
{
  unsigned int Major = 0;
  unsigned int Minor = 0;
  unsigned int Patch = 0;
};

struct RootPresets
{
  CMakeVersion CMakeMinimumRequired;
  std::vector<cmCMakePresetsFile::ConfigurePreset> ConfigurePresets;
  std::vector<cmCMakePresetsFile::BuildPreset> BuildPresets;
  std::vector<cmCMakePresetsFile::TestPreset> TestPresets;
};

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

auto const VersionHelper = cmJSONRequiredHelper<int, ReadFileResult>(
  ReadFileResult::NO_VERSION, VersionIntHelper);

auto const RootVersionHelper =
  cmJSONObjectHelper<int, ReadFileResult>(ReadFileResult::READ_OK,
                                          ReadFileResult::INVALID_ROOT)
    .Bind("version"_s, VersionHelper, false);

auto const VariableStringHelper = cmJSONStringHelper<ReadFileResult>(
  ReadFileResult::READ_OK, ReadFileResult::INVALID_VARIABLE);

ReadFileResult VariableValueHelper(std::string& out, const Json::Value* value)
{
  if (!value) {
    out.clear();
    return ReadFileResult::READ_OK;
  }

  if (value->isBool()) {
    out = value->asBool() ? "TRUE" : "FALSE";
    return ReadFileResult::READ_OK;
  }

  return VariableStringHelper(out, value);
}

auto const VariableObjectHelper =
  cmJSONObjectHelper<CacheVariable, ReadFileResult>(
    ReadFileResult::READ_OK, ReadFileResult::INVALID_VARIABLE, false)
    .Bind("type"_s, &CacheVariable::Type, VariableStringHelper, false)
    .Bind("value"_s, &CacheVariable::Value, VariableValueHelper);

ReadFileResult VariableHelper(cm::optional<CacheVariable>& out,
                              const Json::Value* value)
{
  if (value->isBool()) {
    out = CacheVariable{
      /*Type=*/"BOOL",
      /*Value=*/value->asBool() ? "TRUE" : "FALSE",
    };
    return ReadFileResult::READ_OK;
  }
  if (value->isString()) {
    out = CacheVariable{
      /*Type=*/"",
      /*Value=*/value->asString(),
    };
    return ReadFileResult::READ_OK;
  }
  if (value->isObject()) {
    out.emplace();
    return VariableObjectHelper(*out, value);
  }
  if (value->isNull()) {
    out = cm::nullopt;
    return ReadFileResult::READ_OK;
  }
  return ReadFileResult::INVALID_VARIABLE;
}

auto const VariablesHelper =
  cmJSONMapHelper<cm::optional<CacheVariable>, ReadFileResult>(
    ReadFileResult::READ_OK, ReadFileResult::INVALID_PRESET, VariableHelper);

auto const PresetStringHelper = cmJSONStringHelper<ReadFileResult>(
  ReadFileResult::READ_OK, ReadFileResult::INVALID_PRESET);

ReadFileResult EnvironmentHelper(cm::optional<std::string>& out,
                                 const Json::Value* value)
{
  if (!value || value->isNull()) {
    out = cm::nullopt;
    return ReadFileResult::READ_OK;
  }
  if (value->isString()) {
    out = value->asString();
    return ReadFileResult::READ_OK;
  }
  return ReadFileResult::INVALID_PRESET;
}

auto const EnvironmentMapHelper =
  cmJSONMapHelper<cm::optional<std::string>, ReadFileResult>(
    ReadFileResult::READ_OK, ReadFileResult::INVALID_PRESET,
    EnvironmentHelper);

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

auto const PresetIntHelper = cmJSONIntHelper<ReadFileResult>(
  ReadFileResult::READ_OK, ReadFileResult::INVALID_PRESET);

auto const PresetOptionalIntHelper = cmJSONOptionalHelper<int, ReadFileResult>(
  ReadFileResult::READ_OK, PresetIntHelper);

auto const PresetVectorIntHelper = cmJSONVectorHelper<int, ReadFileResult>(
  ReadFileResult::READ_OK, ReadFileResult::INVALID_PRESET, PresetIntHelper);

auto const PresetWarningsHelper =
  cmJSONObjectHelper<ConfigurePreset, ReadFileResult>(
    ReadFileResult::READ_OK, ReadFileResult::INVALID_PRESET, false)
    .Bind("dev"_s, &ConfigurePreset::WarnDev, PresetOptionalBoolHelper, false)
    .Bind("deprecated"_s, &ConfigurePreset::WarnDeprecated,
          PresetOptionalBoolHelper, false)
    .Bind("uninitialized"_s, &ConfigurePreset::WarnUninitialized,
          PresetOptionalBoolHelper, false)
    .Bind("unusedCli"_s, &ConfigurePreset::WarnUnusedCli,
          PresetOptionalBoolHelper, false)
    .Bind("systemVars"_s, &ConfigurePreset::WarnSystemVars,
          PresetOptionalBoolHelper, false);

auto const PresetErrorsHelper =
  cmJSONObjectHelper<ConfigurePreset, ReadFileResult>(
    ReadFileResult::READ_OK, ReadFileResult::INVALID_PRESET, false)
    .Bind("dev"_s, &ConfigurePreset::ErrorDev, PresetOptionalBoolHelper, false)
    .Bind("deprecated"_s, &ConfigurePreset::ErrorDeprecated,
          PresetOptionalBoolHelper, false);

auto const PresetDebugHelper =
  cmJSONObjectHelper<ConfigurePreset, ReadFileResult>(
    ReadFileResult::READ_OK, ReadFileResult::INVALID_PRESET, false)
    .Bind("output"_s, &ConfigurePreset::DebugOutput, PresetOptionalBoolHelper,
          false)
    .Bind("tryCompile"_s, &ConfigurePreset::DebugTryCompile,
          PresetOptionalBoolHelper, false)
    .Bind("find"_s, &ConfigurePreset::DebugFind, PresetOptionalBoolHelper,
          false);

ReadFileResult ArchToolsetStrategyHelper(
  cm::optional<ArchToolsetStrategy>& out, const Json::Value* value)
{
  if (!value) {
    out = cm::nullopt;
    return ReadFileResult::READ_OK;
  }

  if (!value->isString()) {
    return ReadFileResult::INVALID_PRESET;
  }

  if (value->asString() == "set") {
    out = ArchToolsetStrategy::Set;
    return ReadFileResult::READ_OK;
  }

  if (value->asString() == "external") {
    out = ArchToolsetStrategy::External;
    return ReadFileResult::READ_OK;
  }

  return ReadFileResult::INVALID_PRESET;
}

std::function<ReadFileResult(ConfigurePreset&, const Json::Value*)>
ArchToolsetHelper(
  std::string ConfigurePreset::*valueField,
  cm::optional<ArchToolsetStrategy> ConfigurePreset::*strategyField)
{
  auto const objectHelper =
    cmJSONObjectHelper<ConfigurePreset, ReadFileResult>(
      ReadFileResult::READ_OK, ReadFileResult::INVALID_PRESET, false)
      .Bind("value", valueField, PresetStringHelper, false)
      .Bind("strategy", strategyField, ArchToolsetStrategyHelper, false);
  return [valueField, strategyField, objectHelper](
           ConfigurePreset& out, const Json::Value* value) -> ReadFileResult {
    if (!value) {
      (out.*valueField).clear();
      out.*strategyField = cm::nullopt;
      return ReadFileResult::READ_OK;
    }

    if (value->isString()) {
      out.*valueField = value->asString();
      out.*strategyField = cm::nullopt;
      return ReadFileResult::READ_OK;
    }

    if (value->isObject()) {
      return objectHelper(out, value);
    }

    return ReadFileResult::INVALID_PRESET;
  };
}

auto const ArchitectureHelper = ArchToolsetHelper(
  &ConfigurePreset::Architecture, &ConfigurePreset::ArchitectureStrategy);
auto const ToolsetHelper = ArchToolsetHelper(
  &ConfigurePreset::Toolset, &ConfigurePreset::ToolsetStrategy);

auto const ConfigurePresetHelper =
  cmJSONObjectHelper<ConfigurePreset, ReadFileResult>(
    ReadFileResult::READ_OK, ReadFileResult::INVALID_PRESET, false)
    .Bind("name"_s, &ConfigurePreset::Name, PresetStringHelper)
    .Bind("inherits"_s, &ConfigurePreset::Inherits, PresetInheritsHelper,
          false)
    .Bind("hidden"_s, &ConfigurePreset::Hidden, PresetBoolHelper, false)
    .Bind<std::nullptr_t>("vendor"_s, nullptr,
                          VendorHelper(ReadFileResult::INVALID_PRESET), false)
    .Bind("displayName"_s, &ConfigurePreset::DisplayName, PresetStringHelper,
          false)
    .Bind("description"_s, &ConfigurePreset::Description, PresetStringHelper,
          false)
    .Bind("generator"_s, &ConfigurePreset::Generator, PresetStringHelper,
          false)
    .Bind("architecture"_s, ArchitectureHelper, false)
    .Bind("toolset"_s, ToolsetHelper, false)
    .Bind("binaryDir"_s, &ConfigurePreset::BinaryDir, PresetStringHelper,
          false)
    .Bind<std::string>("cmakeExecutable"_s, nullptr, PresetStringHelper, false)
    .Bind("cacheVariables"_s, &ConfigurePreset::CacheVariables,
          VariablesHelper, false)
    .Bind("environment"_s, &ConfigurePreset::Environment, EnvironmentMapHelper,
          false)
    .Bind("warnings"_s, PresetWarningsHelper, false)
    .Bind("errors"_s, PresetErrorsHelper, false)
    .Bind("debug"_s, PresetDebugHelper, false);

auto const BuildPresetHelper =
  cmJSONObjectHelper<BuildPreset, ReadFileResult>(
    ReadFileResult::READ_OK, ReadFileResult::INVALID_PRESET, false)
    .Bind("name"_s, &BuildPreset::Name, PresetStringHelper)
    .Bind("inherits"_s, &BuildPreset::Inherits, PresetInheritsHelper, false)
    .Bind("hidden"_s, &BuildPreset::Hidden, PresetBoolHelper, false)
    .Bind<std::nullptr_t>("vendor"_s, nullptr,
                          VendorHelper(ReadFileResult::INVALID_PRESET), false)
    .Bind("displayName"_s, &BuildPreset::DisplayName, PresetStringHelper,
          false)
    .Bind("description"_s, &BuildPreset::Description, PresetStringHelper,
          false)
    .Bind("environment"_s, &BuildPreset::Environment, EnvironmentMapHelper,
          false)
    .Bind("configurePreset"_s, &BuildPreset::ConfigurePreset,
          PresetStringHelper, false)
    .Bind("inheritConfigureEnvironment"_s,
          &BuildPreset::InheritConfigureEnvironment, PresetOptionalBoolHelper,
          false)
    .Bind("jobs"_s, &BuildPreset::Jobs, PresetOptionalIntHelper, false)
    .Bind("targets"_s, &BuildPreset::Targets, PresetVectorStringHelper, false)
    .Bind("configuration"_s, &BuildPreset::Configuration, PresetStringHelper,
          false)
    .Bind("cleanFirst"_s, &BuildPreset::CleanFirst, PresetOptionalBoolHelper,
          false)
    .Bind("verbose"_s, &BuildPreset::Verbose, PresetOptionalBoolHelper, false)
    .Bind("nativeToolOptions"_s, &BuildPreset::NativeToolOptions,
          PresetVectorStringHelper, false);

ReadFileResult TestPresetOutputVerbosityHelper(
  TestPreset::OutputOptions::VerbosityEnum& out, const Json::Value* value)
{
  if (!value) {
    out = TestPreset::OutputOptions::VerbosityEnum::Default;
    return ReadFileResult::READ_OK;
  }

  if (!value->isString()) {
    return ReadFileResult::INVALID_PRESET;
  }

  if (value->asString() == "default") {
    out = TestPreset::OutputOptions::VerbosityEnum::Default;
    return ReadFileResult::READ_OK;
  } else if (value->asString() == "verbose") {
    out = TestPreset::OutputOptions::VerbosityEnum::Verbose;
    return ReadFileResult::READ_OK;
  } else if (value->asString() == "extra") {
    out = TestPreset::OutputOptions::VerbosityEnum::Extra;
    return ReadFileResult::READ_OK;
  }

  return ReadFileResult::INVALID_PRESET;
}

auto const TestPresetOutputHelper =
  cmJSONObjectHelper<TestPreset::OutputOptions, ReadFileResult>(
    ReadFileResult::READ_OK, ReadFileResult::INVALID_PRESET, false)
    .Bind("shortProgress"_s, &TestPreset::OutputOptions::ShortProgress,
          PresetOptionalBoolHelper, false)
    .Bind("verbosity"_s, &TestPreset::OutputOptions::Verbosity,
          TestPresetOutputVerbosityHelper, false)
    .Bind("debug"_s, &TestPreset::OutputOptions::Debug,
          PresetOptionalBoolHelper, false)
    .Bind("outputOnFailure"_s, &TestPreset::OutputOptions::OutputOnFailure,
          PresetOptionalBoolHelper, false)
    .Bind("quiet"_s, &TestPreset::OutputOptions::Quiet,
          PresetOptionalBoolHelper, false)
    .Bind("outputLogFile"_s, &TestPreset::OutputOptions::OutputLogFile,
          PresetStringHelper, false)
    .Bind("labelSummary"_s, &TestPreset::OutputOptions::LabelSummary,
          PresetOptionalBoolHelper, false)
    .Bind("subprojectSummary"_s, &TestPreset::OutputOptions::SubprojectSummary,
          PresetOptionalBoolHelper, false)
    .Bind("maxPassedTestOutputSize"_s,
          &TestPreset::OutputOptions::MaxPassedTestOutputSize,
          PresetOptionalIntHelper, false)
    .Bind("maxFailedTestOutputSize"_s,
          &TestPreset::OutputOptions::MaxFailedTestOutputSize,
          PresetOptionalIntHelper, false)
    .Bind("maxTestNameWidth"_s, &TestPreset::OutputOptions::MaxTestNameWidth,
          PresetOptionalIntHelper, false);

auto const TestPresetFilterIncludeIndexHelper =
  cmJSONObjectHelper<TestPreset::Include::IndexOptions, ReadFileResult>(
    ReadFileResult::READ_OK, ReadFileResult::INVALID_PRESET)
    .Bind("start"_s, &TestPreset::Include::IndexOptions::Start,
          PresetOptionalIntHelper, false)
    .Bind("end"_s, &TestPreset::Include::IndexOptions::End,
          PresetOptionalIntHelper, false)
    .Bind("stride"_s, &TestPreset::Include::IndexOptions::Stride,
          PresetOptionalIntHelper, false)
    .Bind("specificTests"_s, &TestPreset::Include::IndexOptions::SpecificTests,
          PresetVectorIntHelper, false)
    .Bind("useUnion"_s, &TestPreset::Include::IndexOptions::UseUnion,
          PresetOptionalBoolHelper, false);

auto const TestPresetFilterIncludeHelper =
  cmJSONObjectHelper<TestPreset::Include, ReadFileResult>(
    ReadFileResult::READ_OK, ReadFileResult::INVALID_PRESET)
    .Bind("name"_s, &TestPreset::Include::Name, PresetStringHelper, false)
    .Bind("label"_s, &TestPreset::Include::Label, PresetStringHelper, false)
    .Bind("index"_s, &TestPreset::Include::Index,
          TestPresetFilterIncludeIndexHelper, false);

auto const TestPresetFilterExcludeFixturesHelper =
  cmJSONObjectHelper<TestPreset::Exclude::FixturesOptions, ReadFileResult>(
    ReadFileResult::READ_OK, ReadFileResult::INVALID_PRESET)
    .Bind("any"_s, &TestPreset::Exclude::FixturesOptions::Any,
          PresetStringHelper, false)
    .Bind("setup"_s, &TestPreset::Exclude::FixturesOptions::Setup,
          PresetStringHelper, false)
    .Bind("cleanup"_s, &TestPreset::Exclude::FixturesOptions::Cleanup,
          PresetStringHelper, false);

auto const TestPresetFilterExcludeHelper =
  cmJSONObjectHelper<TestPreset::Exclude, ReadFileResult>(
    ReadFileResult::READ_OK, ReadFileResult::INVALID_PRESET)
    .Bind("name"_s, &TestPreset::Exclude::Name, PresetStringHelper, false)
    .Bind("label"_s, &TestPreset::Exclude::Label, PresetStringHelper, false)
    .Bind("fixtures"_s, &TestPreset::Exclude::Fixtures,
          TestPresetFilterExcludeFixturesHelper, false);

ReadFileResult TestPresetExecutionShowOnlyHelper(
  cm::optional<TestPreset::ExecutionOptions::ShowOnlyEnum>& out,
  const Json::Value* value)
{
  if (!value) {
    out = cm::nullopt;
    return ReadFileResult::READ_OK;
  }

  if (!value->isString()) {
    return ReadFileResult::INVALID_PRESET;
  }

  if (value->asString() == "human") {
    out = TestPreset::ExecutionOptions::ShowOnlyEnum::Human;
    return ReadFileResult::READ_OK;
  } else if (value->asString() == "json-v1") {
    out = TestPreset::ExecutionOptions::ShowOnlyEnum::JsonV1;
    return ReadFileResult::READ_OK;
  }

  return ReadFileResult::INVALID_PRESET;
}

ReadFileResult TestPresetExecutionModeHelper(
  TestPreset::ExecutionOptions::RepeatOptions::ModeEnum& out,
  const Json::Value* value)
{
  if (!value) {
    return ReadFileResult::READ_OK;
  }

  if (!value->isString()) {
    return ReadFileResult::INVALID_PRESET;
  }

  if (value->asString() == "until-fail") {
    out = TestPreset::ExecutionOptions::RepeatOptions::ModeEnum::UntilFail;
    return ReadFileResult::READ_OK;
  } else if (value->asString() == "until-pass") {
    out = TestPreset::ExecutionOptions::RepeatOptions::ModeEnum::UntilPass;
    return ReadFileResult::READ_OK;
  } else if (value->asString() == "after-timeout") {
    out = TestPreset::ExecutionOptions::RepeatOptions::ModeEnum::AfterTimeout;
    return ReadFileResult::READ_OK;
  }

  return ReadFileResult::INVALID_PRESET;
}

auto const TestPresetExecutionRepeatHelper =
  cmJSONObjectHelper<TestPreset::ExecutionOptions::RepeatOptions,
                     ReadFileResult>(ReadFileResult::READ_OK,
                                     ReadFileResult::INVALID_PRESET)
    .Bind("mode"_s, &TestPreset::ExecutionOptions::RepeatOptions::Mode,
          TestPresetExecutionModeHelper, true)
    .Bind("count"_s, &TestPreset::ExecutionOptions::RepeatOptions::Count,
          PresetIntHelper, true);

ReadFileResult TestPresetExecutionNoTestsActionHelper(
  TestPreset::ExecutionOptions::NoTestsActionEnum& out,
  const Json::Value* value)
{
  if (!value) {
    out = TestPreset::ExecutionOptions::NoTestsActionEnum::Default;
    return ReadFileResult::READ_OK;
  }

  if (!value->isString()) {
    return ReadFileResult::INVALID_PRESET;
  }

  if (value->asString() == "default") {
    out = TestPreset::ExecutionOptions::NoTestsActionEnum::Default;
    return ReadFileResult::READ_OK;
  } else if (value->asString() == "error") {
    out = TestPreset::ExecutionOptions::NoTestsActionEnum::Error;
    return ReadFileResult::READ_OK;
  } else if (value->asString() == "ignore") {
    out = TestPreset::ExecutionOptions::NoTestsActionEnum::Ignore;
    return ReadFileResult::READ_OK;
  }

  return ReadFileResult::INVALID_PRESET;
}

auto const TestPresetExecutionHelper =
  cmJSONObjectHelper<TestPreset::ExecutionOptions, ReadFileResult>(
    ReadFileResult::READ_OK, ReadFileResult::INVALID_PRESET)
    .Bind("StopOnFailure"_s, &TestPreset::ExecutionOptions::StopOnFailure,
          PresetOptionalBoolHelper, false)
    .Bind("EnableFailover"_s, &TestPreset::ExecutionOptions::EnableFailover,
          PresetOptionalBoolHelper, false)
    .Bind("jobs"_s, &TestPreset::ExecutionOptions::Jobs,
          PresetOptionalIntHelper, false)
    .Bind("resourceSpecFile"_s,
          &TestPreset::ExecutionOptions::ResourceSpecFile, PresetStringHelper,
          false)
    .Bind("testLoad"_s, &TestPreset::ExecutionOptions::TestLoad,
          PresetOptionalIntHelper, false)
    .Bind("showOnly"_s, &TestPreset::ExecutionOptions::ShowOnly,
          TestPresetExecutionShowOnlyHelper, false)
    .Bind("rerunFailed"_s, &TestPreset::ExecutionOptions::RerunFailed,
          PresetOptionalBoolHelper, false)
    .Bind("repeat"_s, &TestPreset::ExecutionOptions::Repeat,
          TestPresetExecutionRepeatHelper, false)
    .Bind("interactiveDebugging"_s,
          &TestPreset::ExecutionOptions::InteractiveDebugging,
          PresetOptionalBoolHelper, false)
    .Bind("scheduleRandom"_s, &TestPreset::ExecutionOptions::ScheduleRandom,
          PresetOptionalBoolHelper, false)
    .Bind("timeout"_s, &TestPreset::ExecutionOptions::Timeout,
          PresetOptionalIntHelper, false)
    .Bind("noTestsAction"_s, &TestPreset::ExecutionOptions::NoTestsAction,
          TestPresetExecutionNoTestsActionHelper, false);

auto const TestPresetHelper =
  cmJSONObjectHelper<TestPreset, ReadFileResult>(
    ReadFileResult::READ_OK, ReadFileResult::INVALID_PRESET, false)
    .Bind("name"_s, &TestPreset::Name, PresetStringHelper)
    .Bind("inherits"_s, &TestPreset::Inherits, PresetInheritsHelper, false)
    .Bind("hidden"_s, &TestPreset::Hidden, PresetBoolHelper, false)
    .Bind<std::nullptr_t>("vendor"_s, nullptr,
                          VendorHelper(ReadFileResult::INVALID_PRESET), false)
    .Bind("displayName"_s, &TestPreset::DisplayName, PresetStringHelper, false)
    .Bind("description"_s, &TestPreset::Description, PresetStringHelper, false)
    .Bind("environment"_s, &TestPreset::Environment, EnvironmentMapHelper,
          false)
    .Bind("configurePreset"_s, &TestPreset::ConfigurePreset,
          PresetStringHelper, false)
    .Bind("inheritConfigureEnvironment"_s,
          &TestPreset::InheritConfigureEnvironment, PresetOptionalBoolHelper,
          false)
    .Bind("configuration"_s, &TestPreset::Configuration, PresetStringHelper,
          false)
    .Bind("overwriteConfigurationFile"_s,
          &TestPreset::OverwriteConfigurationFile, PresetOptionalBoolHelper,
          false)
    .Bind("output"_s, &TestPreset::Output, TestPresetOutputHelper, false)
    .Bind("include"_s, &TestPreset::FilterInclude,
          TestPresetFilterIncludeHelper, false)
    .Bind("exclude"_s, &TestPreset::FilterExclude,
          TestPresetFilterExcludeHelper, false)
    .Bind("execution"_s, &TestPreset::Execution, TestPresetExecutionHelper,
          false);

auto const ConfigurePresetsHelper =
  cmJSONVectorHelper<ConfigurePreset, ReadFileResult>(
    ReadFileResult::READ_OK, ReadFileResult::INVALID_PRESETS,
    ConfigurePresetHelper);

auto const BuildPresetsHelper =
  cmJSONVectorHelper<BuildPreset, ReadFileResult>(
    ReadFileResult::READ_OK, ReadFileResult::INVALID_PRESETS,
    BuildPresetHelper);

auto const TestPresetsHelper = cmJSONVectorHelper<TestPreset, ReadFileResult>(
  ReadFileResult::READ_OK, ReadFileResult::INVALID_PRESETS, TestPresetHelper);

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
    .Bind<int>("version"_s, nullptr, VersionHelper)
    .Bind("configurePresets"_s, &RootPresets::ConfigurePresets,
          ConfigurePresetsHelper, false)
    .Bind("buildPresets"_s, &RootPresets::BuildPresets, BuildPresetsHelper,
          false)
    .Bind("testPresets"_s, &RootPresets::TestPresets, TestPresetsHelper, false)
    .Bind("cmakeMinimumRequired"_s, &RootPresets::CMakeMinimumRequired,
          CMakeVersionHelper, false)
    .Bind<std::nullptr_t>("vendor"_s, nullptr,
                          VendorHelper(ReadFileResult::INVALID_ROOT), false);

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
ReadFileResult VisitPreset(
  std::map<std::string, cmCMakePresetsFile::PresetPair<ConfigurePreset>>&
    presets,
  ConfigurePreset& preset, std::map<std::string, CycleStatus> cycleStatus)
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

  if (preset.CacheVariables.count("") != 0) {
    return ReadFileResult::INVALID_PRESET;
  }
  if (preset.Environment.count("") != 0) {
    return ReadFileResult::INVALID_PRESET;
  }

  for (auto const& i : preset.Inherits) {
    auto parent = presets.find(i);
    if (parent == presets.end()) {
      return ReadFileResult::INVALID_PRESET;
    }

    if (!preset.User && parent->second.Unexpanded.User) {
      return ReadFileResult::USER_PRESET_INHERITANCE;
    }

    auto result = VisitPreset(presets, parent->second.Unexpanded, cycleStatus);
    if (result != ReadFileResult::READ_OK) {
      return result;
    }

    InheritString(preset.Generator, parent->second.Unexpanded.Generator);
    InheritString(preset.Architecture, parent->second.Unexpanded.Architecture);
    InheritString(preset.Toolset, parent->second.Unexpanded.Toolset);
    if (!preset.ArchitectureStrategy) {
      preset.ArchitectureStrategy =
        parent->second.Unexpanded.ArchitectureStrategy;
    }
    if (!preset.ToolsetStrategy) {
      preset.ToolsetStrategy = parent->second.Unexpanded.ToolsetStrategy;
    }
    InheritString(preset.BinaryDir, parent->second.Unexpanded.BinaryDir);
    InheritOptionalBool(preset.WarnDev, parent->second.Unexpanded.WarnDev);
    InheritOptionalBool(preset.ErrorDev, parent->second.Unexpanded.ErrorDev);
    InheritOptionalBool(preset.WarnDeprecated,
                        parent->second.Unexpanded.WarnDeprecated);
    InheritOptionalBool(preset.ErrorDeprecated,
                        parent->second.Unexpanded.ErrorDeprecated);
    InheritOptionalBool(preset.WarnUninitialized,
                        parent->second.Unexpanded.WarnUninitialized);
    InheritOptionalBool(preset.WarnUnusedCli,
                        parent->second.Unexpanded.WarnUnusedCli);
    InheritOptionalBool(preset.WarnSystemVars,
                        parent->second.Unexpanded.WarnSystemVars);
    for (auto const& v : parent->second.Unexpanded.CacheVariables) {
      preset.CacheVariables.insert(v);
    }
    for (auto const& v : parent->second.Unexpanded.Environment) {
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
  std::map<std::string, cmCMakePresetsFile::PresetPair<ConfigurePreset>>&
    presets)
{
  std::map<std::string, CycleStatus> cycleStatus;
  for (auto const& it : presets) {
    cycleStatus[it.first] = CycleStatus::Unvisited;
  }

  for (auto& it : presets) {
    auto result = VisitPreset(presets, it.second.Unexpanded, cycleStatus);
    if (result != ReadFileResult::READ_OK) {
      return result;
    }
  }

  return ReadFileResult::READ_OK;
}

constexpr const char* ValidPrefixes[] = {
  "",
  "env",
  "penv",
  "vendor",
};

bool PrefixesValidMacroNamespace(const std::string& str)
{
  for (auto const& prefix : ValidPrefixes) {
    if (cmHasPrefix(prefix, str)) {
      return true;
    }
  }
  return false;
}

bool IsValidMacroNamespace(const std::string& str)
{
  for (auto const& prefix : ValidPrefixes) {
    if (str == prefix) {
      return true;
    }
  }
  return false;
}

enum class ExpandMacroResult
{
  Ok,
  Ignore,
  Error,
};

ExpandMacroResult VisitEnv(const cmCMakePresetsFile& file,
                           cmCMakePresetsFile::ConfigurePreset& preset,
                           std::map<std::string, CycleStatus>& envCycles,
                           std::string& value, CycleStatus& status);
ExpandMacroResult ExpandMacros(const cmCMakePresetsFile& file,
                               cmCMakePresetsFile::ConfigurePreset& preset,
                               std::map<std::string, CycleStatus>& envCycles,
                               std::string& out);
ExpandMacroResult ExpandMacro(const cmCMakePresetsFile& file,
                              cmCMakePresetsFile::ConfigurePreset& preset,
                              std::map<std::string, CycleStatus>& envCycles,
                              std::string& out,
                              const std::string& macroNamespace,
                              const std::string& macroName);

bool ExpandMacros(const cmCMakePresetsFile& file,
                  const ConfigurePreset& preset,
                  cm::optional<ConfigurePreset>& out)
{
  out = preset;

  std::map<std::string, CycleStatus> envCycles;
  for (auto const& v : out->Environment) {
    envCycles[v.first] = CycleStatus::Unvisited;
  }

  for (auto& v : out->Environment) {
    if (v.second) {
      switch (VisitEnv(file, *out, envCycles, *v.second, envCycles[v.first])) {
        case ExpandMacroResult::Error:
          return false;
        case ExpandMacroResult::Ignore:
          out.reset();
          return true;
        case ExpandMacroResult::Ok:
          break;
      }
    }
  }

  std::string binaryDir = preset.BinaryDir;
  switch (ExpandMacros(file, *out, envCycles, binaryDir)) {
    case ExpandMacroResult::Error:
      return false;
    case ExpandMacroResult::Ignore:
      out.reset();
      return true;
    case ExpandMacroResult::Ok:
      break;
  }
  if (!cmSystemTools::FileIsFullPath(binaryDir)) {
    binaryDir = cmStrCat(file.SourceDir, '/', binaryDir);
  }
  out->BinaryDir = cmSystemTools::CollapseFullPath(binaryDir);
  cmSystemTools::ConvertToUnixSlashes(out->BinaryDir);

  for (auto& variable : out->CacheVariables) {
    if (variable.second) {
      switch (ExpandMacros(file, *out, envCycles, variable.second->Value)) {
        case ExpandMacroResult::Error:
          return false;
        case ExpandMacroResult::Ignore:
          out.reset();
          return true;
        case ExpandMacroResult::Ok:
          break;
      }
    }
  }

  return true;
}

ExpandMacroResult VisitEnv(const cmCMakePresetsFile& file,
                           cmCMakePresetsFile::ConfigurePreset& preset,
                           std::map<std::string, CycleStatus>& envCycles,
                           std::string& value, CycleStatus& status)
{
  if (status == CycleStatus::Verified) {
    return ExpandMacroResult::Ok;
  }
  if (status == CycleStatus::InProgress) {
    return ExpandMacroResult::Error;
  }

  status = CycleStatus::InProgress;
  auto e = ExpandMacros(file, preset, envCycles, value);
  if (e != ExpandMacroResult::Ok) {
    return e;
  }
  status = CycleStatus::Verified;
  return ExpandMacroResult::Ok;
}

ExpandMacroResult ExpandMacros(const cmCMakePresetsFile& file,
                               cmCMakePresetsFile::ConfigurePreset& preset,
                               std::map<std::string, CycleStatus>& envCycles,
                               std::string& out)
{
  std::string result;
  std::string macroNamespace;
  std::string macroName;

  enum class State
  {
    Default,
    MacroNamespace,
    MacroName,
  } state = State::Default;

  for (auto c : out) {
    switch (state) {
      case State::Default:
        if (c == '$') {
          state = State::MacroNamespace;
        } else {
          result += c;
        }
        break;

      case State::MacroNamespace:
        if (c == '{') {
          if (IsValidMacroNamespace(macroNamespace)) {
            state = State::MacroName;
          } else {
            result += '$';
            result += macroNamespace;
            result += '{';
            macroNamespace.clear();
            state = State::Default;
          }
        } else {
          macroNamespace += c;
          if (!PrefixesValidMacroNamespace(macroNamespace)) {
            result += '$';
            result += macroNamespace;
            macroNamespace.clear();
            state = State::Default;
          }
        }
        break;

      case State::MacroName:
        if (c == '}') {
          auto e = ExpandMacro(file, preset, envCycles, result, macroNamespace,
                               macroName);
          if (e != ExpandMacroResult::Ok) {
            return e;
          }
          macroNamespace.clear();
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
    case State::MacroNamespace:
      result += '$';
      result += macroNamespace;
      break;
    case State::MacroName:
      return ExpandMacroResult::Error;
  }

  out = std::move(result);
  return ExpandMacroResult::Ok;
}

ExpandMacroResult ExpandMacro(const cmCMakePresetsFile& file,
                              cmCMakePresetsFile::ConfigurePreset& preset,
                              std::map<std::string, CycleStatus>& envCycles,
                              std::string& out,
                              const std::string& macroNamespace,
                              const std::string& macroName)
{
  if (macroNamespace.empty()) {
    if (macroName == "sourceDir") {
      out += file.SourceDir;
      return ExpandMacroResult::Ok;
    }
    if (macroName == "sourceParentDir") {
      out += cmSystemTools::GetParentDirectory(file.SourceDir);
      return ExpandMacroResult::Ok;
    }
    if (macroName == "sourceDirName") {
      out += cmSystemTools::GetFilenameName(file.SourceDir);
      return ExpandMacroResult::Ok;
    }
    if (macroName == "presetName") {
      out += preset.Name;
      return ExpandMacroResult::Ok;
    }
    if (macroName == "generator") {
      out += preset.Generator;
      return ExpandMacroResult::Ok;
    }
    if (macroName == "dollar") {
      out += '$';
      return ExpandMacroResult::Ok;
    }
  }

  if (macroNamespace == "env" && !macroName.empty()) {
    auto v = preset.Environment.find(macroName);
    if (v != preset.Environment.end() && v->second) {
      auto e =
        VisitEnv(file, preset, envCycles, *v->second, envCycles[macroName]);
      if (e != ExpandMacroResult::Ok) {
        return e;
      }
      out += *v->second;
      return ExpandMacroResult::Ok;
    }
  }

  if (macroNamespace == "env" || macroNamespace == "penv") {
    if (macroName.empty()) {
      return ExpandMacroResult::Error;
    }
    const char* value = std::getenv(macroName.c_str());
    if (value) {
      out += value;
    }
    return ExpandMacroResult::Ok;
  }

  if (macroNamespace == "vendor") {
    return ExpandMacroResult::Ignore;
  }

  return ExpandMacroResult::Error;
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
  this->SourceDir = sourceDir;
  this->ClearPresets();

  auto result = this->ReadProjectPresetsInternal(sourceDir, allowNoFiles);
  if (result != ReadFileResult::READ_OK) {
    this->ClearPresets();
  }

  return result;
}

cmCMakePresetsFile::ReadFileResult
cmCMakePresetsFile::ReadProjectPresetsInternal(const std::string& sourceDir,
                                               bool allowNoFiles)
{
  bool haveOneFile = false;

  std::string filename = GetUserFilename(this->SourceDir);
  if (cmSystemTools::FileExists(filename)) {
    auto result = this->ReadJSONFile(filename, true);
    if (result != ReadFileResult::READ_OK) {
      return result;
    }
    haveOneFile = true;
  }

  filename = GetFilename(this->SourceDir);
  if (cmSystemTools::FileExists(filename)) {
    auto result = this->ReadJSONFile(filename, false);
    if (result != ReadFileResult::READ_OK) {
      return result;
    }
    haveOneFile = true;
  }

  if (!haveOneFile) {
    return allowNoFiles ? ReadFileResult::READ_OK
                        : ReadFileResult::FILE_NOT_FOUND;
  }

  auto result = ComputePresetInheritance(this->ConfigurePresets);
  if (result != ReadFileResult::READ_OK) {
    return result;
  }

  for (auto& it : this->ConfigurePresets) {
    if (!ExpandMacros(*this, it.second.Unexpanded, it.second.Expanded)) {
      return ReadFileResult::INVALID_MACRO_EXPANSION;
    }
  }

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
    case ReadFileResult::INVALID_MACRO_EXPANSION:
      return "Invalid macro expansion";
  }

  return "Unknown error";
}

cmCMakePresetsFile::ReadFileResult cmCMakePresetsFile::ReadJSONFile(
  const std::string& filename, bool user)
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

  int v = 0;
  auto result = RootVersionHelper(v, &root);
  if (result != ReadFileResult::READ_OK) {
    return result;
  }
  if (v < MIN_VERSION || v > MAX_VERSION) {
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

  for (auto& preset : presets.ConfigurePresets) {
    preset.User = user;
    if (preset.Name.empty()) {
      return ReadFileResult::INVALID_PRESET;
    }
    if (!this->ConfigurePresets
           .insert({ preset.Name, { preset, cm::nullopt } })
           .second) {
      return ReadFileResult::DUPLICATE_PRESETS;
    }
    this->ConfigurePresetOrder.push_back(preset.Name);
  }

  for (auto& preset : presets.BuildPresets) {
    preset.User = user;
    if (preset.Name.empty()) {
      return ReadFileResult::INVALID_PRESET;
    }
    if (!this->BuildPresets.insert({ preset.Name, { preset, cm::nullopt } })
           .second) {
      return ReadFileResult::DUPLICATE_PRESETS;
    }
    this->BuildPresetOrder.push_back(preset.Name);
  }

  for (auto& preset : presets.TestPresets) {
    preset.User = user;
    if (preset.Name.empty()) {
      return ReadFileResult::INVALID_PRESET;
    }
    if (!this->TestPresets.insert({ preset.Name, { preset, cm::nullopt } })
           .second) {
      return ReadFileResult::DUPLICATE_PRESETS;
    }
    this->TestPresetOrder.push_back(preset.Name);
  }

  return ReadFileResult::READ_OK;
}

void cmCMakePresetsFile::ClearPresets()
{
  this->ConfigurePresets.clear();
  this->BuildPresets.clear();
  this->TestPresets.clear();

  this->ConfigurePresetOrder.clear();
  this->BuildPresetOrder.clear();
  this->TestPresetOrder.clear();
}