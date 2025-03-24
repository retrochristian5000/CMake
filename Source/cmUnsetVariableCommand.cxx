/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file LICENSE.rst or https://cmake.org/licensing for details.  */
#include "cmUnsetVariableCommand.h"

#include "cmExecutionStatus.h"
#include "cmMakefile.h"
#include "cmMessageType.h"
#include "cmSetVariableHelper.h"
#include "cmStringAlgorithms.h"
#include "cmSystemTools.h"

namespace {
// class UnsetVariableArgumentParser
class UnsetVariableArgumentParser
  : public SetVariableCommand::VariableArgumentParser
{
public:
  using RangeArguments = SetVariableCommand::RangeArguments;

  UnsetVariableArgumentParser(std::vector<std::string> const& args,
                              cmExecutionStatus& status)
    : SetVariableCommand::VariableArgumentParser(args, status)
  {
    if (args.empty()) {
      this->Status.GetMakefile().IssueMessage(
        MessageType::FATAL_ERROR,
        "Called with incorrect number of arguments.");
      this->ReportError = true;
      return;
    }

    this->Options = RangeArguments{ args.cbegin() + 1, args.cend() };
  }
};
}

// cmUnsetVariableCommand
bool cmUnsetVariableCommand(std::vector<std::string> const& args,
                            cmExecutionStatus& status)
{
  UnsetVariableArgumentParser parser{ args, status };
  if (parser.MaybeReportError()) {
    return false;
  }

  using VariableType = SetVariableCommand::VariableArgumentParser::
    VariableDescriptor::VariableType;
  auto const variableDescriptor = parser.getVariableDescriptor();

  // Handle the ENV{} signature
  if (variableDescriptor.Type == VariableType::Env) {
    // check for unexpected arguments
    if (!parser.GetOptions().empty()) {
      status.GetMakefile().IssueMessage(
        MessageType::FATAL_ERROR,
        cmStrCat("called with unexpected arguments for an ENV variable: ",
                 cmJoin(parser.GetOptions(), ",  "), '.'));
      return false;
    }

#ifndef CMAKE_BOOTSTRAP
    cmSystemTools::UnsetEnv(variableDescriptor.Name.c_str());
#endif
    return true;
  }

  // Handle the CACHE{} signature
  if (variableDescriptor.Type == VariableType::Cache) {
    status.GetMakefile().RemoveCacheDefinition(variableDescriptor.Name);
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

    if (optionsParser.GetScopes().contains(ScopeType::LOCAL)) {
      status.GetMakefile().RemoveDefinition(variableDescriptor.Name);
    }
    if (optionsParser.GetScopes().contains(ScopeType::PARENT)) {
      status.GetMakefile().RaiseScope(variableDescriptor.Name, nullptr);
    }
  }

  return true;
}
