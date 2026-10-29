/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file LICENSE.rst or https://cmake.org/licensing for details.  */
#pragma once

#include "cmConfigure.h" // IWYU pragma: keep

#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

#include <cm/optional>

class cmJSONState;

namespace cmPackageInfo {

struct SchemaVersion
{
  SchemaVersion() = default;
  SchemaVersion(unsigned major, unsigned minor, unsigned patch = 0)
    : Major(major)
    , Minor(minor)
    , Patch(patch)
  {
  }

  unsigned Major = 0;
  unsigned Minor = 0;
  unsigned Patch = 0;

  friend bool operator==(SchemaVersion lhs, SchemaVersion rhs)
  {
    return std::tie(lhs.Major, lhs.Minor, lhs.Patch) ==
      std::tie(rhs.Major, rhs.Minor, rhs.Patch);
  }
  friend bool operator!=(SchemaVersion lhs, SchemaVersion rhs)
  {
    return !(lhs == rhs);
  }
  friend bool operator<(SchemaVersion lhs, SchemaVersion rhs)
  {
    return std::tie(lhs.Major, lhs.Minor, lhs.Patch) <
      std::tie(rhs.Major, rhs.Minor, rhs.Patch);
  }
  friend bool operator>(SchemaVersion lhs, SchemaVersion rhs)
  {
    return rhs < lhs;
  }
  friend bool operator<=(SchemaVersion lhs, SchemaVersion rhs)
  {
    return !(rhs < lhs);
  }
  friend bool operator>=(SchemaVersion lhs, SchemaVersion rhs)
  {
    return !(lhs < rhs);
  }
};

// CPS configuration overlays distinguish omission (inherit) from null (unset).
template <typename T>
class Field
{
public:
  Field() = default;
  Field(T value)
    : Present(true)
    , Data(std::move(value))
  {
  }

  static Field Null()
  {
    Field result;
    result.Present = true;
    return result;
  }

  bool IsPresent() const { return this->Present; }
  bool IsNull() const { return this->Present && !this->Data; }
  T const* Get() const { return this->Data ? &*this->Data : nullptr; }
  T* Get() { return this->Data ? &*this->Data : nullptr; }

private:
  bool Present = false;
  cm::optional<T> Data;
};

using StringList = std::vector<std::string>;
using LanguageMap = std::unordered_map<std::string, StringList>;

class LanguageStringLists
{
public:
  LanguageStringLists();
  LanguageStringLists(StringList values);
  LanguageStringLists(LanguageMap languages);
  LanguageStringLists(LanguageStringLists const& other);
  LanguageStringLists(LanguageStringLists&& other) noexcept;
  LanguageStringLists& operator=(LanguageStringLists const& other);
  LanguageStringLists& operator=(LanguageStringLists&& other) noexcept;
  ~LanguageStringLists();

  StringList const* GetList() const
  {
    return this->Tag_ == Tag::List ? &this->StringList_ : nullptr;
  }
  StringList* GetList()
  {
    return this->Tag_ == Tag::List ? &this->StringList_ : nullptr;
  }
  LanguageMap const* GetLanguages() const
  {
    return this->Tag_ == Tag::ByLanguage ? &this->LanguageMap_ : nullptr;
  }
  LanguageMap* GetLanguages()
  {
    return this->Tag_ == Tag::ByLanguage ? &this->LanguageMap_ : nullptr;
  }

private:
  enum class Tag
  {
    List,
    ByLanguage
  };
  Tag Tag_;

  union
  {
    StringList StringList_;
    LanguageMap LanguageMap_;
  };

