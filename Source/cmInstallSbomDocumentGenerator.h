/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file LICENSE.rst or https://cmake.org/licensing for details.  */
#pragma once

#include <iosfwd>
#include <memory>
#include <string>
#include <vector>

#include "cmInstallGenerator.h"
#include "cmSbomDocumentGenerator.h"

class cmListFileBacktrace;
class cmLocalGenerator;
namespace cmSbom {
class Document;
}

/// @brief Directs generation of an SBOM document based on a CMake project's
/// installation rules
///
/// The SBOM this generator produces describes the artifacts of a user project
/// installed by CMake. It is specific to the selected configuration and
/// potentially a component.
class cmInstallSbomDocumentGenerator
  : public cmInstallGenerator
  , public cmSbomDocumentGenerator
{
public:
  cmInstallSbomDocumentGenerator(
    std::string output_format, std::string destination,
    std::vector<std::string> const& configurations, std::string component,
    MessageLevel message, bool exclude_from_all, bool all_components,
    cmListFileBacktrace backtrace);

  ~cmInstallSbomDocumentGenerator() override = default;

  bool Compute(cmLocalGenerator* lg) override;

protected:
  void GenerateScriptForConfig(std::ostream& os, std::string const& config,
                               Indent indent) override;

private:
  cmSbom::Document BuildSbomForInstallConfig(
    std::string const& config, bool allComponents,
    std::vector<std::unique_ptr<cmInstallGenerator>> const& gens);

  bool GenerateSbomInstaller(std::ostream& os, cmSbom::Document& doc,
                             std::string const& dest, Indent indent);

  void ReportIncomplete();

  cmLocalGenerator* LocalGenerator = nullptr;
  std::string Config;
  bool SbomBuildIncomplete = false;
};
