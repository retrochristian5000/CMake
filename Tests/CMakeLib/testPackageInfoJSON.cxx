/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file LICENSE.rst or https://cmake.org/licensing for details.  */
#include <limits>
#include <sstream>
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

#include "testCommon.h"

namespace {

namespace CPS = cmPackageInfo;

CPS::SchemaVersion const Version015(0, 15);

std::string const FullRoot = R"json({
  "cps_version": "0.15.0",
  "name": "Example",
  "cps_path": "@prefix@/lib/cps",
  "version": "1.0+local",
  "compat_version": "0.9",
  "version_schema": "simple",
  "description": "An example",
  "display_name": "Example Package",
  "website": "https://example.org",
  "license": "MIT",
  "default_license": "BSD-3-Clause",
  "meta_comment": "Common Package Specification",
  "meta_schema": "https://cps-org.github.io/cps/",
  "configurations": ["Release", "Debug"],
  "default_components": ["library"],
  "platform": {
    "isa": "x86_64", "kernel": "linux", "kernel_version": "6.0",
    "c_runtime_vendor": "gnu", "c_runtime_version": "2.0",
    "cpp_runtime_vendor": "gnu", "cpp_runtime_version": "12",
    "clr_vendor": "mono", "clr_version": "1",
    "jvm_vendor": "openjdk", "jvm_version": "17"
  },
  "requires": {
    "A": {},
    "B": {},
    "C": {
      "version": "2", "components": ["core"], "hints": ["../C"],
      "extensions": {"cmake": {"domains@v1": ["pkg-config", "future"]}}
    }
  },
  "components": {
    "library": {
      "type": "archive",
      "description": "Library",
      "license": "MIT",
      "includes": ["@prefix@/include", "relative"],
      "embeds": {"c": ["@prefix@/embed"]},
      "compile_flags": {"*": ["-all"], "cpp": ["-cpp"], "future": []},
      "definitions": {"*": {"NOVALUE": null, "EMPTY": "", "VALUE": "x=y"},
                      "cpp": {"VALUE": "override"}},
      "compile_features": ["c++20"],
      "link_features": ["thread"],
      "link_flags": ["-flag"],
      "link_directories": ["@prefix@/lib"],
      "link_languages": ["cpp"],
      "link_libraries": ["m"],
      "requires": ["C:core"],
      "compile_requires": [":headers"],
      "link_requires": ["C:link"],
      "dyld_requires": ["C:runtime"],
      "location": "@prefix@/lib/example.a",
      "link_location": "@prefix@/lib/example.lib",
      "link_name": "example",
      "cpp_module_metadata": "relative/modules.json",
      "configurations": {
        "Debug": {"location": "debug.a", "includes": null, "link_flags": []},
        "Release": {}
      },
      "file_sets": [{
        "type": "includes", "root": "@prefix@/include", "files": ["a.h"],
        "extensions": {"cmake": {"name@v1": "public_headers"}}
      }]
    },
    "headers": {"type": "interface", "includes": {}, "file_sets": []},
    "future": {"type": "future-type"}
  }
})json";

Json::Value Parse(std::string const& text)
{
  Json::Value value;
  std::istringstream input(text);
  cmJSONState state(input, &value);
  return value;
}

std::string Write(Json::Value const& value)
{
  return Json::writeString(Json::StreamWriterBuilder(), value);
}

template <typename T>
bool HasError(CPS::Result<T> const& result,
              std::string const& message = std::string())
{
  if (result.Value) {
    std::cout << "Unexpected success; expected error: " << message << '\n';
    return false;
  }
  if (!result.Error.empty() &&
      result.Error.find(message) != std::string::npos) {
    return true;
  }
  std::cout << "Missing error: " << message << '\n' << result.Error << '\n';
  return false;
}

