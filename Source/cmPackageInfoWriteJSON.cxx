/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file LICENSE.rst or https://cmake.org/licensing for details.  */
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <cm/optional>

#include <cm3p/json/value.h>
#include <cm3p/json/writer.h>

#include "cmJSONState.h"
#include "cmPackageInfo.h"
#include "cmStringAlgorithms.h"

namespace cmPackageInfo {
namespace {

Json::Value String(std::string const& value)
{
  return Json::Value(value);
}

Json::Value SchemaVersionValue(SchemaVersion value)
{
  return Json::Value(
    cmStrCat(value.Major, '.', value.Minor, '.', value.Patch));
}

template <typename T, typename Encoder>
Json::Value Vector(std::vector<T> const& values, Encoder encode)
{
  Json::Value result(Json::arrayValue);
  for (auto const& value : values) {
    result.append(encode(value));
  }
  return result;
}

Json::Value Strings(StringList const& values)
{
  return Vector(values, String);
}

template <typename T, typename Encoder>
Json::Value Map(std::unordered_map<std::string, T> const& values,
                Encoder encode)
{
  Json::Value result(Json::objectValue);
  for (auto const& entry : values) {
    result[entry.first] = encode(entry.second);
  }
  return result;
}

template <typename T, typename Encoder>
void Member(Json::Value& object, char const* name, Field<T> const& field,
            Encoder encode)
{
  if (auto const* value = field.Get()) {
    object[name] = encode(*value);
  } else if (field.IsNull()) {
    object[name] = Json::Value(Json::nullValue);
  }
}

template <typename T, typename Encoder>
void Extensions(Json::Value& object, Field<T> const& cmake, Encoder encode)
{
  if (cmake.IsPresent()) {
    Json::Value extensions(Json::objectValue);
    Member(extensions, "cmake", cmake, encode);
    object["extensions"] = std::move(extensions);
  }
}

Json::Value LanguageLists(LanguageStringLists const& values)
{
  if (auto const* languages = values.GetLanguages()) {
    return Map(*languages, Strings);
  }
  return Strings(*values.GetList());
}

Json::Value Definitions(LanguageDefinitions const& values)
{
  return Map(values, [](DefinitionMap const& definitions) {
    return Map(definitions, [](cm::optional<std::string> const& value) {
      return value ? String(*value) : Json::Value(Json::nullValue);
    });
  });
}

Json::Value PropertiesValue(Properties const& value)
{
  Json::Value result(Json::objectValue);
  Member(result, "includes", value.Includes, LanguageLists);
  Member(result, "embeds", value.Embeds, LanguageLists);
  Member(result, "compile_flags", value.CompileFlags, LanguageLists);
  Member(result, "definitions", value.Definitions, Definitions);
  Member(result, "compile_features", value.CompileFeatures, Strings);
  Member(result, "link_features", value.LinkFeatures, Strings);
  Member(result, "link_flags", value.LinkFlags, Strings);
  Member(result, "link_directories", value.LinkDirectories, Strings);
  Member(result, "link_languages", value.LinkLanguages, Strings);
  Member(result, "link_libraries", value.LinkLibraries, Strings);
  Member(result, "requires", value.Requires, Strings);
  Member(result, "compile_requires", value.CompileRequires, Strings);
  Member(result, "link_requires", value.LinkRequires, Strings);
  Member(result, "dyld_requires", value.DyldRequires, Strings);
  Member(result, "location", value.Location, String);
  Member(result, "link_location", value.LinkLocation, String);
  Member(result, "link_name", value.LinkName, String);
  Member(result, "cpp_module_metadata", value.CppModuleMetadata, String);
  return result;
}

Json::Value FileSetValue(FileSet const& value)
{
  Json::Value result(Json::objectValue);
  result["type"] = value.Type;
  result["root"] = value.Root;
  result["files"] = Strings(value.Files);
  Extensions(result, value.CMake, [](FileSetCMakeExtensions const& cmake) {
    Json::Value extension(Json::objectValue);
    Member(extension, "name@v1", cmake.NameV1, String);
    return extension;
  });
  return result;
}

Json::Value ComponentValue(Component const& value)
{
  Json::Value result = PropertiesValue(value.Usage);
  result["type"] = value.Type;
  Member(result, "license", value.License, String);
  Member(result, "description", value.Description, String);
  Member(result, "file_sets", value.FileSets,
         [](std::vector<FileSet> const& files) {
           return Vector(files, FileSetValue);
         });
  Member(
    result, "configurations", value.Configurations,
    [](std::unordered_map<std::string, Properties> const& configurations) {
      return Map(configurations, PropertiesValue);
    });
  return result;
}

Json::Value RequirementValue(Requirement const& value)
{
  Json::Value result(Json::objectValue);
  Member(result, "version", value.Version, String);
  Member(result, "components", value.Components, Strings);
  Member(result, "hints", value.Hints, Strings);
  Extensions(result, value.CMake, [](RequirementCMakeExtensions const& cmake) {
    Json::Value extension(Json::objectValue);
    Member(extension, "domains@v1", cmake.DomainsV1, Strings);
    return extension;
  });
  return result;
}

Json::Value PlatformValue(Platform const& value)
{
  Json::Value result(Json::objectValue);
  Member(result, "isa", value.Isa, String);
  Member(result, "kernel", value.Kernel, String);
  Member(result, "kernel_version", value.KernelVersion, String);
  Member(result, "c_runtime_vendor", value.CRuntimeVendor, String);
  Member(result, "c_runtime_version", value.CRuntimeVersion, String);
  Member(result, "cpp_runtime_vendor", value.CppRuntimeVendor, String);
  Member(result, "cpp_runtime_version", value.CppRuntimeVersion, String);
  Member(result, "clr_vendor", value.ClrVendor, String);
  Member(result, "clr_version", value.ClrVersion, String);
  Member(result, "jvm_vendor", value.JvmVendor, String);
  Member(result, "jvm_version", value.JvmVersion, String);
  return result;
}

Json::Value PackageValue(Package const& value)
{
  Json::Value result(Json::objectValue);
  result["name"] = value.Name;
  result["components"] = Map(value.Components, ComponentValue);
  Member(result, "platform", value.TargetPlatform, PlatformValue);
  Member(result, "requires", value.Requires,
         [](Requirements const& requirements) {
           return Map(requirements, RequirementValue);
         });
  Member(result, "default_components", value.DefaultComponents, Strings);
  Member(result, "configurations", value.Configurations, Strings);
  auto const& metadata = value.Metadata;
  Member(result, "version", metadata.Version, String);
  Member(result, "compat_version", metadata.CompatVersion, String);
  Member(result, "version_schema", metadata.VersionSchema, String);
  Member(result, "description", metadata.Description, String);
  Member(result, "display_name", metadata.DisplayName, String);
  Member(result, "website", metadata.Website, String);
  Member(result, "license", metadata.License, String);
  Member(result, "default_license", metadata.DefaultLicense, String);
  Member(result, "meta_comment", metadata.MetaComment, String);
  Member(result, "meta_schema", metadata.MetaSchema, String);
  return result;
}

Result<std::string> Write(Json::Value const& value)
{
  Result<std::string> result;
  Json::StreamWriterBuilder builder;
  builder["indentation"] = "  ";
  builder["commentStyle"] = "None";
  result.Value = Json::writeString(builder, value);
  return result;
}

} // namespace

Result<std::string> Encode(Root const& document)
{
  Result<std::string> result;
  cmJSONState state;
  if (!Validate(document, state)) {
    result.Error = state.GetErrorMessage();
    return result;
  }
  Json::Value value = PackageValue(document);
  value["cps_version"] = SchemaVersionValue(document.CpsVersion);
  Member(value, "prefix", document.Prefix, String);
  Member(value, "cps_path", document.CpsPath, String);
  return Write(value);
}

Result<std::string> Encode(Appendix const& document,
                           SchemaVersion inheritedCpsVersion)
{
  Result<std::string> result;
  cmJSONState state;
  if (!Validate(document, inheritedCpsVersion, state)) {
    result.Error = state.GetErrorMessage();
    return result;
  }
  Json::Value value = PackageValue(document);
  Member(value, "cps_version", document.CpsVersion, SchemaVersionValue);
  return Write(value);
}

Result<std::string> Encode(ConfigurationSupplement const& document,
                           SchemaVersion inheritedCpsVersion)
{
  Result<std::string> result;
  cmJSONState state;
  if (!Validate(document, inheritedCpsVersion, state)) {
    result.Error = state.GetErrorMessage();
    return result;
  }
  Json::Value value(Json::objectValue);
  value["name"] = document.Name;
  value["configuration"] = document.Configuration;
  value["components"] = Map(document.Components, PropertiesValue);
  return Write(value);
}

} // namespace cmPackageInfo
