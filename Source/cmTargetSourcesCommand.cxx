/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file LICENSE.rst or https://cmake.org/licensing for details.  */
#include "cmTargetSourcesCommand.h"

#include <algorithm>
#include <utility>

#include <cm/optional>
#include <cm/string_view>
#include <cmext/string_view>

#include "cmArgumentParser.h"
#include "cmArgumentParserTypes.h"
#include "cmExperimental.h"
#include "cmFileSet.h"
#include "cmFileSetMetadata.h"
#include "cmGeneratorExpression.h"
#include "cmList.h"
#include "cmListFileCache.h"
#include "cmMakefile.h"
#include "cmMessageType.h"
#include "cmPolicies.h"
#include "cmRange.h"
#include "cmStringAlgorithms.h"
#include "cmSystemTools.h"
#include "cmTarget.h"
#include "cmTargetPropCommandBase.h"
#include "cmTargetTypes.h"

namespace {

struct FileSetArgs
{
  std::string Type;
  std::string FileSet;
  ArgumentParser::MaybeEmpty<std::vector<std::string>> BaseDirs;
  ArgumentParser::MaybeEmpty<std::vector<std::string>> Files;
};

auto const FileSetArgsParser = cmArgumentParser<FileSetArgs>()
                                 .Bind("TYPE"_s, &FileSetArgs::Type)
                                 .Bind("FILE_SET"_s, &FileSetArgs::FileSet)
                                 .Bind("BASE_DIRS"_s, &FileSetArgs::BaseDirs)
                                 .Bind("FILES"_s, &FileSetArgs::Files);

struct FileSetsArgs
{
  ArgumentParser::MaybeEmpty<std::vector<std::vector<std::string>>> FileSets;
};

auto const FileSetsArgsParser =
  cmArgumentParser<FileSetsArgs>().Bind("FILE_SET"_s, &FileSetsArgs::FileSets);

struct InteropHeaderArgs
{
  // Language and Filename are positional arguments
  ArgumentParser::MaybeEmpty<std::string> FileSet;
  ArgumentParser::MaybeEmpty<std::string> BaseDir;
};

auto const InteropHeaderArgsParser =
  cmArgumentParser<InteropHeaderArgs>()
    .Bind("FILE_SET"_s, &InteropHeaderArgs::FileSet)
    .Bind("BASE_DIR"_s, &InteropHeaderArgs::BaseDir);

struct InteropHeaderListArgs
{
  ArgumentParser::MaybeEmpty<std::vector<std::vector<std::string>>>
    InteropHeaders;
};

auto const InteropHeaderListArgsParser =
  cmArgumentParser<InteropHeaderListArgs>().Bind(
    "GENERATE_INTEROP_HEADER"_s, &InteropHeaderListArgs::InteropHeaders);

class TargetSourcesImpl : public cmTargetPropCommandBase
{
public:
  using cmTargetPropCommandBase::cmTargetPropCommandBase;

protected:
  void HandleInterfaceContent(cmTarget* tgt,
                              std::vector<std::string> const& content,
                              bool prepend, bool system) override
  {
    this->cmTargetPropCommandBase::HandleInterfaceContent(
      tgt,
      this->ConvertToAbsoluteContent(tgt, content, IsInterface::Yes,
                                     CheckCMP0076::Yes),
      prepend, system);
  }

private:
  void HandleMissingTarget(std::string const& name) override
  {
    this->Makefile->IssueMessage(
      MessageType::FATAL_ERROR,
      cmStrCat("Cannot specify sources for target \""_s, name,
               "\" which is not built by this project."_s));
  }

  bool HandleDirectContent(cmTarget* tgt,
                           std::vector<std::string> const& content,
                           bool /*prepend*/, bool /*system*/) override
  {
    tgt->AppendProperty("SOURCES",
                        this->Join(this->ConvertToAbsoluteContent(
                          tgt, content, IsInterface::No, CheckCMP0076::Yes)),
                        this->Makefile->GetBacktrace());
    return true; // Successfully handled.
  }

  bool PopulateTargetProperties(std::string const& scope,
                                std::vector<std::string> const& content,
                                bool prepend, bool system) override
  {
    if (!content.empty() && content.front() == "FILE_SET"_s) {
      return this->HandleFileSetMode(scope, content);
    }
    if (!content.empty() && content.front() == "GENERATE_INTEROP_HEADER"_s) {
      return this->HandleInteropHeaderMode(scope, content);
    }
    return this->cmTargetPropCommandBase::PopulateTargetProperties(
      scope, content, prepend, system);
  }