bool TestRoot()
{
  auto decoded = CPS::DecodeRoot(FullRoot);
  ASSERT_TRUE(decoded);
  ASSERT_TRUE(decoded.Error.empty());
  auto const& library = decoded.Value->Components.at("library");
  ASSERT_TRUE(library.Usage.Includes.Get()->GetList());
  ASSERT_TRUE(library.Usage.CompileFlags.Get()->GetLanguages());
  auto const& definitions = library.Usage.Definitions.Get()->at("*");
  ASSERT_TRUE(!definitions.at("NOVALUE"));
  ASSERT_TRUE(definitions.at("EMPTY"));
  ASSERT_EQUAL(*definitions.at("EMPTY"), "");
  ASSERT_EQUAL(*library.Usage.Location.Get(), "@prefix@/lib/example.a");
  ASSERT_TRUE(!library.Usage.Location.IsNull());
  ASSERT_TRUE(library.Configurations.Get()->at("Debug").Includes.IsNull());
  ASSERT_TRUE(
    !library.Configurations.Get()->at("Release").Includes.IsPresent());
  ASSERT_EQUAL(decoded.Value->Components.at("future").Type, "future-type");
  auto encoded = CPS::Encode(*decoded.Value);
  ASSERT_TRUE(encoded);
  ASSERT_TRUE(Parse(*encoded.Value) == Parse(FullRoot));
  auto again = CPS::DecodeRoot(*encoded.Value);
  ASSERT_TRUE(again);
  auto reencoded = CPS::Encode(*again.Value);
  ASSERT_TRUE(reencoded);
  ASSERT_EQUAL(*encoded.Value, *reencoded.Value);
  return true;
}

bool TestConstructedModel()
{
  CPS::Root root;
  root.Name = "Constructed";
  root.CpsVersion = CPS::SchemaVersion(0, 13);
  root.CpsPath = std::string("@prefix@");
  root.Metadata.Description = std::string();
  root.Requires = CPS::Requirements();
  root.Configurations = CPS::StringList();
  CPS::Component component;
  component.Type = "interface";
  component.Usage.Includes = CPS::LanguageStringLists(CPS::LanguageMap());
  component.Usage.CompileFlags = CPS::LanguageStringLists(CPS::StringList());
  component.Usage.Definitions = CPS::LanguageDefinitions();
  component.Usage.LinkFlags = CPS::Field<CPS::StringList>::Null();
  root.Components.emplace("headers", std::move(component));
  auto encoded = CPS::Encode(root);
  ASSERT_TRUE(encoded);
  auto json = Parse(*encoded.Value);
  ASSERT_TRUE(json["requires"].isObject());
  ASSERT_TRUE(json["configurations"].isArray());
  ASSERT_TRUE(json["description"].isString());
  ASSERT_TRUE(json["components"]["headers"]["includes"].isObject());
  ASSERT_TRUE(json["components"]["headers"]["compile_flags"].isArray());
  ASSERT_TRUE(json["components"]["headers"]["definitions"].isObject());
  ASSERT_TRUE(json["components"]["headers"].isMember("link_flags"));
  ASSERT_TRUE(!json["components"]["headers"].isMember("location"));
  auto decoded = CPS::DecodeRoot(*encoded.Value);
  ASSERT_TRUE(decoded);
  ASSERT_TRUE(
    decoded.Value->Components.at("headers").Usage.LinkFlags.IsNull());

  root.Components.clear();
  encoded = CPS::Encode(root);
  ASSERT_TRUE(encoded);
  ASSERT_TRUE(Parse(*encoded.Value)["components"].isObject());
  root.CpsVersion = CPS::SchemaVersion(0, 16);
  ASSERT_TRUE(HasError(CPS::Encode(root), "cps_version"));
  root.CpsVersion = Version015;
  root.Prefix = std::string("/prefix");
  ASSERT_TRUE(HasError(CPS::Encode(root), ""));
  return true;
}

