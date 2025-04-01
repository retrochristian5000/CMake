/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file LICENSE.rst or https://cmake.org/licensing for details.  */
#pragma once

#include "cmConfigure.h" // IWYU pragma: keep

#include <cstdint>
#include <string>
#include <vector>

#include <cmext/enum_set>

#include "cmRange.h"

class cmExecutionStatus;

namespace SetVariableCommand {

using RangeArguments = cmRange<std::vector<std::string>::const_iterator>;

class VariableArgumentParser
{
protected:
  VariableArgumentParser(std::vector<std::string> const& args,
                         cmExecutionStatus& status);

public:
  bool MaybeReportError() const { return this->ReportError; }

  struct VariableDescriptor
  {
    enum VariableType : std::uint8_t
    {
      Normal,
      Cache,
      Env
    };

    std::string Name;
    VariableType Type;
  };
  VariableDescriptor getVariableDescriptor() const;

  RangeArguments const& GetOptions() const { return this->Options; };

protected:
  bool ReportError = false;
  cmExecutionStatus& Status;
  std::vector<std::string> const& Args;
  RangeArguments Options;
};

enum class ScopeType : std::uint8_t
{
  LOCAL,
  PARENT
};
using ScopeSet = cm::enum_set<ScopeType, 2>;

class NormalVariableOptionParser
{
public:
  enum class ScopeType : std::uint8_t
  {
    LOCAL,
    PARENT
  };
  using ScopeSet = cm::enum_set<ScopeType, 2>;

  NormalVariableOptionParser(RangeArguments const& options,
                             cmExecutionStatus& status)
    : Options{ options }
    , Status{ status }
  {
  }

  bool Parse();

  ScopeSet const& GetScopes() const { return this->Scopes; };

private:
  RangeArguments const& Options;
  cmExecutionStatus& Status;
  ScopeSet Scopes;
};

} // namespace SetVariableCommand
