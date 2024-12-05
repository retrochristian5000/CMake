/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#include "cmPackageInfoReader.h"

#include <initializer_list>
#include <limits>
#include <unordered_map>
#include <utility>

#include <cmext/string_view>

#include <cm3p/json/reader.h>
#include <cm3p/json/value.h>

#include "cmsys/FStream.hxx"
#include "cmsys/RegularExpression.hxx"

#include "cmExecutionStatus.h"
#include "cmListFileCache.h"
#include "cmMakefile.h"
#include "cmMessageType.h"
#include "cmStringAlgorithms.h"
#include "cmSystemTools.h"
#include "cmTarget.h"

namespace {

// Map of CPS language names to CMake language name.  Case insensitivity is
// achieved by converting the CPS value to lower case, so keys in this map must
// be lower case.
std::unordered_map<std::string, std::string> Languages = {
  // clang-format off
  { "c", "C" },
  { "c++", "CXX" },
  { "cpp", "CXX" },
  { "cxx", "CXX" },
  { "objc", "OBJC" },
  { "objc++", "OBJCXX" },
  { "objcpp", "OBJCXX" },
  { "objcxx", "OBJCXX" },
  { "swift", "swift" },
  { "hip", "HIP" },
  { "cuda", "CUDA" },
  { "ispc", "ISPC" },
  { "c#", "CSharp" },
  { "csharp", "CSharp" },
  { "fortran", "Fortran" },
  // clang-format on
};

std::string GetRealPath(std::string const& path)
{
  return cmSystemTools::GetRealPath(path);
}

std::string GetRealDir(std::string const& path)
{
  return cmSystemTools::GetFilenamePath(cmSystemTools::GetRealPath(path));
}

Json::Value ReadJson(std::string const& fileName)
{
  // Open the specified file.
  cmsys::ifstream file(fileName.c_str(), std::ios::in | std::ios::binary);
  if (!file) {
    return Json::Value::nullSingleton();
  }

  // Read file content and translate JSON.
  Json::Value data;
  Json::CharReaderBuilder builder;
  builder["collectComments"] = false;
  if (!Json::parseFromStream(builder, file, &data, nullptr)) {
    return Json::Value::nullSingleton();
  }

  return data;
}

bool CheckSchemaVersion(Json::Value const& data)
{
  auto const& version = data["cps_version"].asString();

  // Check that a valid version is specified.
  if (version.empty()) {
    return false;
  }

  // Check that we understand this version.
  return cmSystemTools::VersionCompare(cmSystemTools::OP_GREATER_EQUAL,
                                       version, "0.13") &&
    cmSystemTools::VersionCompare(cmSystemTools::OP_LESS, version, "1");

  // TODO Eventually this probably needs to return the version tuple, and
  // should share code with cmPackageInfoReader::ParseVersion.
}

bool ComparePathSuffix(std::string const& path, std::string const& suffix)
{
  auto const& tail = path.substr(path.size() - suffix.size());
  return cmSystemTools::ComparePath(tail, suffix);
}

std::string DeterminePrefix(std::string const& filepath,
                            Json::Value const& data)
{
  // First check if an absolute prefix was supplied.
  auto prefix = data["prefix"].asString();
  if (!prefix.empty()) {
    // Ensure that the specified prefix is valid.
    if (cmsys::SystemTools::FileIsFullPath(prefix) &&
        cmsys::SystemTools::FileIsDirectory(prefix)) {
      cmSystemTools::ConvertToUnixSlashes(prefix);
      return prefix;
    }
    // The specified absolute prefix is not valid.
    return {};
  }

  // Get and validate prefix-relative path.
  auto relPath = data["cps_path"].asString();
  cmSystemTools::ConvertToUnixSlashes(relPath);
  if (relPath.empty() || !cmHasLiteralPrefix(relPath, "@prefix@/")) {
    // The relative prefix is not valid.
    return {};
  }
  relPath = relPath.substr(8);

  // Get directory portion of the absolute path.
  auto const& absPath = cmSystemTools::GetFilenamePath(filepath);
  if (ComparePathSuffix(absPath, relPath)) {
    return absPath.substr(0, absPath.size() - relPath.size());
  }

  for (auto* const f : { GetRealPath, GetRealDir }) {
    auto const& tmpPath = (*f)(absPath);
    if (!cmSystemTools::ComparePath(tmpPath, absPath) &&
        ComparePathSuffix(tmpPath, relPath)) {
      return tmpPath.substr(0, tmpPath.size() - relPath.size());
    }
  }

  return {};
}

// Extract key name from value iterator as string_view.
cm::string_view IterKey(Json::Value::const_iterator const& iter)
{
  char const* end;
  auto const* const start = iter.memberName(&end);
  return { start, static_cast<std::string::size_type>(end - start) };
}

// Get list-of-strings value from object.
std::vector<std::string> ReadList(Json::Value const& data, char const* key)
{
  std::vector<std::string> result;

  auto arr = data[key];
  if (arr.isArray()) {
    for (auto const& val : arr) {
      if (val.isString()) {
        result.push_back(val.asString());
      }
    }
  }

  return result;
}

std::string NormalizeTargetName(std::string const& name,
                                std::string const& context)
{
  if (cmHasLiteralPrefix(name, ":")) {
    return cmStrCat(context, name);
  }

  auto const n = name.find_first_of(':');
  if (n != std::string::npos) {
    auto v = cm::string_view{ name };
    return cmStrCat(v.substr(0, n), ":"_s, v.substr(n));
  }
  return name;
}

void AppendProperty(cmMakefile* makefile, cmTarget* target,
                    cm::string_view property, cm::string_view configuration,
                    std::string const& value)
{
  std::string fullprop;
  if (configuration.empty()) {
    fullprop = cmStrCat("INTERFACE_"_s, property);
  } else {
    fullprop = cmStrCat("INTERFACE_"_s, property, "_"_s,
                        cmSystemTools::UpperCase(configuration));
  }

  target->AppendProperty(fullprop, value, makefile->GetBacktrace());
}

void AddCompileFeature(cmMakefile* makefile, cmTarget* target,
                       cm::string_view configuration, std::string const& value)
{
  auto reLanguageLevel = [] {
    static cmsys::RegularExpression re{ "^[Cc]([+][+])?([0-9][0-9])$" };
    return re;
  }();

  if (reLanguageLevel.find(value)) {
    auto const n = reLanguageLevel.end() - 2;
    auto const featurePrefix = (n == 3 ? "cxx_std_"_s : "c_std_"_s);
    if (configuration.empty()) {
      AppendProperty(makefile, target, "COMPILE_FEATURES"_s, {},
                     cmStrCat(featurePrefix, value.substr(n)));
    } else {
      auto const& feature = cmStrCat("$<$<CONFIG:"_s, configuration, ">:"_s,
                                     featurePrefix, value.substr(n), ">"_s);
      AppendProperty(makefile, target, "COMPILE_FEATURES"_s, {}, feature);
    }
  } else if (cmStrCaseEq(value, "gnu"_s)) {
    // Not implemented in CMake at this time
  } else if (cmStrCaseEq(value, "threads"_s)) {
    AppendProperty(makefile, target, "LINK_LIBRARIES"_s, configuration,
                   "Threads::Threads");
  }
}

void AddLinkFeature(cmMakefile* makefile, cmTarget* target,
                    cm::string_view configuration, std::string const& value)
{
  if (cmStrCaseEq(value, "thread"_s)) {
    AppendProperty(makefile, target, "LINK_LIBRARIES"_s, configuration,
                   "Threads::Threads");
  }
}

} // namespace

