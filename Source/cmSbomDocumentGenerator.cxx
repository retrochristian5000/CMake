/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file LICENSE.rst or https://cmake.org/licensing for details.  */
#include "cmSbomDocumentGenerator.h"

#include <cstddef>
#include <memory>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "cmSbom.h"

#include "cm3p/json/value.h"
#include "cm3p/json/writer.h"

namespace {

class SpdxSerializer : public cmSbom::Document::Serializer
{
public:
  SpdxSerializer() = default;
  ~SpdxSerializer() override = default;
  SpdxSerializer(SpdxSerializer const&) = delete;
  SpdxSerializer& operator=(SpdxSerializer const&) = delete;

  bool Serialize(
    cmSbom::Document const& doc,
    std::vector<std::unique_ptr<cmSbom::Element>> const& all_elements,
    std::ostream& os) override;

private:
  std::string const& GetOrAssignId(cmSbom::Element const* element,
                                   std::string const& idName = "element");

  std::unordered_map<cmSbom::Element const*, std::string> AssignedIds;
  std::size_t NextId = 1;
};

std::string const& SpdxSerializer::GetOrAssignId(
  cmSbom::Element const* element, std::string const& idName)
{
  // Check if the element already has an assigned ID
  auto it = this->AssignedIds.find(element);
  if (it != this->AssignedIds.end()) {
    return it->second;
  }

  // If the element does not have an ID, assign one
  std::string id = "_:" + idName + "_" + std::to_string(this->NextId++);
  this->AssignedIds[element] = id;
  return this->AssignedIds[element];
}

bool SpdxSerializer::Serialize(
  cmSbom::Document const& doc,
  std::vector<std::unique_ptr<cmSbom::Element>> const& all_elements,
  std::ostream& os)
{
  Json::Value root(Json::objectValue);
  root["@context"] = "https://spdx.org/rdf/3.0.1/spdx-context.jsonld";

  auto generateSpdxElement = [&](cmSbom::Element const* element) {
    // Create the base element
    Json::Value spdxElement(Json::objectValue);
    spdxElement["type"] = "Element";
    spdxElement["creationInfo"] = "_:creationInfo_0";
    if (!element->GetName().empty()) {
      spdxElement["name"] = element->GetName();
    }

    // Add specialized properties based on the element type
    if (auto const* relationship =
          dynamic_cast<cmSbom::Relationship const*>(element)) {

      spdxElement["type"] = "Relationship";
      spdxElement["relationshipType"] =
        cmSbom::RelationshipTypeToString(relationship->GetType()).data();
      spdxElement["from"] =
        this->GetOrAssignId(relationship->GetSourceElement());

      Json::Value to(Json::arrayValue);
      for (auto const& target : relationship->GetTargetElements()) {
        to.append(this->GetOrAssignId(target));
      }
      spdxElement["to"] = to;
    } else if (auto const* package =
                 dynamic_cast<cmSbom::Package const*>(element)) {
      static_cast<void>(package);
      spdxElement["type"] = "software_Package";
    } else if (auto const* file = dynamic_cast<cmSbom::File const*>(element)) {
      spdxElement["type"] = "software_File";
      spdxElement["fileType"] =
        file->GetKind() == cmSbom::File::FileKind::File ? "file" : "directory";
    } else if (auto const* document =
                 dynamic_cast<cmSbom::Document const*>(element)) {
      spdxElement["type"] = "SpdxDocument";

      Json::Value roots(Json::arrayValue);
      for (auto const& docRoot : document->GetRootElements()) {
        roots.append(this->GetOrAssignId(docRoot));
      }
    }
    spdxElement["spdxId"] =
      this->GetOrAssignId(element, spdxElement["type"].asString());

    return spdxElement;
  };

  Json::Value graph(Json::arrayValue);

  // Add one creation info to note CMake as the creator of data in the SBOM
  Json::Value creationInfo(Json::objectValue);
  creationInfo["@id"] = "_:creationInfo_0";
  creationInfo["type"] = "CreationInfo";
  creationInfo["specVersion"] = "3.0.1";
  creationInfo["created"] = "@CMAKE_SBOM_GENERATOR_TIMESTAMP@";
  Json::Value creators(Json::arrayValue);
  creators.append(Json::Value("_:cmake_agent_0"));
  creationInfo["createdBy"] = creators;
  graph.append(creationInfo);

  Json::Value cmakeAgent(Json::objectValue);
  cmakeAgent["creationInfo"] = "_:creationInfo_0";
  cmakeAgent["spdxId"] = "_:cmake_agent_0";
  cmakeAgent["type"] = "Agent";
  cmakeAgent["name"] = "CMake";
  graph.append(cmakeAgent);

  graph.append(generateSpdxElement(&doc));

  for (auto const& element : all_elements) {
    Json::Value spdxElement = generateSpdxElement(element.get());
    graph.append(spdxElement);
  }
  root["@graph"] = graph;

  // Write the JSON to the output stream
  Json::StreamWriterBuilder builder;
  std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
  return (0 == writer->write(root, &os));
}
} // namespace

bool cmSbomDocumentGenerator::WriteSbom(std::ostream& os,
                                        cmSbom::Document const& doc)
{
  if (this->OutputFormat == "SPDX") {
    return this->WriteSpdxSbom(os, doc);
  }
  return false;
}

bool cmSbomDocumentGenerator::WriteSpdxSbom(std::ostream& os,
                                            cmSbom::Document const& doc)
{
  SpdxSerializer serializer;
  return doc.Serialize(os, serializer);
}
