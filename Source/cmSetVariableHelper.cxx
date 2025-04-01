/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file LICENSE.rst or https://cmake.org/licensing for details.  */
#include "cmSetVariableHelper.h"

#include <cm/string_view>
#include <cmext/enum_set>
#include <cmext/string_view>

#include "cmArgumentParser.h"
#include "cmExecutionStatus.h"
#include "cmMakefile.h"
#include "cmMessageType.h"
#include "cmStringAlgorithms.h"

namespace SetVariableCommand {

// Class ArgumentParser
VariableArgumentParser::VariableArgumentParser(
  std::vector<std::string> const& args, cmExecutionStatus& status)
  : Status{ status }
  , Args{ args }
  , Options{ args.cend(), args.cend() }
{
}

VariableArgumentParser::VariableDescriptor
VariableArgumentParser::getVariableDescriptor() const
{
  auto const& variable = this->Args[0]; // VAR is always first

  if (cmHasLiteralPrefix(variable, "ENV{") && variable.size() > 5 &&
      cmHasLiteralSuffix(variable, "}")) {
    return VariableDescriptor{ variable.substr(4, variable.size() - 5),
                               VariableDescriptor::VariableType::Env };
  }
  if (cmHasLiteralPrefix(variable, "CACHE{") && variable.size() > 7 &&
      cmHasLiteralSuffix(variable, "}")) {
    return VariableDescriptor{ variable.substr(6, variable.size() - 7),
                               VariableDescriptor::VariableType::Cache };
  }
  return VariableDescriptor{ variable,
                             VariableDescriptor::VariableType::Normal };
}

// Class NormalVariableOptionParser
bool NormalVariableOptionParser::Parse()
{
  // Handle optional arguments
  struct Arguments : public ArgumentParser::ParseResult
  {
    ArgumentParser::Continue validateScopeValue(cm::string_view scope)
    {
      if (scope != "LOCAL"_s && scope != "PARENT"_s) {
        this->InvalidScopes.emplace_back(std::string{ scope });
      } else {
        this->Scopes.insert(scope == "LOCAL"_s ? ScopeType::LOCAL
                                               : ScopeType::PARENT);
      }
      return ArgumentParser::Continue::Yes;
    }
    ScopeSet Scopes;
    std::vector<std::string> InvalidScopes;
  };
  static auto const parser = cmArgumentParser<Arguments>{}.Bind(
    "SCOPE"_s, &Arguments::validateScopeValue);
  std::vector<std::string> unrecognizedArguments;
  auto parsedArgs = parser.Parse(this->Options, &unrecognizedArguments);
  if (!unrecognizedArguments.empty()) {
    this->Status.GetMakefile().IssueMessage(
      MessageType::FATAL_ERROR,
      cmStrCat("Called with unsupported argument(s): ",
               cmJoin(unrecognizedArguments, ",  "_s), '.'));
    return false;
  }
  if (parsedArgs.MaybeReportError(this->Status.GetMakefile())) {
    return false;
  }

  if (!parsedArgs.InvalidScopes.empty()) {
    this->Status.GetMakefile().IssueMessage(
      MessageType::FATAL_ERROR,
      cmStrCat("Error after keyword \"SCOPE\":\nInvalid value(s): ",
               cmJoin(parsedArgs.InvalidScopes, ",  "_s), '.'));
    return false;
  }

  this->Scopes = parsedArgs.Scopes;
  if (!this->Scopes) {
    this->Scopes.insert(ScopeType::LOCAL);
  }

  return true;
}

} // namespace SetVariableCommand