std::unique_ptr<cmPackageInfoReader> cmPackageInfoReader::Read(
  std::string const& path, cmPackageInfoReader const* parent)
{
  // Read file and perform some basic validation:
  //   - the input is valid JSON
  //   - the input is a JSON object
  //   - the input has a "cps_version" that we (in theory) know how to parse
  auto data = ReadJson(path);
  if (!data.isObject() || !CheckSchemaVersion(data)) {
    return nullptr;
  }

  //   - the input has a "name" attribute that is a non-empty string
  auto const& name = data["name"];
  if (!name.isString() || name.empty()) {
    return nullptr;
  }

  //   - the input has a "components" attribute that is a JSON object
  if (!data["components"].isObject()) {
    return nullptr;
  }

  std::string prefix = (parent ? parent->Prefix : DeterminePrefix(path, data));
  if (prefix.empty()) {
    return nullptr;
  }

  // Seems sane enough to hand back to the caller.
  auto reader = std::unique_ptr<cmPackageInfoReader>(new cmPackageInfoReader);
  reader->Data = std::move(data);
  reader->Prefix = std::move(prefix);
  reader->Path = path;

  // Determine other information we need to know immediately, or (if this is
  // a supplemental reader) copy from the parent.
  if (parent) {
    reader->ComponentTargets = parent->ComponentTargets;
    reader->DefaultConfigurations = parent->DefaultConfigurations;
  } else {
    reader->DefaultConfigurations = ReadList(reader->Data, "configurations");
  }

  return reader;
}

