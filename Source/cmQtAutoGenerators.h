/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#ifndef cmQtAutoGenerators_h
#define cmQtAutoGenerators_h

#include "cmConfigure.h" // IWYU pragma: keep

#include "cmFilePathChecksum.h"
#include "cmQtAutoGen.h"
#include "cmsys/RegularExpression.hxx"

#include <map>
#include <memory> // IWYU pragma: keep
#include <set>
#include <string>
#include <vector>

class cmMakefile;

class cmQtAutoGenerators
{
  CM_DISABLE_COPY(cmQtAutoGenerators)
public:
  cmQtAutoGenerators();
  bool Run(const std::string& targetDirectory, const std::string& config);

private:
  // -- Types

  /// @brief Search key plus regular expression pair
  struct KeyRegExp
  {
    KeyRegExp() = default;

    KeyRegExp(const char* key, const char* regExp)
      : Key(key)
      , RegExp(regExp)
    {
    }

    KeyRegExp(const std::string& key, const std::string& regExp)
      : Key(key)
      , RegExp(regExp)
    {
    }

    std::string Key;
    cmsys::RegularExpression RegExp;
  };

  /// @brief Source file job
  struct SourceJob
  {
    bool Moc = false;
    bool Uic = false;
  };

  /// @brief MOC job
  struct MocJobAuto
  {
    std::string SourceFile;
    std::string BuildFileRel;
    std::set<std::string> Depends;
  };

  /// @brief MOC job
  struct MocJobIncluded : MocJobAuto
  {
    bool DependsValid = false;
    std::string Includer;
    std::string IncludeString;
  };

  /// @brief UIC job
  struct UicJob
  {
    std::string SourceFile;
    std::string BuildFileRel;
    std::string Includer;
    std::string IncludeString;
  };

  /// @brief RCC job
  struct RccJob
  {
    std::string QrcFile;
    std::string RccFile;
    std::vector<std::string> Options;
    std::vector<std::string> Inputs;
  };

  // -- Initialization
  bool InitInfoFile(cmMakefile* makefile, const std::string& targetDirectory,
                    const std::string& config);

  // -- Settings file
  void SettingsFileRead(cmMakefile* makefile);
  bool SettingsFileWrite();
  bool SettingsChanged() const
  {
    return (this->MocSettingsChanged || this->RccSettingsChanged ||
            this->UicSettingsChanged);
  }

  // -- Central processing
  bool Process();

  // -- Source parsing
  bool ParseSourceFile(const std::string& absFilename, const SourceJob& job);
  bool ParseHeaderFile(const std::string& absFilename, const SourceJob& job);
  bool ParsePostprocess();

  // -- Moc
  bool MocEnabled() const { return !this->MocExecutable.empty(); }
  bool MocSkip(const std::string& absFilename) const;
  bool MocRequired(const std::string& contentText,
                   std::string* macroName = nullptr);
  // Moc strings
  std::string MocStringMacros() const;
  std::string MocStringHeaders(const std::string& fileBase) const;
  std::string MocFindIncludedHeader(const std::string& sourcePath,
                                    const std::string& includeBase) const;
  bool MocFindIncludedFile(std::string& absFile, const std::string& sourceFile,
                           const std::string& includeString) const;
  // Moc depends
  bool MocDependFilterPush(const std::string& key, const std::string& regExp);
  void MocFindDepends(const std::string& absFilename,
                      const std::string& contentText,
                      std::set<std::string>& depends);
  // Moc
  bool MocParseSourceContent(const std::string& absFilename,
                             const std::string& contentText);
  void MocParseHeaderContent(const std::string& absFilename,
                             const std::string& contentText);

  bool MocGenerateAll();
  bool MocGenerateFile(const MocJobAuto& job, bool* generated = nullptr);

  // -- Uic
  bool UicEnabled() const { return !this->UicExecutable.empty(); }
  bool UicSkip(const std::string& absFilename) const;
  bool UicParseContent(const std::string& fileName,
                       const std::string& contentText);
  bool UicFindIncludedFile(std::string& absFile, const std::string& sourceFile,
                           const std::string& includeString);
  bool UicGenerateAll();
  bool UicGenerateFile(const UicJob& uicJob);

