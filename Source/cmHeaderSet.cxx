/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#include "cmHeaderSet.h"

#include <string>
#include <utility>
#include <vector>

#include "cmGeneratorExpression.h"
#include "cmListFileCache.h"
#include "cmLocalGenerator.h"
#include "cmMessageType.h"
#include "cmStringAlgorithms.h"
#include "cmSystemTools.h"
#include "cmake.h"

cmHeaderSet::cmHeaderSet(std::string name)
  : Name(std::move(name))
{
}

void cmHeaderSet::AddHeaderEntry(BT<std::string> headers)
{
  this->HeaderEntries.push_back(std::move(headers));
}

void cmHeaderSet::ClearHeaderEntries()
{
  this->HeaderEntries.clear();
}

std::vector<std::unique_ptr<cmCompiledGeneratorExpression>>
cmHeaderSet::CompileHeaderEntries() const
{
  std::vector<std::unique_ptr<cmCompiledGeneratorExpression>> result;

  for (auto const& entry : this->HeaderEntries) {
    for (auto const& ex : cmExpandedList(entry.Value)) {
      cmGeneratorExpression ge(entry.Backtrace);
      auto cge = ge.Parse(ex);
      result.push_back(std::move(cge));
    }
  }

  return result;
}

std::unique_ptr<cmCompiledGeneratorExpression> cmHeaderSet::CompileDirectory()
  const
{
  cmGeneratorExpression ge(this->Directory.Backtrace);
  return ge.Parse(this->Directory.Value);
}

const std::string& cmHeaderSet::EvaluateDirectory(
  const std::unique_ptr<cmCompiledGeneratorExpression>& cge,
  cmLocalGenerator* lg, const std::string& config,
  const cmGeneratorTarget* target,
  cmGeneratorExpressionDAGChecker* dagChecker) const
{
  return cge->Evaluate(lg, config, target, dagChecker);
}

void cmHeaderSet::EvaluateHeaderEntry(
  const std::string& dir,
  std::map<std::string, std::vector<std::string>>& filesPerDir,
  const std::unique_ptr<cmCompiledGeneratorExpression>& cge,
  cmLocalGenerator* lg, const std::string& config,
  const cmGeneratorTarget* target,
  cmGeneratorExpressionDAGChecker* dagChecker) const
{
  auto files = cge->Evaluate(lg, config, target, dagChecker);
  std::vector<std::string> absFiles;
  for (auto const& file : cmExpandedList(files)) {
    if (cmSystemTools::FileIsFullPath(file)) {
      lg->GetCMakeInstance()->IssueMessage(
        MessageType::FATAL_ERROR, "Files in header set cannot be absolute",
        cge->GetBacktrace());
      return;
    }

    std::string relDir;
    auto slash = file.rfind('/');
    if (slash != std::string::npos) {
      relDir = file.substr(0, slash);
    }
    filesPerDir[relDir].push_back(cmStrCat(dir, '/', file));
  }
}
