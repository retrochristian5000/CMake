/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#pragma once

#include <map>
#include <string>
#include <utility>
#include <vector>

#include <cm/optional>

class cmCMakePresetsFile
{
public:
  enum class ArchToolsetStrategy
  {
    Set,
    External,
  };

  class CacheVariable
  {
  public:
    std::string Type;
    std::string Value;
  };

  class Preset
  {
  public:
#if __cplusplus < 201703L && (!defined(_MSVC_LANG) || _MSVC_LANG < 201703L)
    Preset() = default;
    Preset(const Preset& /*other*/) = default;
    Preset(Preset&& /*other*/) = default;

    Preset& operator=(const Preset& /*other*/) = default;

    // The move assignment operators for several STL classes did not become
    // noexcept until C++17, which causes some tools to warn about this move
    // assignment operator throwing an exception when it shouldn't. Disable the
    // move assignment operator until C++17 is enabled.
    Preset& operator=(Preset&& /*other*/) = delete;
#endif

    std::string Name;
    std::vector<std::string> Inherits;
    bool Hidden;
    bool User;
    std::string DisplayName;
    std::string Description;

    std::map<std::string, cm::optional<std::string>> Environment;
  };

  class ConfigurePreset : Preset
  {
  public:
    std::string Generator;
    std::string Architecture;
    cm::optional<ArchToolsetStrategy> ArchitectureStrategy;
    std::string Toolset;
    cm::optional<ArchToolsetStrategy> ToolsetStrategy;
    std::string BinaryDir;

    std::map<std::string, cm::optional<CacheVariable>> CacheVariables;

    cm::optional<bool> WarnDev;
    cm::optional<bool> ErrorDev;
    cm::optional<bool> WarnDeprecated;
    cm::optional<bool> ErrorDeprecated;
    cm::optional<bool> WarnUninitialized;
    cm::optional<bool> WarnUnusedCli;
    cm::optional<bool> WarnSystemVars;

    cm::optional<bool> DebugOutput;
    cm::optional<bool> DebugTryCompile;
    cm::optional<bool> DebugFind;
  };

  class BuildPreset : Preset
  {
  public:
    std::string ConfigurePreset;
    bool InheritConfigureEnvironment;
    cm::optional<int> Jobs;
    std::vector<std::string> Targets;
    std::string Configuration;
    bool CleanFirst;
    bool Verbose;
    std::vector<std::string> NativeToolOptions;
  };

  class TestPreset : Preset
  {
  public:
    struct Output
    {
      enum class Verbosity
      {
        Default,
        Verbose,
        Extra
      };

      bool ShortProgress;
      Verbosity Verbosity;
      bool Debug;
      bool OutputOnFailure;
      bool Quiet;
      std::string OutputLogFile;
      bool LabelSummary;
      bool SubprojectSummary;
      cm::optional<int> MaxPassedTestOutputSize;
      cm::optional<int> MaxFailedTestOutputSize;
      cm::optional<int> MaxTestNameWidth;
    };

    struct FilterInclude
    {
      struct Index
      {
        cm::optional<int> Start;
        cm::optional<int> End;
        cm::optional<int> Stride;
        std::vector<int> SpecificTests;
        bool UseUnion;
      };

      std::string Name;
      std::string Label;
      Index Index;
    };

    struct FilterExclude
    {
      struct Fixtures
      {
        std::string Any;
        std::string Setup;
        std::string Cleanup;
      };

      std::string Name;
      std::string Label;
      Fixtures Fixtures;
    };

    struct Execution
    {
      enum class ShowOnly
      {
        Human,
        JsonV1
      };

      struct Repeat
      {
        enum class Mode
        {
          UntilFail,
          UntilPass,
          AfterTimeout
        };

        Mode Mode;
        int Count;
      };

      enum class NoTestsAction
      {
        Default,
        Error,
        Ignore
      };

      bool StopOnFailure;
      bool EnableFailover;
      cm::optional<int> Jobs;
      std::string ResourceSpecFile;
      cm::optional<int> TestLoad;
      ShowOnly ShowOnly;
      bool RerunFailed;

      Repeat Repeat;
      cm::optional<bool> InteractiveDebugging;
      bool ScheduleRandom;
      cm::optional<int> Timeout;
      NoTestsAction NoTestsAction;
    };

    std::string ConfigurePreset;
    bool InheritConfigureEnvironment;
    std::string Configuration;
    bool OverwriteConfigurationFile;
    Output Output;
    FilterInclude FilterInclude;
    FilterExclude FilterExclude;
    Execution Execution;
  };

  class UnexpandedPreset : public Preset
  {
  public:
    using Preset::Preset;

    UnexpandedPreset() = default;
    UnexpandedPreset(const Preset& preset)
      : Preset(preset)
    {
    }
    UnexpandedPreset(Preset&& preset)
      : Preset(std::move(preset))
    {
    }
  };

  class ExpandedPreset : public Preset
  {
  public:
    using Preset::Preset;

    ExpandedPreset() = default;
    ExpandedPreset(const Preset& preset)
      : Preset(preset)
    {
    }
    ExpandedPreset(Preset&& preset)
      : Preset(std::move(preset))
    {
    }
  };

  class PresetPair
  {
  public:
    UnexpandedPreset Unexpanded;
    cm::optional<ExpandedPreset> Expanded;
  };

  std::string SourceDir;
  std::map<std::string, PresetPair> Presets;
  std::vector<std::string> PresetOrder;

  enum class ReadFileResult
  {
    READ_OK,
    FILE_NOT_FOUND,
    JSON_PARSE_ERROR,
    INVALID_ROOT,
    NO_VERSION,
    INVALID_VERSION,
    UNRECOGNIZED_VERSION,
    INVALID_CMAKE_VERSION,
    UNRECOGNIZED_CMAKE_VERSION,
    INVALID_PRESETS,
    INVALID_PRESET,
    INVALID_VARIABLE,
    DUPLICATE_PRESETS,
    CYCLIC_PRESET_INHERITANCE,
    USER_PRESET_INHERITANCE,
    INVALID_MACRO_EXPANSION,
  };

  static std::string GetFilename(const std::string& sourceDir);
  static std::string GetUserFilename(const std::string& sourceDir);
  ReadFileResult ReadProjectPresets(const std::string& sourceDir,
                                    bool allowNoFiles = false);
  static const char* ResultToString(ReadFileResult result);

private:
  ReadFileResult ReadJSONFile(const std::string& filename,
                              std::vector<std::string>& presetOrder,
                              std::map<std::string, PresetPair>& presetMap,
                              bool user);
};
