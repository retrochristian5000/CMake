/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#include "cmFileAPIToolchains.h"

#include <string>
#include <vector>

#include <cm3p/json/value.h>

#include "cmFileAPI.h"
#include "cmGlobalGenerator.h"
#include "cmMakefile.h"
#include "cmake.h"

namespace {

struct ToolchainVariable
{
  std::string ObjectKey;
  std::string VariableSuffix;
  bool IsList;
};

class Toolchains
{
  cmFileAPI& FileAPI;
  unsigned long Version;

  static const std::vector<ToolchainVariable> Variables;

  Json::Value DumpToolchains();
  Json::Value DumpToolchain(std::string const& lang);
  void DumpToolchainVariable(cmMakefile const* mf, Json::Value& object,
                             std::string const& lang,
                             ToolchainVariable const& variable);

public:
  Toolchains(cmFileAPI& fileAPI, unsigned long version);
  Json::Value Dump();
};

const std::vector<ToolchainVariable> Toolchains::Variables{
  { "compilerPath", "COMPILER", false },
  { "compilerId", "COMPILER_ID", false },
  { "compilerVersion", "COMPILER_VERSION", false },
  { "compilerTarget", "COMPILER_TARGET", false },
  { "implicitIncludeDirectories", "IMPLICIT_INCLUDE_DIRECTORIES", true },
  { "implicitLinkDirectories", "IMPLICIT_LINK_DIRECTORIES", true },
  { "implicitLinkFrameworkDirectories", "IMPLICIT_LINK_FRAMEWORK_DIRECTORIES",
    true },
  { "implicitLinkLibraries", "IMPLICIT_LINK_LIBRARIES", true },
  { "sourceFileExtensions", "SOURCE_FILE_EXTENSIONS", true }
};

Toolchains::Toolchains(cmFileAPI& fileAPI, unsigned long version)
  : FileAPI(fileAPI)
  , Version(version)
{
  static_cast<void>(this->Version);
}

Json::Value Toolchains::Dump()
{
  Json::Value toolchains = Json::objectValue;
  toolchains["toolchains"] = this->DumpToolchains();
  return toolchains;
}

Json::Value Toolchains::DumpToolchains()
{
  Json::Value toolchains = Json::arrayValue;

  for (std::string const& lang :
       this->FileAPI.GetCMakeInstance()->GetState()->GetEnabledLanguages()) {
    toolchains.append(this->DumpToolchain(lang));
  }

  return toolchains;
}

Json::Value Toolchains::DumpToolchain(std::string const& lang)
{
  const auto& mf =
    this->FileAPI.GetCMakeInstance()->GetGlobalGenerator()->GetMakefiles()[0];
  Json::Value toolchain = Json::objectValue;
  toolchain["language"] = lang;
  for (const auto& variable : Variables) {
    this->DumpToolchainVariable(mf.get(), toolchain, lang, variable);
  }
  return toolchain;
}

void Toolchains::DumpToolchainVariable(cmMakefile const* mf,
                                       Json::Value& object,
                                       std::string const& lang,
                                       ToolchainVariable const& variable)
{
  std::string const variableName =
    cmStrCat("CMAKE_", lang, "_", variable.VariableSuffix);

  if (variable.IsList) {
    std::vector<std::string> values;
    if (mf->GetDefExpandList(variableName, values)) {
      Json::Value jsonArray = Json::arrayValue;
      for (std::string const& value : values) {
        jsonArray.append(value);
      }
      object[variable.ObjectKey] = jsonArray;
    }
  } else {
    cmProp def = mf->GetDefinition(variableName);
    if (def) {
      object[variable.ObjectKey] = *def;
    }
  }
}
}

Json::Value cmFileAPIToolchainsDump(cmFileAPI& fileAPI, unsigned long version)
{
  Toolchains toolchains(fileAPI, version);
  return toolchains.Dump();
}
