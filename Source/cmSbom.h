/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file LICENSE.rst or https://cmake.org/licensing for details.  */
#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include <cm/memory>
#include <cm/optional>
#include <cm/string_view>
#include <cm/type_traits>

#include "cmInstallGenerator.h"

/// @brief Object model for assembling software bill of materials (SBOM) docs
///
/// The CMake SBOM object model is based on the SPDX v3.0.1 specification and
/// is designed to represent semantic information about the creation and usage
/// of artifacts included in distributions of software.
///
/// See the [SPDX v3.0.1
/// Specification](https://spdx.github.io/spdx-spec/v3.0.1/) for more
/// information about the underlying model.
///
/// A CMake project may maintain several SBOMs (e.g. for a specific component
/// or configuration). For example, `cmInstallSbomDocumentGenerator` produces
/// an SBOM for a project installation. SBOM producers should derive from
/// `cmSbomDocumentGenerator` to support several underlying serialization
/// formats.
namespace cmSbom {

class Graph;

enum class RelationshipType
{
  Describes,
  Contains,
  DependsOn,
  Generates,
  Uses,
};

inline cm::string_view RelationshipTypeToString(RelationshipType type)
{
  switch (type) {
    case RelationshipType::Describes:
      return "describes";
    case RelationshipType::Contains:
      return "contains";
    case RelationshipType::DependsOn:
      return "dependsOn";
    case RelationshipType::Generates:
      return "generates";
    case RelationshipType::Uses:
      return "uses";
  }
  return "";
}

/// @brief A unit of information in the SBOM graph
class Element
{
public:
  /// @brief Create an element belonging to the given document
  Element(std::string name = "")
    : Name(std::move(name))
  {
  }
  virtual ~Element() = default;
  Element(Element&&) = default;

  Element(Element const&) = delete;
  Element& operator=(Element const&) = delete;

  /// @brief Get the name of the element (optional; may be empty)
  std::string const& GetName() const { return this->Name; }

  /// @brief Set the name of the element
  void SetName(std::string const& name) { this->Name = name; }

  Graph* GetParentGraph() const { return this->ParentGraph; }

private:
  Graph* ParentGraph = nullptr;
  /// @brief The optional name of the element
  std::string Name;

  void SetParentGraph(Graph* graph) { this->ParentGraph = graph; }
  friend class Graph;
};

/// @brief Manages a collection of SBOM elements and relationships between them
class Graph
{
public:
  Graph() = default;
  virtual ~Graph() = default;

  Graph(Graph&&) = default;
  Graph& operator=(Graph&&) = default;
  Graph(Graph const&) = delete;
  Graph& operator=(Graph const&) = delete;

  /// @brief Adds an element to the graph, taking ownership and returning a
  /// non-owning pointer
  template <typename T,
            typename = cm::enable_if_t<std::is_base_of<Element, T>::value>>
  T* AddElement(std::unique_ptr<T> element)
  {
    Elements.emplace_back(std::move(element));
    auto* newElem = static_cast<T*>(Elements.back().get());
    newElem->SetParentGraph(this);
    return newElem;
  }

protected:
  std::vector<std::unique_ptr<Element>> Elements;
};

/// @brief Represents a software bill of materials (SBOM) document
///
/// An SBOM document is a potential serialization of an SBOM graph with a
/// collection of root elements.
class Document
  : public Graph
  , public Element
{
public:
  Document() = default;

  class Serializer
  {
  public:
    virtual ~Serializer() = default;
    virtual bool Serialize(
      Document const& doc,
      std::vector<std::unique_ptr<Element>> const& all_elements,
      std::ostream& os) = 0;
  };

  bool Serialize(std::ostream& os, Serializer& serializer) const
  {
    return serializer.Serialize(*this, this->Elements, os);
  }

  template <typename T,
            typename = cm::enable_if_t<std::is_base_of<Element, T>::value>>
  T* AddRootElement(std::unique_ptr<T> element)
  {
    auto* newElem = this->AddElement(std::move(element));
    RootElements.push_back(newElem);
    return newElem;
  }

  std::vector<Element*> const& GetRootElements() const { return RootElements; }

protected:
  std::vector<Element*> RootElements;
};

class Relationship : public Element
{
private:
  std::vector<Element*> TargetElements;

public:
  Relationship(RelationshipType type, Element* source, Element* target)
    : Type(type)
    , SourceElement(source)
  {
    this->TargetElements.push_back(target);
  }

  RelationshipType GetType() const { return Type; }
  Element* GetSourceElement() const { return SourceElement; }
  std::vector<Element*> const& GetTargetElements() const
  {
    return TargetElements;
  }

private:
  RelationshipType Type;
  Element* SourceElement;
};

class Artifact : public Element
{
public:
  Artifact(std::string name)
    : Element(std::move(name))
  {
  }
};

class Package : public Artifact
{
public:
  Package(std::string name)
    : Artifact(std::move(name))
  {
  }
};

class File : public Artifact
{
public:
  enum class FileKind
  {
    File,
    Directory,
    Symlink,
  };

  File(std::string name, FileKind kind)
    : Artifact(std::move(name))
    , Kind(kind)
  {
  }

  FileKind GetKind() const { return Kind; }
  void SetKind(FileKind kind) { this->Kind = kind; }

private:
  FileKind Kind;
};

} // namespace cmSbom