bool TestLanguageStringLists()
{
  CPS::LanguageStringLists empty;
  ASSERT_TRUE(empty.GetList());
  ASSERT_TRUE(empty.GetList()->empty());
  ASSERT_TRUE(!empty.GetLanguages());

  CPS::StringList const list{ "first", "second" };
  CPS::LanguageMap const languages{ { "cpp", { "-cpp" } }, { "*", {} } };
  CPS::LanguageStringLists const alternatives[]{
    CPS::LanguageStringLists(list), CPS::LanguageStringLists(languages)
  };
  for (auto const& source : alternatives) {
    CPS::LanguageStringLists copy(source);
    ASSERT_TRUE((copy.GetList() != nullptr) == (source.GetList() != nullptr));
    ASSERT_TRUE((copy.GetLanguages() != nullptr) ==
                (source.GetLanguages() != nullptr));
    CPS::LanguageStringLists moved(std::move(copy));
    if (source.GetList()) {
      ASSERT_TRUE(*moved.GetList() == list);
      ASSERT_TRUE(copy.GetList());
      moved.GetList()->push_back("copy only");
      ASSERT_TRUE(*source.GetList() == list);
    } else {
      ASSERT_TRUE(*moved.GetLanguages() == languages);
      ASSERT_TRUE(copy.GetLanguages());
      (*moved.GetLanguages())["cpp"].push_back("copy only");
      ASSERT_TRUE(*source.GetLanguages() == languages);
    }

    // Exercise both same-alternative and cross-alternative assignment.
    for (auto const& initial : alternatives) {
      CPS::LanguageStringLists assigned(initial);
      assigned = source;
      if (source.GetList()) {
        ASSERT_TRUE(!assigned.GetLanguages());
        ASSERT_TRUE(*assigned.GetList() == list);
      } else {
        ASSERT_TRUE(!assigned.GetList());
        ASSERT_TRUE(*assigned.GetLanguages() == languages);
      }
      assigned = initial;
      CPS::LanguageStringLists toMove(source);
      assigned = std::move(toMove);
      if (source.GetList()) {
        ASSERT_TRUE(!assigned.GetLanguages());
        ASSERT_TRUE(*assigned.GetList() == list);
        ASSERT_TRUE(toMove.GetList());
      } else {
        ASSERT_TRUE(!assigned.GetList());
        ASSERT_TRUE(*assigned.GetLanguages() == languages);
        ASSERT_TRUE(toMove.GetLanguages());
      }

      auto* alias = &assigned;
      assigned = *alias;
      assigned = std::move(*alias);
      if (source.GetList()) {
        ASSERT_TRUE(*assigned.GetList() == list);
      } else {
        ASSERT_TRUE(*assigned.GetLanguages() == languages);
      }
    }
  }
  return true;
}

bool TestDocumentKinds()
{
  std::string const appendix = R"({"name":"Example", "cps_version":"0.13.0",
    "default_license":null, "requires":{"A":null},
    "components":{"extra":{"type":"symbolic"}}})";
  auto decoded = CPS::DecodeAppendix(appendix, Version015);
  ASSERT_TRUE(decoded);
  auto encoded = CPS::Encode(*decoded.Value, Version015);
  ASSERT_TRUE(encoded);
  auto expected = Parse(appendix);
  expected["requires"]["A"] = Json::Value(Json::objectValue);
  ASSERT_TRUE(Parse(*encoded.Value) == expected);
  ASSERT_TRUE(!decoded.Value->Requires.Get()->at("A").Version.IsPresent());
  ASSERT_TRUE(!CPS::DecodeRoot(appendix));
  auto inherited = CPS::DecodeAppendix(R"({"name":"Example","components":{}})",
                                       CPS::SchemaVersion(0, 14));
  ASSERT_TRUE(inherited);
  ASSERT_TRUE(!inherited.Value->CpsVersion.IsPresent());
  ASSERT_TRUE(!CPS::DecodeAppendix(appendix, CPS::SchemaVersion(0, 16)));

  std::string const supplement = R"({"name":"Example","configuration":"Debug",
    "components":{"library":{"location":"@prefix@/debug.a","includes":null}}})";
  auto config = CPS::DecodeConfigurationSupplement(supplement, Version015);
  ASSERT_TRUE(config);
  auto configEncoded = CPS::Encode(*config.Value, Version015);
  ASSERT_TRUE(configEncoded);
  ASSERT_TRUE(Parse(*configEncoded.Value) == Parse(supplement));
  ASSERT_TRUE(!CPS::DecodeAppendix(supplement, Version015));
  auto json = Parse(supplement);
  for (char const* field :
       { "type", "configurations", "file_sets", "license", "description" }) {
    auto invalid = json;
    invalid["components"]["library"][field] = Json::Value();
    ASSERT_TRUE(HasError(
      CPS::DecodeConfigurationSupplement(Write(invalid), Version015), field));
  }
  for (char const* field : { "cps_version", "prefix", "cps_path", "requires",
                             "version", "default_license", "platform" }) {
    auto invalid = json;
    invalid[field] = Json::Value();
    ASSERT_TRUE(HasError(
      CPS::DecodeConfigurationSupplement(Write(invalid), Version015), field));
  }
  json.removeMember("configuration");
  ASSERT_TRUE(
    HasError(CPS::DecodeConfigurationSupplement(Write(json), Version015),
             "configuration"));
  return true;
}