std::string cmPackageInfoReader::GetName() const
{
  return this->Data["name"].asString();
}

cm::optional<std::string> cmPackageInfoReader::GetVersion() const
{
  auto const& version = this->Data["version"];
  if (version.isString()) {
    return version.asString();
  }
  return cm::nullopt;
}

std::vector<unsigned> cmPackageInfoReader::ParseVersion() const
{
  // Check that we have a version.
  auto const& version = this->GetVersion();
  if (!version) {
    return {};
  }

  std::vector<unsigned> result;
  cm::string_view remnant{ *version };

  // Check if we know how to parse the version.
  auto const& schema = this->Data["version_schema"];
  if (schema.isNull() || cmStrCaseEq(schema.asString(), "simple"_s)) {
    // Keep going until we run out of parts.
    while (!remnant.empty()) {
      auto n = remnant.find('.');
      auto part = remnant.substr(0, n);
      if (n == std::string::npos) {
        remnant = {};
      } else {
        remnant = remnant.substr(n + 1);
      }

      auto const value = std::stoul(std::string{ part }, &n);
      if (n == 0 || value > std::numeric_limits<unsigned>::max()) {
        // The part was not a valid number or is too big.
        return {};
      }
      result.push_back(static_cast<unsigned>(value));
    }
  }

  return result;
}

std::string cmPackageInfoReader::ResolvePath(std::string&& path) const
{
  cmSystemTools::ConvertToUnixSlashes(path);
  if (cmHasPrefix(path, "@prefix@"_s)) {
    return cmStrCat(this->Prefix, path.substr(8));
  }
  if (!cmSystemTools::FileIsFullPath(path)) {
    return cmStrCat(cmSystemTools::GetFilenamePath(this->Path), '/', path);
  }
  return path;
}

void cmPackageInfoReader::SetOptionalProperty(cmTarget* target,
                                              cm::string_view property,
                                              cm::string_view configuration,
                                              Json::Value const& value) const
{
  if (!value.isNull()) {
    std::string fullprop;
    if (configuration.empty()) {
      fullprop = cmStrCat("IMPORTED_"_s, property);
    } else {
      fullprop = cmStrCat("IMPORTED_"_s, property, "_"_s,
                          cmSystemTools::UpperCase(configuration));
    }

    target->SetProperty(fullprop, this->ResolvePath(value.asString()));
  }
}

void cmPackageInfoReader::SetTargetProperties(
  cmMakefile* makefile, cmTarget* target, Json::Value const& data,
  std::string const& package, cm::string_view configuration) const
{
  // Add compile and link features.
  for (auto const& def : ReadList(data, "compile_features")) {
    AddCompileFeature(makefile, target, configuration, def);
  }

  for (auto const& def : ReadList(data, "link_features")) {
    AddLinkFeature(makefile, target, configuration, def);
  }

  // Add compile definitions.
  for (auto const& def : ReadList(data, "definitions")) {
    AppendProperty(makefile, target, "COMPILE_DEFINITIONS"_s, configuration,
                   def);
  }

  // Add include directories.
  for (auto&& inc : ReadList(data, "includes")) {
    AppendProperty(makefile, target, "INCLUDE_DIRECTORIES"_s, configuration,
                   this->ResolvePath(std::move(inc)));
  }

  // Add link name/location(s).
  this->SetOptionalProperty(target, "LOCATION"_s, configuration,
                            data["location"]);

  this->SetOptionalProperty(target, "IMPLIB"_s, configuration,
                            data["link_location"]);

  this->SetOptionalProperty(target, "SONAME"_s, configuration,
                            data["link_name"]);

  // Add link languages.
  for (auto const& lang : ReadList(data, "link_languages")) {
    auto const li = Languages.find(cmSystemTools::LowerCase(lang));
    if (li != Languages.end()) {
      AppendProperty(makefile, target, "LINK_LANGUAGES"_s, configuration,
                     li->second);
    }
  }

  // Add transitive dependencies.
  for (auto const& dep : ReadList(data, "requires")) {
    AppendProperty(makefile, target, "LINK_LIBRARIES"_s, configuration,
                   NormalizeTargetName(dep, package));
  }

  for (auto const& dep : ReadList(data, "link_requires")) {
    auto const& lib =
      cmStrCat("$<LINK_ONLY:"_s, NormalizeTargetName(dep, package), ">"_s);
    AppendProperty(makefile, target, "LINK_LIBRARIES"_s, configuration, lib);
  }
}

