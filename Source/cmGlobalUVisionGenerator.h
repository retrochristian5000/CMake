/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#ifndef cmGlobalUVisionGenerator_h
#define cmGlobalUVisionGenerator_h

#include "cmConfigure.h" // IWYU pragma: keep

#include "cmGlobalGenerator.h"
#include "cmGlobalGeneratorFactory.h"

class cmake;
struct cmDocumentationEntry;

#define UV_BUILD_EVENTS_GROUP ":BuildEvents"
#define UV_CMAKE_GROUP ":CMake"
#define UV_REFERENCES_GROUP ":References"
#define UV_LIBRARIES_GROUP "Libraries"
#define UV_SCATTER_FILES_GROUP "Scatter Files"

/** \class cmGlobalUVisionGenerator
 * \brief Global generator for uVision projects
 */
class cmGlobalUVisionGenerator : public cmGlobalGenerator
{
public:
  cmGlobalUVisionGenerator(cmake* cm);

  static std::unique_ptr<cmGlobalGeneratorFactory> NewFactory()
  {
    return std::make_unique<
      cmGlobalGeneratorSimpleFactory<cmGlobalUVisionGenerator>>();
  }

  ~cmGlobalUVisionGenerator() override;

  std::unique_ptr<cmLocalGenerator> CreateLocalGenerator(
    cmMakefile* mf) override;

  void Generate() override;

  std::string GetName() const override
  {
    return cmGlobalUVisionGenerator::GetActualName();
  }

  static std::string GetActualName() { return "uVision"; }

  static void GetDocumentation(cmDocumentationEntry& entry);

  bool SetGeneratorInstance(std::string const& i, cmMakefile* mf) override;

  /**
   * Utilized by the generator factory to determine if this generator
   * supports toolsets.
   */
  static bool SupportsToolset() { return true; }

  /**
   * Utilized by the generator factory to determine if this generator
   * supports platforms.
   */
  static bool SupportsPlatform() { return false; }

  bool IsMultiConfig() const override { return true; }

  /** Append the sub directory for the given configuration.  */
  void AppendDirectoryForConfig(std::string const& prefix,
                                std::string const& config,
                                std::string const& suffix,
                                std::string& dir) override;

  bool SetGeneratorToolset(std::string const& ts, bool build,
                           cmMakefile* mf) override;

  std::string GetToolsetVersionString(cmMakefile* mf) const;

  void EnableLanguage(std::vector<std::string> const& l, cmMakefile* mf,
                      bool optional) override;

  std::string GetProjectNameForDirectory(std::string const& projectName,
                                         std::string const& projectDir) const;

  std::vector<cmGlobalGenerator::GeneratedMakeCommand> GenerateBuildCommand(
    std::string const& makeProgram, std::string const& projectName,
    std::string const& projectDir, std::vector<std::string> const& targetNames,
    std::string const& config, bool fast, int jobs, bool verbose,
    std::vector<std::string> const& makeOptions) override;

  bool Open(std::string const& binDir, std::string const& projectName,
            bool dryRun) override;

private:
  void CleanRTE(cmLocalGenerator* root);

  void GenerateMultiProjectWorkspace(
    cmLocalGenerator* root, std::vector<cmLocalGenerator*>& generators);

  bool ComputeTargetBuildOrder(cmGeneratorTarget const* tgt,
                               std::vector<cmGeneratorTarget const*>& build);
  bool ComputeTargetBuildOrder(std::vector<cmGeneratorTarget const*>& tgt,
                               std::vector<cmGeneratorTarget const*>& build);
  bool VisitTarget(std::set<cmGeneratorTarget const*>& temp,
                   std::set<cmGeneratorTarget const*>& perm,
                   std::vector<cmGeneratorTarget const*>& order,
                   cmGeneratorTarget const* ti);

  // Target sorting
  class TargetSet : public std::set<cmGeneratorTarget const*>
  {
  };
  class TargetCompare
  {
    std::string First;

  public:
    TargetCompare(std::string first)
      : First(std::move(first))
    {
    }
    bool operator()(cmGeneratorTarget const* l,
                    cmGeneratorTarget const* r) const;
  };
  class OrderedTargetDependSet;
};

class cmGlobalUVisionGenerator::OrderedTargetDependSet
  : public std::multiset<cmTargetDepend,
                         cmGlobalUVisionGenerator::TargetCompare>
{
  using derived =
    std::multiset<cmTargetDepend, cmGlobalUVisionGenerator::TargetCompare>;

public:
  using TargetDependSet = cmGlobalGenerator::TargetDependSet;
  OrderedTargetDependSet(TargetDependSet const&, std::string const& first);
};

#endif
