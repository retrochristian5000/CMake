/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file LICENSE.rst or https://cmake.org/licensing for details.  */
#include "cmSetVariableCommand.h"

#include <algorithm>
#include <vector>

#include <cm/optional>
#include <cm/string_view>
#include <cmext/string_view>

#include "cmArgumentParser.h"
#include "cmArgumentParserTypes.h"
#include "cmExecutionStatus.h"
#include "cmList.h"
#include "cmMakefile.h"
#include "cmMessageType.h"
#include "cmRange.h"
#include "cmSetVariableHelper.h"
#include "cmState.h"
#include "cmStateTypes.h"
#include "cmStringAlgorithms.h"
#include "cmSystemTools.h"
#include "cmValue.h"

namespace {
// class SetVariableArgumentParser
class SetVariableArgumentParser
  : public SetVariableCommand::VariableArgumentParser
{
public:
  using RangeArguments = SetVariableCommand::RangeArguments;

  SetVariableArgumentParser(std::vector<std::string> const& args,
                            cmExecutionStatus& status)
    : SetVariableCommand::VariableArgumentParser(args, status)
    , Values{ args.cend(), args.cend() }
  {
    if (args.size() < 2) {
      this->Status.GetMakefile().IssueMessage(
        MessageType::FATAL_ERROR, "Required argument 'VALUES' is missing.");
      this->ReportError = true;
      return;
    }

    // values handling
    auto valuesArg = std::find(args.cbegin() + 1, args.cend(), "VALUES");
    if (valuesArg == args.cend()) {
      this->Status.GetMakefile().IssueMessage(
        MessageType::FATAL_ERROR, "Required argument 'VALUES' is missing.");
      this->ReportError = true;
      return;
    }
    this->Values = RangeArguments{ valuesArg + 1, args.cend() };
    this->Options = RangeArguments{ args.cbegin() + 1, valuesArg };
  }

  RangeArguments const& GetValues() const { return this->Values; };

private:
  RangeArguments Values;
};
}

// cmSetVariableCommand
bool cmSetVariableCommand(std::vector<std::string> const& args,
                          cmExecutionStatus& status)
{
  SetVariableArgumentParser parser{ args, status };
  if (parser.MaybeReportError()) {
    return false;
  }

  using VariableType = SetVariableCommand::VariableArgumentParser::
    VariableDescriptor::VariableType;
  auto const variableDescriptor = parser.getVariableDescriptor();

  auto const& values = parser.GetValues();

  // Handle the ENV{} signature
  if (variableDescriptor.Type == VariableType::Env) {
    // check for unexpected arguments
    if (!parser.GetOptions().empty()) {
      status.GetMakefile().IssueMessage(
        MessageType::FATAL_ERROR,
        cmStrCat("called with unexpected argument(s) for an ENV variable: ",
                 cmJoin(parser.GetOptions(), ",  "), '.'));
      return false;
    }

    // Only the first value will be used. Other values will be ignored.
    if (values.size() > 1) {
      std::string m = "Only the first value is used when setting "
                      "an environment variable.  Value '" +
        *(values.begin() + 1) + "' and later are unused.";
      status.GetMakefile().IssueMessage(
        MessageType::AUTHOR_WARNING,
        cmStrCat("Only the first value is used when setting "
                 "an environment variable.  Value '",
                 *(values.begin() + 1), "' and later are unused."));
    }

    std::string currentValue;
    cmSystemTools::GetEnv(variableDescriptor.Name, currentValue);
    if (!values.empty() && !values.begin()->empty()) {
      auto const& newValue = *values.begin();
      // set it only if it is different from current value
      if (currentValue != newValue) {
        cmSystemTools::PutEnv(
          cmStrCat(variableDescriptor.Name, '=', newValue));
      }
    } else {
      cmSystemTools::PutEnv(cmStrCat(variableDescriptor.Name, '='));
    }
    return true;
  }

  // Handle the CACHE{} signature
  if (variableDescriptor.Type == VariableType::Cache) {
    // Handle arguments
    struct Arguments : public ArgumentParser::ParseResult
    {
      ArgumentParser::Continue validateTypeValue(cm::string_view type)
      {
        if (!cmState::StringToCacheEntryType(std::string{ type },
                                             this->Type)) {
          this->AddKeywordError("TYPE"_s,
                                cmStrCat("Invalid value: ", type, '.'));
        }
        return ArgumentParser::Continue::No;
      }
      cmStateEnums::CacheEntryType Type = cmStateEnums::UNINITIALIZED;
      cm::optional<ArgumentParser::NonEmpty<std::vector<std::string>>> Help;
      bool Force = false;
    };
    static auto const optionsParser =
      cmArgumentParser<Arguments>{}
        .Bind("TYPE"_s, &Arguments::validateTypeValue)
        .Bind("HELP"_s, &Arguments::Help)
        .Bind("FORCE"_s, &Arguments::Force);
    std::vector<std::string> unrecognizedArguments;
    auto parsedArgs =
      optionsParser.Parse(parser.GetOptions(), &unrecognizedArguments);
    if (!unrecognizedArguments.empty()) {
      status.GetMakefile().IssueMessage(
        MessageType::FATAL_ERROR,
        cmStrCat("Called with unsupported argument(s): ",
                 cmJoin(unrecognizedArguments, ",  "_s), '.'));
      return false;
    }
    if (parsedArgs.MaybeReportError(status.GetMakefile())) {
      return false;
    }

    // see if this is already in the cache
    cmState* state = status.GetMakefile().GetState();
    cmValue existingValue = state->GetCacheEntryValue(variableDescriptor.Name);
    cmStateEnums::CacheEntryType existingType =
      state->GetCacheEntryType(variableDescriptor.Name);
    if (parsedArgs.Type == cmStateEnums::UNINITIALIZED) {
      parsedArgs.Type = existingType == cmStateEnums::UNINITIALIZED
        ? cmStateEnums::STRING
        : existingType;
    }
    std::string help = parsedArgs.Help
      ? cmJoin(*parsedArgs.Help, "")
      : *state->GetCacheEntryProperty(variableDescriptor.Name, "HELPSTRING");
    if (existingValue && existingType != cmStateEnums::UNINITIALIZED) {
      // if the set is trying to CACHE the value but the value
      // is already in the cache and the type is not internal
      // then leave now without setting any definitions in the cache
      // or the makefile
      if (parsedArgs.Type != cmStateEnums::INTERNAL && !parsedArgs.Force) {
        return true;
      }
    }
    status.GetMakefile().AddCacheDefinition(variableDescriptor.Name,
                                            cmList::to_string(values), help,
                                            parsedArgs.Type, parsedArgs.Force);

    return true;
  }

  // Handle standard variable
  {
    SetVariableCommand::NormalVariableOptionParser optionsParser{
      parser.GetOptions(), status
    };

    if (!optionsParser.Parse()) {
      return false;
    }

    using ScopeType =
      SetVariableCommand::NormalVariableOptionParser::ScopeType;
    auto const value = cmList::to_string(values);

    if (optionsParser.GetScopes().contains(ScopeType::LOCAL)) {
      status.GetMakefile().AddDefinition(variableDescriptor.Name, value);
    }
    if (optionsParser.GetScopes().contains(ScopeType::PARENT)) {
      status.GetMakefile().RaiseScope(variableDescriptor.Name, value.c_str());
    }
  }

  return true;
}