cmTarget* cmPackageInfoReader::AddLibraryComponent(
  cmMakefile* makefile, cmStateEnums::TargetType type, std::string const& name,
  Json::Value const& data, std::string const& package) const
{
  // Create the imported target.
  auto* const target = makefile->AddImportedTarget(name, type, false);

  // Set target properties.
  this->SetTargetProperties(makefile, target, data, package, {});
  auto const& cfgData = data["configurations"];
  for (auto ci = cfgData.begin(), ce = cfgData.end(); ci != ce; ++ci) {
    this->SetTargetProperties(makefile, target, *ci, package, IterKey(ci));
  }

  // Set default configurations.
  if (!this->DefaultConfigurations.empty()) {
    target->SetProperty("IMPORTED_CONFIGURATIONS",
                        cmJoin(this->DefaultConfigurations, ";"_s));
  }

  return target;
}

bool cmPackageInfoReader::ImportTargets(cmMakefile* makefile,
                                        cmExecutionStatus& status)
{
  auto const& package = this->GetName();

  // Read components.
  auto const& components = this->Data["components"];

  for (auto ci = components.begin(), ce = components.end(); ci != ce; ++ci) {
    auto const& name = IterKey(ci);
    auto const& type = cmSystemTools::LowerCase((*ci)["type"].asString());

    // Get and validate full target name.
    auto const& fullName = cmStrCat(package, "::", name);
    {
      std::string msg;
      if (!makefile->EnforceUniqueName(fullName, msg)) {
        status.SetError(msg);
        return false;
      }
    }

    cmTarget* target = nullptr;
    if (type == "symbolic"_s) {
      // TODO
    } else if (type == "dylib"_s) {
      target = this->AddLibraryComponent(
        makefile, cmStateEnums::SHARED_LIBRARY, fullName, *ci, package);
    } else if (type == "module"_s) {
      target = this->AddLibraryComponent(
        makefile, cmStateEnums::MODULE_LIBRARY, fullName, *ci, package);
    } else if (type == "archive"_s) {
      target = this->AddLibraryComponent(
        makefile, cmStateEnums::STATIC_LIBRARY, fullName, *ci, package);
    } else if (type == "interface"_s) {
      target = this->AddLibraryComponent(
        makefile, cmStateEnums::INTERFACE_LIBRARY, fullName, *ci, package);
    } else {
      makefile->IssueMessage(MessageType::WARNING,
                             cmStrCat("component "_s, fullName,
                                      " has unknown type "_s, type,
                                      " and was not imported"));
    }

    if (target) {
      this->ComponentTargets.emplace(name, target);
    }
  }

  // Read default components.
  auto const& defaultComponents = ReadList(this->Data, "default_components");
  if (!defaultComponents.empty()) {
    std::string msg;
    if (!makefile->EnforceUniqueName(package, msg)) {
      status.SetError(msg);
      return false;
    }

    auto* const target = makefile->AddImportedTarget(
      package, cmStateEnums::INTERFACE_LIBRARY, false);
    for (auto const& name : defaultComponents) {
      auto const& fullName = cmStrCat(package, "::", name);
      AppendProperty(makefile, target, "LINK_LIBRARIES", {}, fullName);
    }
  }

  return true;
}

bool cmPackageInfoReader::ImportTargetConfigurations(
  cmMakefile* makefile, cmExecutionStatus& status) const
{
  auto const& configuration = this->Data["configuration"].asString();

  if (configuration.empty()) {
    makefile->IssueMessage(MessageType::WARNING,
                           cmStrCat("supplemental file "_s, this->Path,
                                    " does not specify a configuration"_s));
    return true;
  }

  auto const& package = this->GetName();
  auto const& components = this->Data["components"];

  for (auto ci = components.begin(), ce = components.end(); ci != ce; ++ci) {
    // Get component name and look up target.
    auto const& name = IterKey(ci);
    auto const& ti = this->ComponentTargets.find(std::string{ name });
    if (ti == this->ComponentTargets.end()) {
      status.SetError(cmStrCat("component "_s, name, " was not found"_s));
      return false;
    }

    // Read supplemental data for component.
    this->SetTargetProperties(makefile, ti->second, *ci, package,
                              configuration);
  }

  return true;
}
