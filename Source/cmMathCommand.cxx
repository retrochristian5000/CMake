/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#include "cmMathCommand.h"

#include "cmExprParserHelper.h"
#include "cmMakefile.h"
#include "cm_kwiml.h"

#include <stdexcept>
#include <stdio.h>

class cmExecutionStatus;

bool cmMathCommand::InitialPass(std::vector<std::string> const& args,
                                cmExecutionStatus&)
{
  if (args.empty()) {
    this->SetError("must be called with at least one argument.");
    return false;
  }
  const std::string& subCommand = args[0];
  if (subCommand == "EXPR") {
    return this->HandleExprCommand(args);
  }
  std::string e = "does not recognize sub-command " + subCommand;
  this->SetError(e);
  return false;
}

bool cmMathCommand::HandleExprCommand(std::vector<std::string> const& args)
{
  if (args.size() != 3) {
    this->SetError("EXPR called with incorrect arguments.");
    return false;
  }

  const std::string& outputVariable = args[1];
  const std::string& expression = args[2];

  this->Makefile->AddDefinition(outputVariable, "ERROR");

  cmExprParserHelper helper;
  bool parsed = false;
  try {
    parsed = helper.ParseString(expression.c_str(), 0) == 1;
  } catch (const std::runtime_error& fail) {
    std::string e = "cannot evaluate the expression: \"" + expression + "\": ";
    e += fail.what();
    e += ".";
    this->SetError(e);
    return false;
  } catch (const std::out_of_range& fail) {
    std::string e = "cannot parse the expression: \"" + expression +
      "\": a numeric value is out of range.";
    this->SetError(e);
    return false;
  } catch (...) {
    std::string e = "cannot parse the expression: \"" + expression + "\".";
    this->SetError(e);
    return false;
  }
  if (!parsed) {
    std::string e = "cannot parse the expression: \"" + expression + "\": ";
    e += helper.GetError();
    e += ".";
    this->SetError(e);
    return false;
  }

  char buffer[1024];
  sprintf(buffer, "%" KWIML_INT_PRId64, helper.GetResult());

  this->Makefile->AddDefinition(outputVariable, buffer);
  return true;
}