  // -- Rcc
  bool RccEnabled() const { return !this->RccExecutable.empty(); }
  bool RccGenerateAll();
  bool RccGenerateFile(const RccJob& rccJob);

  // -- Log info
  void LogBold(const std::string& message) const;
  void LogInfo(cmQtAutoGen::GeneratorType genType,
               const std::string& message) const;
  // -- Log warning
  void LogWarning(cmQtAutoGen::GeneratorType genType,
                  const std::string& message) const;
  void LogFileWarning(cmQtAutoGen::GeneratorType genType,
                      const std::string& filename,
                      const std::string& message) const;
  // -- Log error
  void LogError(cmQtAutoGen::GeneratorType genType,
                const std::string& message) const;
  void LogFileError(cmQtAutoGen::GeneratorType genType,
                    const std::string& filename,
                    const std::string& message) const;
  void LogCommandError(cmQtAutoGen::GeneratorType genType,
                       const std::string& message,
                       const std::vector<std::string>& command,
                       const std::string& output) const;

  // -- Utility
  bool MakeParentDirectory(cmQtAutoGen::GeneratorType genType,
                           const std::string& filename) const;
  bool FileDiffers(const std::string& filename, const std::string& content);
  bool FileWrite(cmQtAutoGen::GeneratorType genType,
                 const std::string& filename, const std::string& content);
  bool FindHeader(std::string& header, const std::string& testBasePath) const;
  bool RunCommand(const std::vector<std::string>& command,
                  std::string& output) const;

  // -- Meta
  std::string ConfigSuffix;
  std::string InfoFile;
  // -- Settings
  bool IncludeProjectDirsBefore;
  bool Verbose;
  bool ColorOutput;
  std::string SettingsFile;
  std::string SettingsStringMoc;
  std::string SettingsStringUic;
  std::string SettingsStringRcc;
  // -- Directories
  std::string ProjectSourceDir;
  std::string ProjectBinaryDir;
  std::string CurrentSourceDir;
  std::string CurrentBinaryDir;
  std::string AutogenBuildDir;
  std::string AutogenIncludeDir;
  // -- Qt environment
  std::string QtMajorVersion;
  std::string QtMinorVersion;
  std::string MocExecutable;
  std::string UicExecutable;
  std::string RccExecutable;
  // -- File lists
  std::map<std::string, SourceJob> HeaderJobs;
  std::map<std::string, SourceJob> SourceJobs;
  std::vector<std::string> HeaderExtensions;
  cmFilePathChecksum FilePathChecksum;
  // -- Moc
  bool MocSettingsChanged;
  bool MocPredefsChanged;
  bool MocRelaxedMode;
  std::string MocCompFileRel;
  std::string MocCompFileAbs;
  std::string MocPredefsFileRel;
  std::string MocPredefsFileAbs;
  std::vector<std::string> MocSkipList;
  std::vector<std::string> MocIncludePaths;
  std::vector<std::string> MocIncludes;
  std::vector<std::string> MocDefinitions;
  std::vector<std::string> MocOptions;
  std::vector<std::string> MocAllOptions;
  std::vector<std::string> MocPredefsCmd;
  std::vector<KeyRegExp> MocDependFilters;
  std::vector<KeyRegExp> MocMacroFilters;
  cmsys::RegularExpression MocRegExpInclude;
  std::vector<std::unique_ptr<MocJobIncluded>> MocJobsIncluded;
  std::vector<std::unique_ptr<MocJobAuto>> MocJobsAuto;
  // -- Uic
  bool UicSettingsChanged;
  std::vector<std::string> UicSkipList;
  std::vector<std::string> UicTargetOptions;
  std::map<std::string, std::vector<std::string>> UicOptions;
  std::vector<std::string> UicSearchPaths;
  cmsys::RegularExpression UicRegExpInclude;
  std::vector<std::unique_ptr<UicJob>> UicJobs;
  // -- Rcc
  bool RccSettingsChanged;
  std::vector<RccJob> RccJobs;
};

#endif
