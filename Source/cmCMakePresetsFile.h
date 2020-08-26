/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#pragma once

#include <map>
#include <string>
#include <vector>

#include <cm/optional>

#include "cmString.hxx"

class cmCMakePresetsFile
{
public:
  class Variable
  {
  public:
    std::string Name;
    std::string Type;
    std::string Value;
  };

  class Preset
  {
  public:
    std::string Name;
    std::vector<std::string> Inherits;
    bool Hidden;
    bool User;
    std::string Description;
    std::string LongDescription;
    std::string Generator;
    std::string Architecture;
    std::string Toolset;
    std::string BinaryDir;

    std::vector<Variable> Variables;
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

  std::string SourceDir;
  std::map<std::string, UnexpandedPreset> Presets;
  std::vector<std::string> PresetOrder;

#ifndef CMAKE_BOOTSTRAP
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
    NO_PRESETS,
    INVALID_PRESETS,
    INVALID_PRESET,
    INVALID_VARIABLE,
    DUPLICATE_PRESETS,
    CYCLIC_PRESET_INHERITANCE,
    USER_PRESET_INHERITANCE,
  };

  static std::string GetFilename(const std::string& sourceDir);
  static std::string GetUserFilename(const std::string& sourceDir);
  ReadFileResult ReadProjectPresets(const std::string& sourceDir);
  static const char* ResultToString(ReadFileResult result);
#endif

  cm::optional<ExpandedPreset> ExpandMacros(
    const UnexpandedPreset& preset) const;

private:
  bool ExpandMacrosInPlace(const Preset& preset, std::string& str) const;
  cm::String ExpandMacros(const Preset& preset, const std::string& str) const;
  cm::String ExpandMacro(const Preset& preset,
                         const std::string& macroName) const;

#ifndef CMAKE_BOOTSTRAP
  ReadFileResult ReadJSONFile(
    const std::string& filename, std::vector<std::string>& presetOrder,
    std::map<std::string, UnexpandedPreset>& presetMap, bool user);
#endif
};
