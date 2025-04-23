/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file LICENSE.rst or https://cmake.org/licensing for details.  */
#pragma once

#include <ostream>
#include <string>
#include <utility>

namespace cmSbom {
class Document;
}

/// @brief Base class for generators that produce serialized software bill of
/// materials (SBOM) documents
class cmSbomDocumentGenerator
{
public:
  cmSbomDocumentGenerator(std::string outputFormat)
    : OutputFormat(std::move(outputFormat))
  {
  }
  virtual ~cmSbomDocumentGenerator() = default;

protected:
  /// @brief Serialize the SBOM document to the stream in the specified format
  bool WriteSbom(std::ostream& os, cmSbom::Document const& doc);

  /// @brief Serialize the SBOM document to the stream in SPDX format
  bool WriteSpdxSbom(std::ostream& os, cmSbom::Document const& doc);

private:
  std::string OutputFormat;
};