bool TestInvalidTypes()
{
  auto root = Parse(FullRoot);
  for (auto const& key : root.getMemberNames()) {
    auto invalid = root;
    invalid[key] = 42;
    ASSERT_TRUE(HasError(CPS::DecodeRoot(Write(invalid)), key));
  }
  auto const& library = root["components"]["library"];
  for (auto const& key : library.getMemberNames()) {
    auto invalid = root;
    invalid["components"]["library"][key] = true;
    ASSERT_TRUE(HasError(CPS::DecodeRoot(Write(invalid)), key));
  }
  for (char const* type : { "null", "[]", "1", "true", "\"string\"" }) {
    ASSERT_TRUE(!CPS::DecodeRoot(type));
  }
  for (char const* key : { "name", "components", "cps_version" }) {
    auto invalid = root;
    invalid.removeMember(key);
    ASSERT_TRUE(HasError(CPS::DecodeRoot(Write(invalid)), key));
    invalid[key] = Json::Value();
    ASSERT_TRUE(HasError(CPS::DecodeRoot(Write(invalid)), key));
  }
  for (char const* definitions :
       { "[]", R"({"cpp":[]})", R"({"cpp":{"X":[]}})", R"({"cpp":{"X":42}})",
         R"({"cpp":{"X":true}})" }) {
    auto invalid = root;
    invalid["components"]["library"]["definitions"] = Parse(definitions);
    ASSERT_TRUE(!CPS::DecodeRoot(Write(invalid)));
  }
  auto invalid = root;
  invalid["components"]["library"]["includes"][0] = 42;
  ASSERT_TRUE(HasError(CPS::DecodeRoot(Write(invalid)), "expected a string"));
  invalid = root;
  invalid["components"]["library"]["compile_flags"]["cpp"] = "not an array";
  ASSERT_TRUE(
    HasError(CPS::DecodeRoot(Write(invalid)), "\"cpp\" expected an array"));
  invalid = root;
  invalid["components"]["library"]["definitions"]["a/b~c"]["X"] = 42;
  ASSERT_TRUE(
    HasError(CPS::DecodeRoot(Write(invalid)), "\"X\" expected a string"));
  invalid = root;
  invalid["components"]["library"]["configurations"]["Debug"] = 42;
  ASSERT_TRUE(!CPS::DecodeRoot(Write(invalid)));
  invalid = root;
  invalid["components"]["library"]["file_sets"][0].removeMember("root");
  ASSERT_TRUE(HasError(CPS::DecodeRoot(Write(invalid)), "root"));
  invalid = root;
  invalid["requires"]["C"]["extensions"]["cmake"]["domains@v1"][0] = false;
  ASSERT_TRUE(HasError(CPS::DecodeRoot(Write(invalid)), "expected a string"));
  return true;
}

