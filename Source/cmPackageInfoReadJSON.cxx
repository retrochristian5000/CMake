/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file LICENSE.rst or https://cmake.org/licensing for details.  */
#include <cstddef>
#include <functional>
#include <initializer_list>
#include <limits>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <cm/optional>
#include <cmext/string_view>

#include <cm3p/json/value.h>

#include "cmJSONHelpers.h"
#include "cmJSONState.h"
#include "cmPackageInfo.h"
#include "cmStringAlgorithms.h"

namespace cmPackageInfo {
namespace {

using Builder = cmJSONHelperBuilder;

template <typename T>
Builder::Object<T> ObjectHelper()
{
  return Builder::Object<T>(JsonErrors::INVALID_NAMED_OBJECT_KEY);
}

// Optional CPS attributes accept null. Unlike cm::optional, Field also retains
// that null, since it can suppress inheritance in a configuration overlay.
template <typename T, typename Helper>
cmJSONHelper<Field<T>> FieldHelper(Helper helper)
{
  return
    [helper](Field<T>& out, Json::Value const* value, cmJSONState* state) {
      if (!value) {
        out = Field<T>();
        return true;
      }
      if (value->isNull()) {
        out = Field<T>::Null();
        return true;
      }
      T decoded;
      if (!helper(decoded, value, state)) {
        return false;
      }
      out = Field<T>(std::move(decoded));
      return true;
    };
}

template <typename T, typename Helper>
cmJSONHelper<cm::optional<T>> NullableHelper(Helper helper)
{
  return [helper](cm::optional<T>& out, Json::Value const* value,
                  cmJSONState* state) {
    if (value && value->isNull()) {
      out.reset();
      return true;
    }
    T decoded;
    if (!helper(decoded, value, state)) {
      return false;
    }
    out = std::move(decoded);
    return true;
  };
}

template <typename T, typename Helper>
cmJSONHelper<std::vector<T>> VectorHelper(Helper helper)
{
  return Builder::Vector<T>(JsonErrors::EXPECTED_TYPE("an array"), helper);
}

cmJSONHelper<std::string> StringHelper()
{
  return Builder::String();
}

bool ParseSchemaVersion(std::string const& text, SchemaVersion& version)
{
  SchemaVersion decoded;
  unsigned* const parts[]{ &decoded.Major, &decoded.Minor, &decoded.Patch };
  std::size_t pos = 0;
  for (unsigned i = 0; i < 3; ++i) {
    std::size_t const end = text.find('.', pos);
    std::string const part = text.substr(pos, end - pos);
    unsigned long number;
    if (part.empty() ||
        part.find_first_not_of("0123456789") != std::string::npos ||
        !cmStrToULong(part, &number) ||
        number > (std::numeric_limits<unsigned>::max)()) {
      return false;
    }
    *parts[i] = static_cast<unsigned>(number);
    if (end == std::string::npos) {
      if (i == 0) {
        return false;
      }
      version = decoded;
      return true;
    }
    pos = end + 1;
  }
  return false;
}

cmJSONHelper<SchemaVersion> SchemaVersionHelper()
{
  return [](SchemaVersion& out, Json::Value const* value, cmJSONState* state) {
    std::string text;
    if (!StringHelper()(text, value, state)) {
      return false;
    }
    if (!ParseSchemaVersion(text, out)) {
      state->AddErrorAtValue(
        "\"cps_version\" expected 'major.minor[.patch]' with unsigned integer "
        "components in range",
        value);
      return false;
    }
    return true;
  };
}

cmJSONHelper<StringList> StringsHelper()
{
  return VectorHelper<std::string>(StringHelper());
}

template <typename T, typename Helper>
cmJSONHelper<std::unordered_map<std::string, T>> MapHelper(Helper helper)
{
  return Builder::FilteredObject<std::unordered_map<std::string, T>>(
    JsonErrors::EXPECTED_TYPE("an object"), helper,
    [](std::string const&, Json::Value const*, cmJSONState*) {
      return Builder::FilterResult::Continue;
    });
}

bool RejectFields(Json::Value const* value, cmJSONState* state,
                  std::initializer_list<char const*> fields)
{
  bool success = true;
  // This can be called before the Object helper checks the enclosing value.
  if (value && value->isObject()) {
    for (char const* field : fields) {
      if (value->isMember(field)) {
        state->AddErrorAtValue(
          cmStrCat("Member \"", field,
                   "\" is not allowed in this document or object"),
          &(*value)[field]);
        success = false;
      }
    }
  }
  return success;
}

cmJSONHelper<LanguageStringLists> LanguageListsHelper()
{
  return [](LanguageStringLists& out, Json::Value const* value,
            cmJSONState* state) {
    if (value && value->isArray()) {
      StringList values;
      if (!StringsHelper()(values, value, state)) {
        return false;
      }
      out = LanguageStringLists(std::move(values));
      return true;
    }
    LanguageMap values;
    if (!MapHelper<StringList>(StringsHelper())(values, value, state)) {
      return false;
    }
    out = LanguageStringLists(std::move(values));
    return true;
  };
}

cmJSONHelper<Properties> PropertiesHelper(bool configuration = true)
{
  auto object = ObjectHelper<Properties>();
  auto lists = FieldHelper<LanguageStringLists>(LanguageListsHelper());
  auto strings = FieldHelper<StringList>(StringsHelper());
  auto string = FieldHelper<std::string>(StringHelper());
  auto definitions = FieldHelper<LanguageDefinitions>(
    MapHelper<DefinitionMap>(MapHelper<cm::optional<std::string>>(
      NullableHelper<std::string>(StringHelper()))));
  object.Bind("includes"_s, &Properties::Includes, lists, false)
    .Bind("embeds"_s, &Properties::Embeds, lists, false)
    .Bind("compile_flags"_s, &Properties::CompileFlags, lists, false)
    .Bind("definitions"_s, &Properties::Definitions, definitions, false)
    .Bind("compile_features"_s, &Properties::CompileFeatures, strings, false)
    .Bind("link_features"_s, &Properties::LinkFeatures, strings, false)
    .Bind("link_flags"_s, &Properties::LinkFlags, strings, false)
    .Bind("link_directories"_s, &Properties::LinkDirectories, strings, false)
    .Bind("link_languages"_s, &Properties::LinkLanguages, strings, false)
    .Bind("link_libraries"_s, &Properties::LinkLibraries, strings, false)
    .Bind("requires"_s, &Properties::Requires, strings, false)
    .Bind("compile_requires"_s, &Properties::CompileRequires, strings, false)
    .Bind("link_requires"_s, &Properties::LinkRequires, strings, false)
    .Bind("dyld_requires"_s, &Properties::DyldRequires, strings, false)
    .Bind("location"_s, &Properties::Location, string, false)
    .Bind("link_location"_s, &Properties::LinkLocation, string, false)
    .Bind("link_name"_s, &Properties::LinkName, string, false)
    .Bind("cpp_module_metadata"_s, &Properties::CppModuleMetadata, string,
          false);
  return [object, configuration](Properties& out, Json::Value const* value,
                                 cmJSONState* state) {
    bool const allowed = !configuration ||
      RejectFields(
        value, state,
        { "type", "configurations", "file_sets", "license", "description" });
    return object(out, value, state) && allowed;
  };
}

template <typename T, typename Helper>
cmJSONHelper<Field<T>> ExtensionsHelper(Helper helper)
{
  auto object = ObjectHelper<Field<T>>().Bind(
    "cmake"_s,
    [helper](Field<T>& out, Json::Value const* value, cmJSONState* state) {
      return FieldHelper<T>(helper)(out, value, state);
    },
    false);
  return
    [object](Field<T>& out, Json::Value const* value, cmJSONState* state) {
      if (!value || value->isNull()) {
        out = Field<T>();
        return true;
      }
      return object(out, value, state);
    };
}

cmJSONHelper<FileSet> FileSetHelper()
{
  auto cmake = ObjectHelper<FileSetCMakeExtensions>().Bind(
    "name@v1"_s, &FileSetCMakeExtensions::NameV1,
    FieldHelper<std::string>(StringHelper()), false);
  return ObjectHelper<FileSet>()
    .Bind("type"_s, &FileSet::Type, StringHelper())
    .Bind("root"_s, &FileSet::Root, StringHelper())
    .Bind("files"_s, &FileSet::Files, StringsHelper())
    .Bind("extensions"_s, &FileSet::CMake,
          ExtensionsHelper<FileSetCMakeExtensions>(cmake), false);
}

cmJSONHelper<Component> ComponentHelper()
{
  auto object =
    ObjectHelper<Component>()
      .Bind("type"_s, &Component::Type, StringHelper())
      .Bind("license"_s, &Component::License,
            FieldHelper<std::string>(StringHelper()), false)
      .Bind("description"_s, &Component::Description,
            FieldHelper<std::string>(StringHelper()), false)
      .Bind("file_sets"_s, &Component::FileSets,
            FieldHelper<std::vector<FileSet>>(
              VectorHelper<FileSet>(FileSetHelper())),
            false)
      .Bind("configurations"_s, &Component::Configurations,
            FieldHelper<std::unordered_map<std::string, Properties>>(
              MapHelper<Properties>(PropertiesHelper())),
            false);
  return
    [object](Component& out, Json::Value const* value, cmJSONState* state) {
      bool const success = object(out, value, state);
      return PropertiesHelper(false)(out.Usage, value, state) && success;
    };
}

cmJSONHelper<Requirement> RequirementHelper()
{
  auto cmake = ObjectHelper<RequirementCMakeExtensions>().Bind(
    "domains@v1"_s, &RequirementCMakeExtensions::DomainsV1,
    FieldHelper<StringList>(StringsHelper()), false);
  auto object =
    ObjectHelper<Requirement>()
      .Bind("version"_s, &Requirement::Version,
            FieldHelper<std::string>(StringHelper()), false)
      .Bind("components"_s, &Requirement::Components,
            FieldHelper<StringList>(StringsHelper()), false)
      .Bind("hints"_s, &Requirement::Hints,
            FieldHelper<StringList>(StringsHelper()), false)
      .Bind("extensions"_s, &Requirement::CMake,
            ExtensionsHelper<RequirementCMakeExtensions>(cmake), false);
  return
    [object](Requirement& out, Json::Value const* value, cmJSONState* state) {
      // CPS defines a null requirement as an empty requirement object.
      if (value && value->isNull()) {
        out = Requirement();
        return true;
      }
      return object(out, value, state);
    };
}

cmJSONHelper<Platform> PlatformHelper()
{
  auto string = FieldHelper<std::string>(StringHelper());
  return ObjectHelper<Platform>()
    .Bind("isa"_s, &Platform::Isa, string, false)
    .Bind("kernel"_s, &Platform::Kernel, string, false)
    .Bind("kernel_version"_s, &Platform::KernelVersion, string, false)
    .Bind("c_runtime_vendor"_s, &Platform::CRuntimeVendor, string, false)
    .Bind("c_runtime_version"_s, &Platform::CRuntimeVersion, string, false)
    .Bind("cpp_runtime_vendor"_s, &Platform::CppRuntimeVendor, string, false)
    .Bind("cpp_runtime_version"_s, &Platform::CppRuntimeVersion, string, false)
    .Bind("clr_vendor"_s, &Platform::ClrVendor, string, false)
    .Bind("clr_version"_s, &Platform::ClrVersion, string, false)
    .Bind("jvm_vendor"_s, &Platform::JvmVendor, string, false)
    .Bind("jvm_version"_s, &Platform::JvmVersion, string, false);
}

cmJSONHelper<PackageMetadata> MetadataHelper()
{
  auto string = FieldHelper<std::string>(StringHelper());
  return ObjectHelper<PackageMetadata>()
    .Bind("version"_s, &PackageMetadata::Version, string, false)
    .Bind("compat_version"_s, &PackageMetadata::CompatVersion, string, false)
    .Bind("version_schema"_s, &PackageMetadata::VersionSchema, string, false)
    .Bind("description"_s, &PackageMetadata::Description, string, false)
    .Bind("display_name"_s, &PackageMetadata::DisplayName, string, false)
    .Bind("website"_s, &PackageMetadata::Website, string, false)
    .Bind("license"_s, &PackageMetadata::License, string, false)
    .Bind("default_license"_s, &PackageMetadata::DefaultLicense, string, false)
    .Bind("meta_comment"_s, &PackageMetadata::MetaComment, string, false)
    .Bind("meta_schema"_s, &PackageMetadata::MetaSchema, string, false);
}

cmJSONHelper<Package> PackageHelper()
{
  auto object = ObjectHelper<Package>()
                  .Bind("name"_s, &Package::Name, StringHelper())
                  .Bind("components"_s, &Package::Components,
                        MapHelper<Component>(ComponentHelper()))
                  .Bind("requires"_s, &Package::Requires,
                        FieldHelper<Requirements>(
                          MapHelper<Requirement>(RequirementHelper())),
                        false)
                  .Bind("default_components"_s, &Package::DefaultComponents,
                        FieldHelper<StringList>(StringsHelper()), false)
                  .Bind("configurations"_s, &Package::Configurations,
                        FieldHelper<StringList>(StringsHelper()), false)
                  .Bind("platform"_s, &Package::TargetPlatform,
                        FieldHelper<Platform>(PlatformHelper()), false);
  return [object](Package& out, Json::Value const* value, cmJSONState* state) {
    bool const success = object(out, value, state);
    return MetadataHelper()(out.Metadata, value, state) && success;
  };
}

cmJSONHelper<Root> RootHelper()
{
  auto object =
    ObjectHelper<Root>()
      .Bind("cps_version"_s, &Root::CpsVersion, SchemaVersionHelper())
      .Bind("prefix"_s, &Root::Prefix,
            FieldHelper<std::string>(StringHelper()), false)
      .Bind("cps_path"_s, &Root::CpsPath,
            FieldHelper<std::string>(StringHelper()), false);
  return [object](Root& out, Json::Value const* value, cmJSONState* state) {
    bool const success = object(out, value, state);
    return PackageHelper()(out, value, state) && success;
  };
}

cmJSONHelper<Appendix> AppendixHelper()
{
  auto object = ObjectHelper<Appendix>().Bind(
    "cps_version"_s, &Appendix::CpsVersion,
    FieldHelper<SchemaVersion>(SchemaVersionHelper()), false);
  return
    [object](Appendix& out, Json::Value const* value, cmJSONState* state) {
      bool const success = object(out, value, state);
      return PackageHelper()(out, value, state) && success;
    };
}

cmJSONHelper<ConfigurationSupplement> SupplementHelper()
{
  auto object =
    ObjectHelper<ConfigurationSupplement>()
      .Bind("name"_s, &ConfigurationSupplement::Name, StringHelper())
      .Bind("configuration"_s, &ConfigurationSupplement::Configuration,
            StringHelper())
      .Bind("components"_s, &ConfigurationSupplement::Components,
            MapHelper<Properties>(PropertiesHelper()));
  return [object](ConfigurationSupplement& out, Json::Value const* value,
                  cmJSONState* state) {
    bool const allowed = RejectFields(
      value, state,
      { "cps_version", "prefix", "cps_path", "requires", "configurations",
        "default_components", "platform", "version", "compat_version",
        "version_schema", "description", "display_name", "website", "license",
        "default_license", "meta_comment", "meta_schema" });
    return object(out, value, state) && allowed;
  };
}

template <typename T, typename Helper, typename Validator>
Result<T> Decode(std::string const& input, Helper helper, Validator validate)
{
  Result<T> result;
  Json::Value root;
  std::istringstream stream(input);
  // Keep the existing CPS reader's syntax compatibility. Type checking is
  // independent of relaxed JSON syntax (comments, for example).
  cmJSONState state(stream, &root, cmJSONState::StrictMode::Relaxed);
  if (!state.errors.empty()) {
    result.Error = state.GetErrorMessage();
    return result;
  }
  T document;
  if (helper()(document, &root, &state) && validate(document, state)) {
    result.Value = std::move(document);
  } else {
    result.Error = state.GetErrorMessage();
  }
  return result;
}

} // namespace

Result<Root> DecodeRoot(std::string const& input)
{
  return Decode<Root>(input, RootHelper,
                      [](Root const& document, cmJSONState& state) {
                        return Validate(document, state);
                      });
}

Result<Appendix> DecodeAppendix(std::string const& input,
                                SchemaVersion inheritedCpsVersion)
{
  return Decode<Appendix>(
    input, AppendixHelper,
    [inheritedCpsVersion](Appendix const& document, cmJSONState& state) {
      return Validate(document, inheritedCpsVersion, state);
    });
}

Result<ConfigurationSupplement> DecodeConfigurationSupplement(
  std::string const& input, SchemaVersion inheritedCpsVersion)
{
  return Decode<ConfigurationSupplement>(
    input, SupplementHelper,
    [inheritedCpsVersion](ConfigurationSupplement const& document,
                          cmJSONState& state) {
      return Validate(document, inheritedCpsVersion, state);
    });
}

} // namespace cmPackageInfo
