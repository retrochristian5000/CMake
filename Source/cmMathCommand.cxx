/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file LICENSE.rst or https://cmake.org/licensing for details.  */
#include "cmMathCommand.h"

#include <cstdio>
#include <cstdlib>

#include <cm3p/kwiml/int.h>

#include "cmDiagnostics.h"
#include "cmExecutionStatus.h"
#include "cmExprParserHelper.h"
#include "cmMakefile.h"
#include "cmStringAlgorithms.h"
#include "cmValue.h"

namespace {
bool HandleExprCommand(std::vector<std::string> const& args,
                       cmExecutionStatus& status);
bool HandleIncDecCommand(std::vector<std::string> const& args,
                         cmExecutionStatus& status, int amount);
}

bool cmMathCommand(std::vector<std::string> const& args,
                   cmExecutionStatus& status)
{
  if (args.empty()) {
    status.SetError("must be called with at least one argument.");
    return false;
  }
  std::string const& subCommand = args[0];
  if (subCommand == "EXPR") {
    return HandleExprCommand(args, status);
  }
  if (subCommand == "INCREMENT") {
    return HandleIncDecCommand(args, status, 1);
  }
  if (subCommand == "DECREMENT") {
    return HandleIncDecCommand(args, status, -1);
  }
  std::string e = "does not recognize sub-command " + subCommand;
  status.SetError(e);
  return false;
}

namespace {
bool HandleExprCommand(std::vector<std::string> const& args,
                       cmExecutionStatus& status)
{
  if ((args.size() != 3) && (args.size() != 5)) {
    status.SetError("EXPR called with incorrect arguments.");
    return false;
  }

  enum class NumericFormat
  {
    UNINITIALIZED,
    DECIMAL,
    HEXADECIMAL,
  };

  std::string const& outputVariable = args[1];
  std::string const& expression = args[2];
  size_t argumentIndex = 3;
  NumericFormat outputFormat = NumericFormat::UNINITIALIZED;

  status.GetMakefile().AddDefinition(outputVariable, "ERROR");

  if (argumentIndex < args.size()) {
    std::string const messageHint = "sub-command EXPR ";
    std::string const& option = args[argumentIndex++];
    if (option == "OUTPUT_FORMAT") {
      if (argumentIndex < args.size()) {
        std::string const& argument = args[argumentIndex++];
        if (argument == "DECIMAL") {
          outputFormat = NumericFormat::DECIMAL;
        } else if (argument == "HEXADECIMAL") {
          outputFormat = NumericFormat::HEXADECIMAL;
        } else {
          std::string error = messageHint + "value \"" + argument +
            "\" for option \"" + option + "\" is invalid.";
          status.SetError(error);
          return false;
        }
      } else {
        std::string error =
          messageHint + "missing argument for option \"" + option + "\".";
        status.SetError(error);
        return false;
      }
    } else {
      std::string error =
        messageHint + "option \"" + option + "\" is unknown.";
      status.SetError(error);
      return false;
    }
  }

  if (outputFormat == NumericFormat::UNINITIALIZED) {
    outputFormat = NumericFormat::DECIMAL;
  }

  cmExprParserHelper helper;
  if (!helper.ParseString(expression.c_str(), 0)) {
    status.SetError(helper.GetError());
    return false;
  }

  char buffer[1024];
  char const* fmt;
  switch (outputFormat) {
    case NumericFormat::HEXADECIMAL:
      fmt = "0x%" KWIML_INT_PRIx64;
      break;
    case NumericFormat::DECIMAL:
      CM_FALLTHROUGH;
    default:
      fmt = "%" KWIML_INT_PRId64;
      break;
  }
  snprintf(buffer, sizeof(buffer), fmt, helper.GetResult());

  std::string const& w = helper.GetWarning();
  if (!w.empty()) {
    status.GetMakefile().IssueDiagnostic(cmDiagnostics::CMD_AUTHOR, w);
  }

  status.GetMakefile().AddDefinition(outputVariable, buffer);
  return true;
}

bool HandleIncDecCommand(std::vector<std::string> const& args,
                         cmExecutionStatus& status, int amount)
{
  std::string const messageHint = cmStrCat("sub-command ", args[0], " ");
  if (args.size() != 2) {
    status.SetError(cmStrCat(messageHint, "wrong number of arguments"));
    return false;
  }
  auto value = status.GetMakefile().GetDefinition(args[1]);
  if (!value) {
    status.SetError(
      cmStrCat(messageHint, "variable \"", args[1], "\" is not defined"));
    return false;
  }
  if (value->empty()) {
    status.SetError(
      cmStrCat(messageHint, "value \"\" is not a valid integer"));
    return false;
  }
  char* end;
  auto intValue = std::strtoll(value->c_str(), &end, 10);
  if (end != &*value->end()) {
    status.SetError(
      cmStrCat(messageHint, "value \"", *value, "\" is not a valid integer"));
    return false;
  }
  status.GetMakefile().AddDefinition(args[1],
                                     std::to_string(intValue + amount));
  return true;
}
}