bool CheckNestedTypes(Json::Value& root, Json::Value& value)
{
  // No supported field in this fixture accepts a boolean. Mutate every node,
  // not just the top-level attributes, to exercise all decoding boundaries.
  Json::Value original = value;
  value = false;
  ASSERT_TRUE(HasError(CPS::DecodeRoot(Write(root))));
  value = std::move(original);
  if (value.isObject()) {
    for (auto const& key : value.getMemberNames()) {
      ASSERT_TRUE(CheckNestedTypes(root, value[key]));
    }
  } else if (value.isArray()) {
    for (Json::ArrayIndex i = 0; i < value.size(); ++i) {
      ASSERT_TRUE(CheckNestedTypes(root, value[i]));
    }
  }
  return true;
}

bool TestAllNestedTypes()
{
  auto root = Parse(FullRoot);
  return CheckNestedTypes(root, root);
}

bool TestNullAndExtensions()
{
  auto root = Parse(FullRoot);
  for (auto const& key : root.getMemberNames()) {
    if (key != "name" && key != "cps_version" && key != "components" &&
        key != "cps_path") {
      auto json = root;
      json[key] = Json::Value();
      auto decoded = CPS::DecodeRoot(Write(json));
      ASSERT_TRUE(decoded);
      auto encoded = CPS::Encode(*decoded.Value);
      ASSERT_TRUE(encoded);
      ASSERT_TRUE(Parse(*encoded.Value) == json);
    }
  }
  for (auto const& key : root["components"]["library"].getMemberNames()) {
    if (key != "type") {
      auto json = root;
      json["components"]["library"][key] = Json::Value();
      auto decoded = CPS::DecodeRoot(Write(json));
      ASSERT_TRUE(decoded);
      auto encoded = CPS::Encode(*decoded.Value);
      ASSERT_TRUE(encoded);
      ASSERT_TRUE(Parse(*encoded.Value) == json);
    }
  }
  root["unknown"] = Json::Value(Json::arrayValue);
  root["components"]["library"]["unknown"] = 42;
  root["requires"]["C"]["extensions"]["foreign"] = 42;
  ASSERT_TRUE(CPS::DecodeRoot(Write(root)));
  root["requires"]["C"]["extensions"]["cmake"] = 42;
  ASSERT_TRUE(HasError(CPS::DecodeRoot(Write(root)), "cmake"));
  return true;
}

bool TestVersionsAndDiagnostics()
{
  auto root = Parse(FullRoot);
  for (char const* version : { "0.13", "0.14.0", "0.15.123" }) {
    root["cps_version"] = version;
    ASSERT_TRUE(CPS::DecodeRoot(Write(root)));
  }
  for (char const* version : { "",
                               "0",
                               "0.12",
                               "0.16",
                               "1.0",
                               "0.15.",
                               "0.15x",
                               "0.15.0junk",
                               "0.15.1.2",
                               "999999999999999999",
                               "0.999999999999999999.0",
                               "0.15.9999999999999999999999",
                               "0.15.-1",
                               "0.15.+1",
                               " 0.15",
                               "0.15.0 ",
                               "0..15",
                               ".0.15",
                               "0.15..1",
                               "0.15. 1",
                               "0.+15.0",
                               "\t0.15",
                               "0.15\n" }) {
    root["cps_version"] = version;
    ASSERT_TRUE(HasError(CPS::DecodeRoot(Write(root)), "cps_version"));
  }
  root["cps_version"] = std::string("0.15.0\0junk", 11);
  ASSERT_TRUE(HasError(CPS::DecodeRoot(Write(root)), "cps_version"));
  auto malformed = CPS::DecodeRoot("{");
  ASSERT_TRUE(!malformed);
  ASSERT_TRUE(!malformed.Error.empty());
  auto wrongType =
    CPS::DecodeRoot("{\n  \"name\": 42, \"cps_version\": \"0.15\","
                    "\"cps_path\": \"@prefix@\",\"components\":{}}");
  ASSERT_TRUE(HasError(wrongType, "\"name\" expected a string"));
  ASSERT_TRUE(wrongType.Error.find("  \"name\": 42") != std::string::npos);
  ASSERT_TRUE(wrongType.Error.find("\n          ^") != std::string::npos);
  ASSERT_TRUE(CPS::DecodeRoot(cmStrCat("/* comment */", FullRoot)));
  root = Parse(FullRoot);
  root["name"] = "";
  auto emptyName = CPS::DecodeRoot(Write(root));
  ASSERT_TRUE(HasError(emptyName, "Package has an invalid or empty CPS name"));
  root = Parse(FullRoot);
  root["prefix"] = "/prefix";
  ASSERT_TRUE(HasError(CPS::DecodeRoot(Write(root)), ""));
  root.removeMember("prefix");
  root["cps_path"] = "@prefix@not-a-path";
  ASSERT_TRUE(HasError(CPS::DecodeRoot(Write(root)), "cps_path"));
  return true;
}

