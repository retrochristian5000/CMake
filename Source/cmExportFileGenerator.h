/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#pragma once

#include "cmConfigure.h" // IWYU pragma: keep

#include <iosfwd>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "cmExportUtility.h"
#include "cmGeneratorExpression.h"
#include "cmStateTypes.h"
#include "cmVersion.h"
#include "cmVersionConfig.h"

class cmGeneratorTarget;
class cmListFileBacktrace;

#define STRINGIFY_HELPER(X) #X
#define STRINGIFY(X) STRINGIFY_HELPER(X)

#define DEVEL_CMAKE_VERSION(major, minor)                                     \
  (CMake_VERSION_ENCODE(major, minor, 0) >                                    \
       CMake_VERSION_ENCODE(CMake_VERSION_MAJOR, CMake_VERSION_MINOR, 0)      \
     ? STRINGIFY(CMake_VERSION_MAJOR) "." STRINGIFY(                          \
         CMake_VERSION_MINOR) "." STRINGIFY(CMake_VERSION_PATCH)              \
     : #major "." #minor ".0")

/** \class cmExportFileGenerator
 * \brief Generate a file exporting targets from a build or install tree.
 *
 * cmExportFileGenerator is the superclass for
 * cmExportBuildFileGenerator and cmExportInstallFileGenerator.  It
 * contains common code generation routines for the two kinds of
 * export implementations.
 */
class cmExportFileGenerator
{
public:
  cmExportFileGenerator();
  virtual ~cmExportFileGenerator() = default;

  /** Set the full path to the export file to generate.  */
  void SetExportFile(const char* mainFile);
  const std::string& GetMainExportFileName() const;

  /** Set the namespace in which to place exported target names.  */
  void SetNamespace(const std::string& ns) { this->Namespace = ns; }
  std::string GetNamespace() const { return this->Namespace; }

  void SetExportOld(bool exportOld) { this->ExportOld = exportOld; }

  /** Add a configuration to be exported.  */
  void AddConfiguration(const std::string& config);

  /** Actually generate the export file.  Returns whether there was an
      error.  */
  bool GenerateImportFile();

protected:
  using ImportPropertyMap = Exporting::PropertyMap;
  struct Context
  {
    // The set of targets included in the export.
    std::vector<cmGeneratorTarget const*> ExportedTargets;
    std::vector<std::string> MissingTargets;
  };

  // Generate per-configuration target information to the given output
  // stream.
  void GenerateImportConfig(std::ostream& os, std::string const& config,
                            Context& ctx);

  // Methods to implement export file code generation.

  virtual void GenerateImportTargetCode(
    std::ostream& os, cmGeneratorTarget const* target,
    cmStateEnums::TargetType targetType) = 0;

  // Collect properties with detailed information about targets beyond
  // their location on disk.
  void SetImportDetailProperties(const std::string& config,
                                 std::string const& suffix,
                                 cmGeneratorTarget const* target,
                                 ImportPropertyMap& properties, Context& ctx);

  enum class ImportLinkPropertyTargetNames
  {
    Yes,
    No,
  };
  template <typename T>
  void SetImportLinkProperty(std::string const& suffix,
                             cmGeneratorTarget const* target,
                             const std::string& propName,
                             std::vector<T> const& entries,
                             ImportPropertyMap& properties, Context& ctx,
                             ImportLinkPropertyTargetNames targetNames);

  /** Each subclass knows how to generate its kind of export file.  */
  virtual bool GenerateMainFile(std::ostream& os) = 0;

  /** Each subclass knows where the target files are located.  */
  virtual void GenerateImportTargetsConfig(std::ostream& os,
                                           std::string const& config,
                                           Context& ctx,
                                           std::string const& suffix) = 0;

  /** Each subclass knows how to deal with a target that is  missing from an
   *  export set.  */
  virtual void HandleMissingTarget(
    std::string& link_libs, std::vector<std::string>& missingTargets,
    cmGeneratorTarget const* depender,
    cmGeneratorTarget const* dependee) const = 0;

  virtual void ComplainAboutMissingTarget(
    cmGeneratorTarget const* depender, cmGeneratorTarget const* dependee,
    std::vector<std::string> const& exportFiles) const;

  void PopulateInterfaceProperty(const std::string&,
                                 cmGeneratorTarget const* target,
                                 cmGeneratorExpression::PreprocessContext,
                                 ImportPropertyMap& properties,
                                 Context& ctx) const;
  bool PopulateInterfaceLinkLibrariesProperty(
    const cmGeneratorTarget* target, cmGeneratorExpression::PreprocessContext,
    ImportPropertyMap& properties, Context& ctx) const;
  void PopulateInterfaceProperty(const std::string& propName,
                                 const cmGeneratorTarget* target,
                                 ImportPropertyMap& properties) const;
  void PopulateCompatibleInterfaceProperties(
    const cmGeneratorTarget* target, ImportPropertyMap& properties) const;
  virtual void GenerateInterfaceProperties(
    cmGeneratorTarget const* target, std::ostream& os,
    const ImportPropertyMap& properties) const;
  void PopulateIncludeDirectoriesInterface(
    cmGeneratorTarget const* target, std::string additionalIncludeDirectories,
    cmGeneratorExpression::PreprocessContext preprocessRule,
    ImportPropertyMap& properties,
    std::vector<std::string>& missingTargets) const;
  void PopulateIncludeDirectoriesInterface(
    cmGeneratorTarget const* target,
    cmGeneratorExpression::PreprocessContext preprocessRule,
    ImportPropertyMap& properties, Context& ctx) const;
  void PopulateSourcesInterface(
    cmGeneratorTarget const* target,
    cmGeneratorExpression::PreprocessContext preprocessRule,
    ImportPropertyMap& properties, Context& ctx) const;
  void PopulateLinkDirectoriesInterface(
    cmGeneratorTarget const* target,
    cmGeneratorExpression::PreprocessContext preprocessRule,
    ImportPropertyMap& properties, Context& ctx) const;
  void PopulateLinkDependsInterface(
    cmGeneratorTarget const* target,
    cmGeneratorExpression::PreprocessContext preprocessRule,
    ImportPropertyMap& properties, Context& ctx) const;

  void SetImportLinkInterface(
    const std::string& config, std::string const& suffix,
    cmGeneratorExpression::PreprocessContext preprocessRule,
    cmGeneratorTarget const* target, ImportPropertyMap& properties,
    Context& ctx);

  enum FreeTargetsReplace
  {
    ReplaceFreeTargets,
    NoReplaceFreeTargets
  };

  void ResolveTargetsInGeneratorExpressions(
    std::string& input, cmGeneratorTarget const* target, Context& ctx,
    FreeTargetsReplace replace = NoReplaceFreeTargets) const;

  bool PopulateExportProperties(cmGeneratorTarget const* gte,
                                ImportPropertyMap& properties,
                                std::string& errorMessage) const;

  ImportPropertyMap CommonInterfaceProperties(
    cmGeneratorTarget const* target,
    cmGeneratorExpression::PreprocessContext preprocessRule, Context& ctx,
    bool includeInterfaceLinkLibraries) const;

  virtual Context* MakeAndSetupContext(Context* context = nullptr) const = 0;

  template <class T>
  T* NewOrPreCreated(Context* preCreated) const
  {
    return preCreated ? static_cast<T*>(preCreated) : new T();
  }

  // Install generators should use backtraces from from their
  // cmInstallExportGenerator instances
  virtual cmListFileBacktrace GetBacktrace(cmMakefile const& mf) const;

  // The namespace in which the exports are placed in the generated file.
  std::string Namespace;

  bool ExportOld;

  // The set of configurations to export.
  std::vector<std::string> Configurations;

  // The file to generate.
  std::string MainImportFile;
  std::string FileDir;
  std::string FileBase;
  std::string FileExt;
  bool AppendMode;

private:
  void PopulateInterfaceProperty(const std::string&, const std::string&,
                                 cmGeneratorTarget const* target,
                                 cmGeneratorExpression::PreprocessContext,
                                 ImportPropertyMap& properties,
                                 Context& ctx) const;

  void PopulateCommonInterfaceProperties(
    cmGeneratorTarget const* target, ImportPropertyMap& properties,
    cmGeneratorExpression::PreprocessContext preprocessRule, Context& ctx,
    bool includeInterfaceLinkLibraries) const;

  bool AddTargetNamespace(std::string& input, cmGeneratorTarget const* target,
                          Context& ctx) const;

  void ResolveTargetsInGeneratorExpression(std::string& input,
                                           cmGeneratorTarget const* target,
                                           Context& ctx) const;

  virtual void ReplaceInstallPrefix(std::string& input) const;

  virtual std::string InstallNameDir(cmGeneratorTarget const* target,
                                     const std::string& config) = 0;

  virtual void InvocationSyntax(std::ostream& os) const = 0;

  // the set of target pairs we already complained about during this run
  // we do not want to repeat ourselves
  mutable std::map<cmGeneratorTarget const*,
                   std::set<cmGeneratorTarget const*>>
    ComplainedTargets;
};
