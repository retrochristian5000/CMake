/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#ifndef cmUVisionTargetGenerator_h
#define cmUVisionTargetGenerator_h

#include "cmConfigure.h" // IWYU pragma: keep

#include "cmGeneratorTarget.h"
#include "cmXMLEditor.h"

class cmGlobalUVisionGenerator;
class cmLocalUVisionGenerator;
class cmMakefile;
class cmSourceGroup;
class cmUVisionGeneratorOptions;

using cmGroupedAllConfigSources =
  std::map<std::string, std::vector<cmGeneratorTarget::AllConfigSource>>;

struct cmUVRTEPackage
{
  std::string Name;
  std::string Vendor;
  std::string Version;
  std::set<size_t> Configs;
};

struct cmUVRTEComponent
{
  std::string Vendor;
  std::string Bundle;
  std::string Class;
  std::string Group;
  std::string SubGroup;
  std::string Variant;
  std::string Version;
  cmUVRTEPackage* Package;
  std::set<size_t> Configs;
};

/** \class cmUVisionTargetGenerator
 * \brief Generate a uVision project file
 */
class cmUVisionTargetGenerator
{
public:
  cmUVisionTargetGenerator(cmGeneratorTarget* target);

  ~cmUVisionTargetGenerator();

  void Generate();

private:
  cmGeneratorTarget* Target;
  cmLocalUVisionGenerator* LocalGenerator;
  cmGlobalUVisionGenerator* GlobalGenerator;
  std::string const Name;
  cmMakefile* Makefile;
  std::vector<std::string> Configs;
  std::string ProjectFile;
  std::string OptionsFile;
  std::map<std::string, std::string> OptionsValues;
  std::string BinaryDirectory;
  std::string TmpDirectory;
  std::vector<cmSourceGroup> SourceGroups;
  cmGroupedAllConfigSources GroupedSources;
  bool IsXProject = false;

  void ComputeTargetSources();

  bool GenerateProjectFile();

  int GenerateUVisionProject(std::string const& targetDevice);

  bool GenerateOptionsFile();

  void SetToolset(cmXMLEditorElement* targetElement) const;

  void SetTargetOptions(std::string const& config,
                        cmXMLEditorElement* projectTarget);

  void SetBuildEvents(cmXMLEditorElement* commonOption,
                      std::string const& config);

  void SetBuildEvent(std::string const& buildEvent,
                     cmXMLEditorElement* uvBuildEvent,
                     std::string const& config,
                     std::vector<cmCustomCommand> const& commands);

  std::string GenerateCustomCommandBatchFile(
    std::vector<cmCustomCommand> const& commands, std::string const& config,
    std::string const& file) const;

  void AddConfigSourceWithDisplayNameAndGroup(
    std::string const& fileName, cmGeneratorTarget::SourceKind sourceKind,
    std::string const& displayName, std::string const& config,
    std::string const& group);

  void GenerateSourceGroups(cmXMLEditorElement* targetElement,
                            size_t configIndex,
                            cmUVisionGeneratorOptions* generatorOptions);

  void GenerateSourceGroup(cmXMLEditorElement* groupsElement,
                           std::string const& sourceGroup, size_t configIndex,
                           cmUVisionGeneratorOptions* generatorOptions);

  void GenerateRTEs(cmXMLEditorElement* projectElement);

  std::string GetUVFileType(cmGeneratorTarget::AllConfigSource& sourceFile);

  void GenerateComponents(cmXMLEditorElement* rteElement,
                          std::vector<cmUVRTEComponent>& components);

  void GeneratePackages(cmXMLEditorElement* rteElement,
                        std::list<cmUVRTEPackage>& packages);

  void ProcessRTETarget(const cmGeneratorTarget* target,
                        const std::vector<size_t>* configs,
                        std::vector<cmUVRTEComponent>* components,
                        std::list<cmUVRTEPackage>* packages);
};

#endif