  std::string Join(std::vector<std::string> const& content) override
  {
    return cmList::to_string(content);
  }

  enum class IsInterface
  {
    Yes,
    No,
  };
  enum class CheckCMP0076
  {
    Yes,
    No,
  };

  enum class TargetSourceDir
  {
    CurrentSourceDir,
    CurrentBinaryDir,
  };
  std::vector<std::string> ConvertToAbsoluteContent(
    cmTarget* tgt, std::vector<std::string> const& content,
    IsInterface isInterfaceContent, CheckCMP0076 checkCmp0076,
    TargetSourceDir outputDir = TargetSourceDir::CurrentSourceDir);

  bool HandleFileSetMode(std::string const& scope,
                         std::vector<std::string> const& content);
  bool HandleOneFileSet(std::string const& scope,
                        std::vector<std::string> const& content);
  bool HandleInteropHeaderMode(std::string const& scope,
                               std::vector<std::string> const& content);
  bool HandleInteropHeader(std::string const& scope,
                           std::vector<std::string> const& content);
};

std::vector<std::string> TargetSourcesImpl::ConvertToAbsoluteContent(
  cmTarget* tgt, std::vector<std::string> const& content,
  IsInterface isInterfaceContent, CheckCMP0076 checkCmp0076,
  TargetSourceDir outputDir)
{
  // Skip conversion in case old behavior has been explicitly requested
  if (checkCmp0076 == CheckCMP0076::Yes &&
      this->Makefile->GetPolicyStatus(cmPolicies::CMP0076) ==
        cmPolicies::OLD) {
    return content;
  }

  std::string const& makefileRootDir =
    outputDir == TargetSourceDir::CurrentSourceDir
    ? this->Makefile->GetCurrentSourceDirectory()
    : this->Makefile->GetCurrentBinaryDirectory();

  std::string const& targetRootDir =
    outputDir == TargetSourceDir::CurrentSourceDir
    ? tgt->GetMakefile()->GetCurrentSourceDirectory()
    : tgt->GetMakefile()->GetCurrentBinaryDirectory();

  bool changedPath = false;
  std::vector<std::string> absoluteContent;
  absoluteContent.reserve(content.size());
  for (std::string const& src : content) {
    std::string absoluteSrc;
    if (cmSystemTools::FileIsFullPath(src) ||
        cmGeneratorExpression::Find(src) == 0 ||
        (isInterfaceContent == IsInterface::No &&
         (makefileRootDir == targetRootDir))) {
      absoluteSrc = src;
    } else {
      changedPath = true;
      absoluteSrc = cmStrCat(makefileRootDir, '/', src);
    }
    absoluteContent.push_back(absoluteSrc);
  }

  if (!changedPath) {
    return content;
  }

  if (checkCmp0076 == CheckCMP0076::Yes) {
    switch (this->Makefile->GetPolicyStatus(cmPolicies::CMP0076)) {
      case cmPolicies::WARN:
        if (isInterfaceContent == IsInterface::Yes) {
          this->Makefile->IssuePolicyWarning(
            cmPolicies::CMP0076, {},
            cmStrCat("An interface source of target \""_s, tgt->GetName(),
                     "\" has a relative path."_s));
        } else {
          this->Makefile->IssuePolicyWarning(
            cmPolicies::CMP0076, {},
            cmStrCat("A private source from a directory "
                     "other than that of target \""_s,
                     tgt->GetName(), "\" has a relative path."_s));
        }
        CM_FALLTHROUGH;
      case cmPolicies::OLD:
        return content;
      case cmPolicies::NEW:
        break;
    }
  }

  return absoluteContent;
}

bool TargetSourcesImpl::HandleFileSetMode(
  std::string const& scope, std::vector<std::string> const& content)
{
  auto args = FileSetsArgsParser.Parse(content, /*unparsedArguments=*/nullptr);

  for (auto& argList : args.FileSets) {
    argList.emplace(argList.begin(), "FILE_SET"_s);
    if (!this->HandleOneFileSet(scope, argList)) {
      return false;
    }
  }

  return true;
}

bool TargetSourcesImpl::HandleOneFileSet(
  std::string const& scope, std::vector<std::string> const& content)
{
  std::vector<std::string> unparsed;
  auto args = FileSetArgsParser.Parse(content, &unparsed);

  if (!unparsed.empty()) {
    this->SetError(
      cmStrCat("Unrecognized keyword: \""_s, unparsed.front(), "\"."_s));
    return false;
  }

  if (args.FileSet.empty()) {
    this->SetError("FILE_SET must not be empty.");
    return false;
  }

  if (!args.Type.empty() && !cm::FileSetMetadata::IsKnownType(args.Type) &&
      // rule must be known from the target directory
      !this->Makefile->FindRuleToUse(args.Type)) {
    this->SetError(
      cmStrCat("File set TYPE may only be one of the built-in types \""_s,
               cmJoin(cm::FileSetMetadata::GetKnownTypes(), "\", \""),
               "\" or a RULE visible from the current directory."_s));
    return false;
  }
  if (args.Type.empty() && args.FileSet[0] >= 'A' && args.FileSet[0] <= 'Z' &&
      !cm::FileSetMetadata::IsKnownType(args.FileSet)) {
    this->SetError(cmStrCat(
      "FILE_SET names starting with a capital letter are reserved "
      "for built-in file sets and may only be \""_s,
      cmJoin(cm::FileSetMetadata::GetKnownTypes(), "\", \""), "\"."_s));
    return false;
  }
  auto const fileSetDescriptor =
    cm::FileSetMetadata::GetFileSetDescriptor(args.FileSet);
  bool const isCanonicalAlias = !args.Type.empty() && fileSetDescriptor &&
    fileSetDescriptor->Type == args.Type;
  if (!args.Type.empty() && args.FileSet[0] >= 'A' && args.FileSet[0] <= 'Z' &&
      args.Type != args.FileSet && !isCanonicalAlias) {
    this->SetError(cmStrCat("FILE_SET name starting with a capital letter "
                            "must match the TYPE name \""_s,
                            args.Type, "\"."_s));
    return false;
  }

  bool const isDefault = isCanonicalAlias || args.Type == args.FileSet ||
    (args.Type.empty() && cm::FileSetMetadata::IsKnownType(args.FileSet));

  if (!isDefault && !cm::FileSetMetadata::IsValidName(args.FileSet)) {
    this->SetError("Non-default file set name must contain only letters, "
                   "numbers, and underscores, and must not start with a "
                   "capital letter or underscore.");
    return false;
  }

  std::string requestedType = isDefault ? args.FileSet : args.Type;
  bool const isSycl = requestedType == cm::FileSetMetadata::SYCL ||
    requestedType == cm::FileSetMetadata::SYCL_HEADERS;
  if (isSycl &&
      !cmExperimental::HasSupportEnabled(*this->Makefile,
                                         cmExperimental::Feature::SYCL)) {
    this->SetError(cmStrCat("File set TYPE \""_s, requestedType,
                            "\" is experimental and must be enabled by "
                            "the CMAKE_EXPERIMENTAL_SYCL gate."_s));
    return false;
  }

  std::string type = requestedType;
  if (auto const descriptor =
        cm::FileSetMetadata::GetFileSetDescriptor(requestedType)) {
    type = std::string{ descriptor->Type };
  }
  if (cm::FileSetMetadata::IsKnownType(type) &&
      this->Target->GetType() == cm::TargetType::UTILITY) {
    this->SetError(
      cmStrCat("FILE_SETs of type \""_s,
               cmJoin(cm::FileSetMetadata::GetKnownTypes(), "\", \""),
               "\" may not be added to custom targets."_s));
    return false;
  }

  cm::FileSetMetadata::Visibility visibility =
    cm::FileSetMetadata::VisibilityFromName(scope, this->Makefile);

  if (this->Target->IsFrameworkOnApple() &&
      !cm::FileSetMetadata::IsFrameworkSupported(type)) {
    this->SetError(cmStrCat(R"(FILE_SETs, of type ")"_s, type,
                            R"(", may not be added to FRAMEWORK targets.)"_s));
    return false;
  }

  auto fileSet = this->Target->GetOrCreateFileSet(args.FileSet, type,
                                                  visibility, this->Makefile);
  if (fileSet.second) {
    if (type.empty()) {
      this->SetError("Must specify a TYPE when creating file set.");
      return false;
    }

    if (cm::FileSetMetadata::VisibilityIsForSelf(visibility) &&
        this->Target->GetType() == cm::TargetType::INTERFACE_LIBRARY &&
        !this->Target->IsImported()) {
      if (type == cm::FileSetMetadata::CXX_MODULES) {
        this->SetError(
          cmStrCat(R"(File set TYPE ")"_s, cm::FileSetMetadata::CXX_MODULES,
                   R"(" may not have "PUBLIC" )"
                   R"(or "PRIVATE" scope on INTERFACE libraries.)"_s));
        return false;
      }
    }

