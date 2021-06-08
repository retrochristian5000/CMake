/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#pragma once

#include <string>
#include <vector>

enum class LookupMethod
{
  ByName,
  IncludeAngle,
  IncludeQuote,
};

struct cmSourceReqInfo
{
  std::string LogicalName;
  std::string SourcePath;
  std::string CompiledModulePath;
  bool UseSourcePath;
  LookupMethod Method;
};

struct cmSourceInfo
{
  std::string PrimaryOutput;
  std::vector<std::string> ExtraOutputs;

  // Set of provided and required modules.
  std::vector<cmSourceReqInfo> Provides;
  std::vector<cmSourceReqInfo> Requires;
};

bool cmScanDepFormat_P1689_Parse(std::string const& arg_pp,
                                 cmSourceInfo* info);
bool cmScanDepFormat_P1689_Write(std::string const& path,
                                 cmSourceInfo const& info);
