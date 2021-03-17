/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#include "cmLocalUVisionGenerator.h"

#include "cmGeneratedFileStream.h"
#include "cmMakefile.h"
#include "cmUVisionTargetGenerator.h"

class cmGlobalGenerator;
class cmXMLWriter;
class cmGeneratedFileStream;
class cmComputeLinkInformation;

cmLocalUVisionGenerator::cmLocalUVisionGenerator(cmGlobalGenerator* gg,
                                                 cmMakefile* mf)
  : cmLocalGenerator(gg, mf)
{
}

cmLocalUVisionGenerator::~cmLocalUVisionGenerator() = default;

void cmLocalUVisionGenerator::Generate()
{
  std::vector<std::string> configs =
    this->GetMakefile()->GetGeneratorConfigs(cmMakefile::OnlyMultiConfig);
  if (configs.empty()) {
    cmSystemTools::Error("No configurations set.");
    return;
  }

  auto const& targets = this->GetGeneratorTargets();
  for (auto const& target : targets) {
    cmUVisionTargetGenerator targetGenerator(target.get());
    targetGenerator.Generate();
  }
}