    // FIXME(https://wg21.link/P3470): This condition can go
    // away when interface-only module units are a thing.
    if (cm::FileSetMetadata::VisibilityIsForInterface(visibility) &&
        !cm::FileSetMetadata::VisibilityIsForSelf(visibility) &&
        !this->Target->IsImported()) {
      if (type == cm::FileSetMetadata::CXX_MODULES) {
        this->SetError(cmStrCat(R"(File set TYPE ")"_s,
                                cm::FileSetMetadata::CXX_MODULES,
                                R"(" may not have "INTERFACE" scope.)"_s));
        return false;
      }
    }

    if (cm::FileSetMetadata::VisibilityIsForSelf(visibility) &&
        this->Target->GetType() == cm::TargetType::INTERFACE_LIBRARY &&
        type == cm::FileSetMetadata::SOURCES) {
      this->SetError(
        cmStrCat(R"(File set TYPE ")"_s, cm::FileSetMetadata::SOURCES,
                 R"(" may not have "PUBLIC" )"
                 R"(or "PRIVATE" scope on INTERFACE libraries.)"_s));
      return false;
    }

    if (args.BaseDirs.empty()) {
      args.BaseDirs.emplace_back(this->Makefile->GetCurrentSourceDirectory());
    }
  } else {
    type = fileSet.first->GetType();
    std::string specifiedType = args.Type;
    if (auto const descriptor =
          cm::FileSetMetadata::GetFileSetDescriptor(specifiedType)) {
      specifiedType = std::string{ descriptor->Type };
    }
    if (!args.Type.empty() && specifiedType != type) {
      this->SetError(cmStrCat("Type \""_s, args.Type, "\" for file set \""_s,
                              fileSet.first->GetName(),
                              "\" does not match original type \""_s, type,
                              "\"."_s));
      return false;
    }

    if (visibility != fileSet.first->GetVisibility()) {
      this->SetError(cmStrCat(
        "Scope "_s, scope, " for file set \""_s, args.FileSet,
        "\" does not match original scope "_s,
        cm::FileSetMetadata::VisibilityToName(fileSet.first->GetVisibility()),
        '.'));
      return false;
    }
  }

  if (isSycl) {
    fileSet.first->SetProperty("LANGUAGE", "SYCL");
  }

  auto files = this->Join(this->ConvertToAbsoluteContent(
    this->Target, args.Files, IsInterface::Yes, CheckCMP0076::No));
  if (!files.empty()) {
    fileSet.first->AddFileEntry(
      BT<std::string>(files, this->Makefile->GetBacktrace()));
  }

  auto baseDirectories = this->Join(this->ConvertToAbsoluteContent(
    this->Target, args.BaseDirs, IsInterface::Yes, CheckCMP0076::No));
  if (!baseDirectories.empty()) {
    fileSet.first->AddDirectoryEntry(
      BT<std::string>(baseDirectories, this->Makefile->GetBacktrace()));
    if (type == cm::FileSetMetadata::HEADERS) {
      for (auto const& dir : cmList{ baseDirectories }) {
        auto interfaceDirectoriesGenex =
          cmStrCat("$<BUILD_INTERFACE:"_s, dir, '>');
        if (cm::FileSetMetadata::VisibilityIsForSelf(visibility)) {
          this->Target->AppendProperty("INCLUDE_DIRECTORIES",
                                       interfaceDirectoriesGenex,
                                       this->Makefile->GetBacktrace());
        }
        if (cm::FileSetMetadata::VisibilityIsForInterface(visibility)) {
          this->Target->AppendProperty("INTERFACE_INCLUDE_DIRECTORIES",
                                       interfaceDirectoriesGenex,
                                       this->Makefile->GetBacktrace());
        }
      }
    }
  }

  return true;
}