  void Destroy();
};

// null is a valueless definition; an empty string is a definition with '='.
using DefinitionMap =
  std::unordered_map<std::string, cm::optional<std::string>>;
using LanguageDefinitions = std::unordered_map<std::string, DefinitionMap>;

struct Properties
{
  Field<LanguageStringLists> Includes;
  Field<LanguageStringLists> Embeds;
  Field<LanguageStringLists> CompileFlags;
  Field<LanguageDefinitions> Definitions;
  Field<StringList> CompileFeatures;
  Field<StringList> LinkFeatures;
  Field<StringList> LinkFlags;
  Field<StringList> LinkDirectories;
  Field<StringList> LinkLanguages;
  Field<StringList> LinkLibraries;
  Field<StringList> Requires;
  Field<StringList> CompileRequires;
  Field<StringList> LinkRequires;
  Field<StringList> DyldRequires;
  Field<std::string> Location;
  Field<std::string> LinkLocation;
  Field<std::string> LinkName;
  Field<std::string> CppModuleMetadata;
};

struct FileSetCMakeExtensions
{
  Field<std::string> NameV1;
};

struct FileSet
{
  // Keep unknown types: consumers decide which types they support.
  std::string Type;
  std::string Root;
  StringList Files;
  Field<FileSetCMakeExtensions> CMake;
};

struct Component
{
  std::string Type;
  Properties Usage;
  Field<std::string> License;
  Field<std::string> Description;
  Field<std::vector<FileSet>> FileSets;
  Field<std::unordered_map<std::string, Properties>> Configurations;
};

struct RequirementCMakeExtensions
{
  Field<StringList> DomainsV1;
};

struct Requirement
{
  Field<std::string> Version;
  Field<StringList> Components;
  Field<StringList> Hints;
  Field<RequirementCMakeExtensions> CMake;
};

using Requirements = std::unordered_map<std::string, Requirement>;

struct Platform
{
  Field<std::string> Isa;
  Field<std::string> Kernel;
  Field<std::string> KernelVersion;
  Field<std::string> CRuntimeVendor;
  Field<std::string> CRuntimeVersion;
  Field<std::string> CppRuntimeVendor;
  Field<std::string> CppRuntimeVersion;
  Field<std::string> ClrVendor;
  Field<std::string> ClrVersion;
  Field<std::string> JvmVendor;
  Field<std::string> JvmVersion;
};

struct PackageMetadata
{
  Field<std::string> Version;
  Field<std::string> CompatVersion;
  Field<std::string> VersionSchema;
  Field<std::string> Description;
  Field<std::string> DisplayName;
  Field<std::string> Website;
  Field<std::string> License;
  Field<std::string> DefaultLicense;
  Field<std::string> MetaComment;
  Field<std::string> MetaSchema;
};

struct Package
{
  std::string Name;
  PackageMetadata Metadata;
  Field<Platform> TargetPlatform;
  Field<Requirements> Requires;
  Field<StringList> DefaultComponents;
  Field<StringList> Configurations;
  std::unordered_map<std::string, Component> Components;
};

struct Root : Package
{
  SchemaVersion CpsVersion;
  Field<std::string> Prefix;
  Field<std::string> CpsPath;
};

struct Appendix : Package
{
  // An appendix may omit the schema version and use its parent's version.
  Field<SchemaVersion> CpsVersion;
};

struct ConfigurationSupplement
{
  std::string Name;
  std::string Configuration;
  std::unordered_map<std::string, Properties> Components;
};

template <typename T>
struct Result
{
  cm::optional<T> Value;
  // Formatted by cmJSONState, with source context when known.
  std::string Error;
  explicit operator bool() const { return static_cast<bool>(this->Value); }
};

Result<Root> DecodeRoot(std::string const& input);
Result<Appendix> DecodeAppendix(std::string const& input,
                                SchemaVersion inheritedCpsVersion);
Result<ConfigurationSupplement> DecodeConfigurationSupplement(
  std::string const& input, SchemaVersion inheritedCpsVersion);

Result<std::string> Encode(Root const& document);
Result<std::string> Encode(Appendix const& document,
                           SchemaVersion inheritedCpsVersion);
Result<std::string> Encode(ConfigurationSupplement const& document,
                           SchemaVersion inheritedCpsVersion);

// Structural/document validation only. Artifact existence, name resolution,
// version ordering and inheritance are responsibilities of the consumer.
// Validation of typed objects reports errors without source locations.
bool Validate(Root const& document, cmJSONState& state);
bool Validate(Appendix const& document, SchemaVersion inheritedCpsVersion,
              cmJSONState& state);
bool Validate(ConfigurationSupplement const& document,
              SchemaVersion inheritedCpsVersion, cmJSONState& state);

} // namespace cmPackageInfo