bool TestSchemaVersion()
{
  auto root = Parse(FullRoot);
  root["cps_version"] = "0.13";
  auto decoded = CPS::DecodeRoot(Write(root));
  ASSERT_TRUE(decoded);
  ASSERT_EQUAL(decoded.Value->CpsVersion.Major, 0u);
  ASSERT_EQUAL(decoded.Value->CpsVersion.Minor, 13u);
  ASSERT_EQUAL(decoded.Value->CpsVersion.Patch, 0u);
  auto encoded = CPS::Encode(*decoded.Value);
  ASSERT_TRUE(encoded);
  ASSERT_EQUAL(Parse(*encoded.Value)["cps_version"].asString(), "0.13.0");

  unsigned const max = (std::numeric_limits<unsigned>::max)();
  std::string const maxText = std::to_string(max);
  root["cps_version"] = cmStrCat("0.15.", maxText);
  decoded = CPS::DecodeRoot(Write(root));
  ASSERT_TRUE(decoded);
  ASSERT_EQUAL(decoded.Value->CpsVersion.Patch, max);
  encoded = CPS::Encode(*decoded.Value);
  ASSERT_TRUE(encoded);
  ASSERT_EQUAL(Parse(*encoded.Value)["cps_version"].asString(),
               cmStrCat("0.15.", maxText));
  for (std::string const& overflow :
       { cmStrCat(maxText, "0.15.0"), cmStrCat("0.", maxText, "0.0"),
         cmStrCat("0.15.", maxText, '0') }) {
    root["cps_version"] = overflow;
    ASSERT_TRUE(HasError(CPS::DecodeRoot(Write(root)), "cps_version"));
  }

  auto appendix = CPS::DecodeAppendix(
    R"({"name":"Example","cps_version":"0.14","components":{}})", Version015);
  ASSERT_TRUE(appendix);
  ASSERT_EQUAL(appendix.Value->CpsVersion.Get()->Minor, 14u);
  encoded = CPS::Encode(*appendix.Value, Version015);
  ASSERT_TRUE(encoded);
  ASSERT_EQUAL(Parse(*encoded.Value)["cps_version"].asString(), "0.14.0");
  appendix = CPS::DecodeAppendix(
    R"({"name":"Example","cps_version":null,"components":{}})", Version015);
  ASSERT_TRUE(appendix);
  ASSERT_TRUE(appendix.Value->CpsVersion.IsNull());
  encoded = CPS::Encode(*appendix.Value, Version015);
  ASSERT_TRUE(encoded);
  ASSERT_TRUE(Parse(*encoded.Value)["cps_version"].isNull());
  ASSERT_TRUE(HasError(
    CPS::DecodeAppendix(
      R"({"name":"Example","cps_version":"0.15.bad","components":{}})",
      Version015),
    "cps_version"));
  appendix.Value->CpsVersion = CPS::SchemaVersion(1, 15);
  ASSERT_TRUE(
    HasError(CPS::Encode(*appendix.Value, Version015), "cps_version"));
  return true;
}

} // namespace

int testPackageInfoJSON(int, char*[])
{
  return runTests({ TestRoot, TestConstructedModel, TestLanguageStringLists,
                    TestDocumentKinds, TestInvalidTypes, TestAllNestedTypes,
                    TestNullAndExtensions, TestVersionsAndDiagnostics,
                    TestSchemaVersion });
}
