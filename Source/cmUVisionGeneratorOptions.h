/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#ifndef cmUVisionGeneratorOptions_h
#define cmUVisionGeneratorOptions_h

#include "cmConfigure.h" // IWYU pragma: keep

#include <string>
#include <utility>
#include <vector>

#include "cmGeneratorTarget.h"
#include "cmLocalUVisionGenerator.h"
#include "cmXMLEditor.h"

namespace cmUVFlagTableOption {
enum
{
  None = 0,
  ValueInNext = (1 << 1),
  RegexInFlag = (1 << 2),
  RegexInValue = (1 << 3) | ValueInNext,
  ValueIsPath = (1 << 4),
  ValueIsDirectory = (1 << 5) | ValueIsPath,
  OnlyIfPresent = (1 << 6),
  ArmAds = (1 << 7),
  UvOpt = (1 << 8),
  Ignore = (1 << 9)
};
}

struct cmUVFlagTableModifier
{
  std::string Flag;
  std::string Property;
  std::string Define;
  std::string Value;
  std::string Set;
  unsigned int Options = 0;
};

struct cmUVFlagTableEntry
{
  std::string Path;
  std::string Default;
  std::vector<cmUVFlagTableModifier> Modifiers;
  unsigned int Options = 0;
};

using cmUVFlagTable = std::vector<cmUVFlagTableEntry>;

struct cmUVOptionValue
{
  std::string Path;
  std::string Value;
  bool Default = false;
  unsigned int Options = 0;
};

using cmUVOptionValues = std::vector<cmUVOptionValue>;

/**
 * \class cmUVisionGeneratorOptions
 * \brief Write a uVision project file
 */
class cmUVisionGeneratorOptions
{
public:
  cmUVisionGeneratorOptions(cmGeneratorTarget* target, std::string config);

  ~cmUVisionGeneratorOptions();

  cmUVOptionValues GetTargetOptions() { return TargetOptions; }

  cmUVOptionValues GetFileOptions(const cmSourceFile* sourceFile);

private:
  cmLocalUVisionGenerator* LocalGenerator;
  cmGeneratorTarget* Target;
  std::string Config;

  cmUVOptionValues TargetOptions;

  void ComputeTargetOptions();

  bool ProcessFlagTableModifier(cmUVFlagTableEntry const& flagEntry,
                                cmUVFlagTableModifier const& flagModifier,
                                std::string const& flag,
                                std::string const& value,
                                std::vector<cmUVOptionValue>& targetOptions);

  void ProcessFlagTable(
    cmUVFlagTable const& flagTable, std::vector<std::string>* flags,
    std::function<std::string(std::string)> const& getProperty,
    std::vector<cmUVOptionValue>& targetOptions);

  void SetTargetOption(std::string path, std::string const& value,
                       int options = cmUVFlagTableOption::None,
                       bool isDefault = false)
  {
    SetOption(&TargetOptions, std::move(path), value, options, isDefault);
  }

  void SetOption(cmUVOptionValues* optionValues, std::string path,
                 std::string const& value,
                 int options = cmUVFlagTableOption::None,
                 bool isDefault = false);
};

#endif