bool TargetSourcesImpl::HandleInteropHeaderMode(
  std::string const& scope, std::vector<std::string> const& content)
{
  auto args =
    InteropHeaderListArgsParser.Parse(content, /*unparsedArguments=*/nullptr);
  return std::all_of(args.InteropHeaders.begin(), args.InteropHeaders.end(),
                     [&](auto const& argList) {
                       return this->HandleInteropHeader(scope, argList);
                     });
}

bool TargetSourcesImpl::HandleInteropHeader(
  std::string const& scope, std::vector<std::string> const& content)
{
  if (content.size() < 2) {
    this->SetError("GENERATE_INTEROP_HEADER must specify a language and "
                   "generated header filename");
    return false;
  }

  if (this->Target->GetType() == cm::TargetType::UTILITY) {
    this->SetError(
      "GENERATE_INTEROP_HEADER may not be added to custom targets");
    return false;
  }

  if (this->Target->GetType() == cm::TargetType::INTERFACE_LIBRARY) {
    this->SetError(
      "GENERATE_INTEROP_HEADER may not be added to interface targets");
    return false;
  }

  std::string const lang = content[0];
  std::string const filename = content[1];

  std::vector<std::string> unparsed;
  auto args =
    InteropHeaderArgsParser.Parse(cmMakeRange(content).advance(2), &unparsed);

  if (!unparsed.empty()) {
    this->SetError(
      cmStrCat("Unrecognized keyword: \"", unparsed.front(), '"'));
    return false;
  }

  if (lang != "Swift") {
    this->SetError(cmStrCat(
      "GENERATE_INTEROP_HEADER only implemented for Swift, received \"", lang,
      '"'));
    return false;
  }

  if (cm::GeneratedHeader const* interopHeader =
        this->Target->GetLanguageCInteropHeader(lang)) {
    this->SetError(cmStrCat("Language interop header already set on target \"",
                            this->Target->GetName(), "\" to \"",
                            interopHeader->path, '"'));
    return false;
  }

  if (args.BaseDir.empty()) {
    args.BaseDir = this->Makefile->GetCurrentBinaryDirectory();
  }

  std::string const absoluteBaseDir =
    this
      ->ConvertToAbsoluteContent(this->Target, { args.BaseDir },
                                 IsInterface::Yes, CheckCMP0076::No,
                                 TargetSourceDir::CurrentBinaryDir)
      .front();

  std::string const absoluteFilename =
    this
      ->ConvertToAbsoluteContent(this->Target, { filename }, IsInterface::Yes,
                                 CheckCMP0076::No,
                                 TargetSourceDir::CurrentBinaryDir)
      .front();

  cm::FileSetMetadata::Visibility visibility =
    cm::FileSetMetadata::VisibilityFromName(scope, this->Makefile);

  if (!args.FileSet.empty()) {
    std::string const type = cm::FileSetMetadata::IsKnownType(args.FileSet)
      ? args.FileSet
      : std::string(cm::FileSetMetadata::HEADERS);

    auto foundFileSet =
      this->Target->GetOrCreateFileSet(args.FileSet, type, visibility);
    cmFileSet* fileSet = foundFileSet.first;

    if (!foundFileSet.second) {
      if (fileSet->GetType() != cm::FileSetMetadata::HEADERS) {
        this->SetError(
          cmStrCat("FILE_SET \"", args.FileSet,
                   "\" must be of TYPE HEADERS for GENERATE_INTEROP_HEADER"));
        return false;
      }

      if (fileSet->GetVisibility() != visibility) {
        this->SetError(cmStrCat("Scope ", scope, " for file set \"",
                                args.FileSet,
                                "\" does not match its original scope"));
        return false;
      }
    }
    fileSet->AddDirectoryEntry(
      BT<std::string>(absoluteBaseDir, this->Makefile->GetBacktrace()));
    fileSet->AddFileEntry(
      BT<std::string>(absoluteFilename, this->Makefile->GetBacktrace()));
  }

  this->Target->SetLanguageCInteropHeader(lang,
                                          { absoluteFilename, visibility });

  auto interfaceDirectoryGenex =
    cmStrCat("$<BUILD_INTERFACE:", absoluteBaseDir, '>');
  if (cm::FileSetMetadata::VisibilityIsForSelf(visibility)) {
    this->Target->AppendProperty("INCLUDE_DIRECTORIES",
                                 interfaceDirectoryGenex,
                                 this->Makefile->GetBacktrace());
  }
  if (cm::FileSetMetadata::VisibilityIsForInterface(visibility)) {
    this->Target->AppendProperty("INTERFACE_INCLUDE_DIRECTORIES",
                                 interfaceDirectoryGenex,
                                 this->Makefile->GetBacktrace());
  }

  return true;
}

} // namespace

bool cmTargetSourcesCommand(std::vector<std::string> const& args,
                            cmExecutionStatus& status)
{
  return TargetSourcesImpl(status).HandleArguments(args, "SOURCES");
}
