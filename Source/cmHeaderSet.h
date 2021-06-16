/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#pragma once

#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "cmListFileCache.h"

class cmCompiledGeneratorExpression;
struct cmGeneratorExpressionDAGChecker;
class cmGeneratorTarget;
class cmLocalGenerator;

class cmHeaderSet
{
public:
  cmHeaderSet(std::string name);

  const std::string& GetName() const { return this->Name; }

  const BT<std::string>& GetDirectory() const { return this->Directory; }
  void SetDirectory(BT<std::string> directory)
  {
    this->Directory = std::move(directory);
  }

  void ClearHeaderEntries();
  void AddHeaderEntry(BT<std::string> headers);
  const std::vector<BT<std::string>>& GetHeaderEntries() const
  {
    return this->HeaderEntries;
  }

  std::vector<std::unique_ptr<cmCompiledGeneratorExpression>>
  CompileHeaderEntries() const;

  std::unique_ptr<cmCompiledGeneratorExpression> CompileDirectory() const;

  const std::string& EvaluateDirectory(
    const std::unique_ptr<cmCompiledGeneratorExpression>& cge,
    cmLocalGenerator* lg, const std::string& config,
    const cmGeneratorTarget* target,
    cmGeneratorExpressionDAGChecker* dagChecker = nullptr) const;

  void EvaluateHeaderEntry(
    const std::string& dir,
    std::map<std::string, std::vector<std::string>>& filesPerDir,
    const std::unique_ptr<cmCompiledGeneratorExpression>& cge,
    cmLocalGenerator* lg, const std::string& config,
    const cmGeneratorTarget* target,
    cmGeneratorExpressionDAGChecker* dagChecker = nullptr) const;

private:
  std::string Name;
  BT<std::string> Directory;
  std::vector<BT<std::string>> HeaderEntries;
};
