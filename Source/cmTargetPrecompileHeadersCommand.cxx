/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#include "cmTargetPrecompileHeadersCommand.h"

#include <sstream>

#include "cmAlgorithms.h"
#include "cmMakefile.h"
#include "cmTarget.h"

bool cmTargetPrecompileHeadersCommand::InitialPass(
  std::vector<std::string> const& args, cmExecutionStatus&)
{
  return this->HandleArguments(args, "PRECOMPILE_HEADERS");
}

void cmTargetPrecompileHeadersCommand::HandleImportedTarget(
  const std::string& tgt)
{
  std::ostringstream e;
  e << "Cannot specify precompile headers for imported target \"" << tgt
    << "\".";
  this->Makefile->IssueMessage(cmake::FATAL_ERROR, e.str());
}

void cmTargetPrecompileHeadersCommand::HandleMissingTarget(
  const std::string& name)
{
  std::ostringstream e;
  e << "Cannot specify precompile headers for target \"" << name
    << "\" "
       "which is not built by this project.";
  this->Makefile->IssueMessage(cmake::FATAL_ERROR, e.str());
}

std::string cmTargetPrecompileHeadersCommand::Join(
  const std::vector<std::string>& content)
{
  return cmJoin(content, ";");
}

bool cmTargetPrecompileHeadersCommand::HandleDirectContent(
  cmTarget* tgt, const std::vector<std::string>& content, bool, bool)
{
  tgt->AppendProperty("PRECOMPILE_HEADERS", this->Join(content).c_str());
  return true;
}
