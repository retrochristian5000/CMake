/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#include "cmTargetHeadersCommand.h"

#include <algorithm>
#include <cassert>
#include <string>
#include <utility>
#include <vector>

#include <cmext/string_view>

#include "cmArgumentParser.h"
#include "cmGeneratorExpression.h"
#include "cmHeaderSet.h"
#include "cmListFileCache.h"
#include "cmMakefile.h"
#include "cmMessageType.h"
#include "cmStringAlgorithms.h"
#include "cmSystemTools.h"
#include "cmTarget.h"
#include "cmTargetPropCommandBase.h"

namespace {

struct HeadersArgs
{
  std::string HeaderSet;
  std::string Directory;
  std::vector<std::string> Headers;
};

auto const HeadersArgsParser = cmArgumentParser<HeadersArgs>()
                                 .Bind("HEADER_SET"_s, &HeadersArgs::HeaderSet)
                                 .Bind("DIRECTORY"_s, &HeadersArgs::Directory)
                                 .Bind("HEADERS"_s, &HeadersArgs::Headers);

class TargetHeadersImpl : public cmTargetPropCommandBase
{
public:
  using cmTargetPropCommandBase::cmTargetPropCommandBase;

private:
  void HandleMissingTarget(const std::string& name) override
  {
    this->Makefile->IssueMessage(
      MessageType::FATAL_ERROR,
      cmStrCat("Cannot specify headers for target \"", name,
               "\" which is not built by this project."));
  }

  std::string Join(const std::vector<std::string>& /*content*/) override
  {
    assert(false && "This shouldn't happen");
    return "";
  }

  bool PopulateTargetProperies(const std::string& scope,
                               const std::vector<std::string>& content,
                               bool /*prepend*/, bool /*system*/) override
  {
    enum class Scope
    {
      PRIVATE,
      PUBLIC,
      INTERFACE,
    };

    std::vector<std::string> unparsed;
    auto args = HeadersArgsParser.Parse(content, &unparsed);

    if (!unparsed.empty()) {
      this->SetError(
        cmStrCat("Unrecognized keyword: \"", unparsed.front(), "\""));
      return false;
    }

    bool const anonymous = args.HeaderSet.empty();

    if (anonymous && scope == "PRIVATE") {
      this->SetError("Anonymous header set cannot be PRIVATE");
      return false;
    }

    if (args.Directory.empty()) {
      args.Directory = this->Makefile->GetCurrentSourceDirectory();
    } else if (cmGeneratorExpression::Find(args.Directory) != 0 &&
               !cmSystemTools::FileIsFullPath(args.Directory)) {
      args.Directory = cmStrCat(this->Makefile->GetCurrentSourceDirectory(),
                                '/', args.Directory);
    }

    auto headerSet = this->Target->GetOrCreateHeaderSet(args.HeaderSet);
    if (headerSet.second) {
      if (anonymous) {
        this->Target->SetAnonymousHeaderSetIsInterface(scope == "INTERFACE");
        this->Target->AppendProperty(
          "INTERFACE_INCLUDE_DIRECTORIES",
          cmStrCat("$<BUILD_INTERFACE:", args.Directory, ">"));
        if (scope != "INTERFACE") {
          this->Target->AppendProperty(
            "INCLUDE_DIRECTORIES",
            cmStrCat("$<BUILD_INTERFACE:", args.Directory, ">"));
        }
      } else {
        if (scope == "PRIVATE" || scope == "PUBLIC") {
          this->Target->AppendProperty("HEADER_SETS", args.HeaderSet);
          this->Target->AppendProperty(
            "INCLUDE_DIRECTORIES",
            cmStrCat("$<BUILD_INTERFACE:", args.Directory, ">"));
        }
        if (scope == "INTERFACE" || scope == "PUBLIC") {
          this->Target->AppendProperty("INTERFACE_HEADER_SETS",
                                       args.HeaderSet);
          this->Target->AppendProperty(
            "INTERFACE_INCLUDE_DIRECTORIES",
            cmStrCat("$<BUILD_INTERFACE:", args.Directory, ">"));
        }
      }

      headerSet.first->SetDirectory(
        BT<std::string>(args.Directory, this->Makefile->GetBacktrace()));
    } else {
      std::string existingScope = "PRIVATE";

      if (anonymous) {
        existingScope = this->Target->GetAnonymousHeaderSetIsInterface()
          ? "INTERFACE"
          : "PUBLIC";
      } else {
        std::vector<std::string> headerSets;
        cmExpandList(this->Target->GetSafeProperty("HEADER_SETS"), headerSets);
        std::vector<std::string> interfaceHeaderSets;
        cmExpandList(this->Target->GetSafeProperty("INTERFACE_HEADER_SETS"),
                     interfaceHeaderSets);

        if (std::find(interfaceHeaderSets.begin(), interfaceHeaderSets.end(),
                      args.HeaderSet) != interfaceHeaderSets.end()) {
          existingScope = "INTERFACE";
        }
        if (std::find(headerSets.begin(), headerSets.end(), args.HeaderSet) !=
            headerSets.end()) {
          if (existingScope == "INTERFACE") {
            existingScope = "PUBLIC";
          }
        } else if (existingScope != "INTERFACE") {
          this->SetError(
            cmStrCat("Header set \"", args.HeaderSet,
                     "\" is not in HEADER_SETS or INTERFACE_HEADER_SETS"));
          return false;
        }
      }

      std::string headerSetDispay = anonymous
        ? "anonymous header set"
        : cmStrCat("header set \"", args.HeaderSet, "\"");
      if (scope != existingScope) {
        this->SetError(cmStrCat("Scope ", scope, " for ", headerSetDispay,
                                " does not match old scope ", existingScope));
        return false;
      }

      if (headerSet.first->GetDirectory().Value != args.Directory) {
        this->SetError(cmStrCat("Directory:\n  ", args.Directory, "\nfor ",
                                headerSetDispay,
                                " does not match old directory:\n  ",
                                headerSet.first->GetDirectory().Value));
        return false;
      }
    }

    headerSet.first->AddHeaderEntry(BT<std::string>(
      cmJoin(args.Headers, ";"_s), this->Makefile->GetBacktrace()));

    return true;
  }

  bool HandleDirectContent(cmTarget* /*tgt*/,
                           const std::vector<std::string>& /*content*/,
                           bool /*prepend*/, bool /*system*/) override
  {
    assert(false && "This shouldn't happen");
    return false;
  }
};
}

bool cmTargetHeadersCommand(std::vector<std::string> const& args,
                            cmExecutionStatus& status)
{
  return TargetHeadersImpl(status).HandleArguments(
    args, "", TargetHeadersImpl::NO_FLAGS);
}
