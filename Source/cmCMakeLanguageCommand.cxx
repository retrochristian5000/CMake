/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#include "cmCMakeLanguageCommand.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <memory>
#include <string>
#include <utility>

#include <cm/optional>
#include <cm/string_view>
#include <cmext/string_view>

#include "cmExecutionStatus.h"
#include "cmGlobalGenerator.h"
#include "cmListFileCache.h"
#include "cmMakefile.h"
#include "cmRange.h"
#include "cmStringAlgorithms.h"
#include "cmSystemTools.h"

namespace {
std::array<cm::static_string_view, 12> InvalidCommands{
  { // clang-format off
  "function"_s, "endfunction"_s,
  "macro"_s, "endmacro"_s,
  "if"_s, "elseif"_s, "else"_s, "endif"_s,
  "while"_s, "endwhile"_s,
  "foreach"_s, "endforeach"_s
  } // clang-format on
};

std::array<cm::static_string_view, 5> InvalidDeferCommands{
  {
    // clang-format off
  "add_subdirectory"_s,
  "enable_language"_s,
  "project"_s,
  "return"_s,
  "subdirs"_s,
  } // clang-format on
};

struct Defer
{
  std::string Id;
  std::string IdVar;
  cmMakefile* Directory = nullptr;
};

bool cmCMakeLanguageCommandCALL(std::vector<cmListFileArgument> const& args,
                                std::string const& callCommand,
                                size_t startArg, cm::optional<Defer> defer,
                                cmExecutionStatus& status)
{
  // ensure specified command is valid
  // start/end flow control commands are not allowed
  auto cmd = cmSystemTools::LowerCase(callCommand);
  if (std::find(InvalidCommands.cbegin(), InvalidCommands.cend(), cmd) !=
      InvalidCommands.cend()) {
    status.SetError(cmStrCat("invalid command specified: "_s, callCommand));
    return false;
  }
  if (defer &&
      std::find(InvalidDeferCommands.cbegin(), InvalidDeferCommands.cend(),
                cmd) != InvalidDeferCommands.cend()) {
    status.SetError(cmStrCat("invalid command specified: "_s, callCommand));
    return false;
  }

  cmMakefile& makefile = status.GetMakefile();
  cmListFileContext context = makefile.GetBacktrace().Top();

  cmListFileFunction func;
  func.Name = callCommand;
  func.Line = context.Line;

  // The rest of the arguments are passed to the function call above
  for (size_t i = startArg; i < args.size(); ++i) {
    cmListFileArgument lfarg;
    lfarg.Delim = args[i].Delim;
    lfarg.Line = context.Line;
    lfarg.Value = args[i].Value;
    func.Arguments.emplace_back(lfarg);
  }

  if (defer) {
    if (defer->Id.empty()) {
      defer->Id = makefile.NewDeferId();
    }
    if (!defer->IdVar.empty()) {
      makefile.AddDefinition(defer->IdVar, defer->Id);
    }
    cmMakefile* deferMakefile =
      defer->Directory ? defer->Directory : &makefile;
    if (!deferMakefile->DeferCall(defer->Id, context.FilePath, func)) {
      status.SetError(cmStrCat(
        "DEFER CALL may not be scheduled in directory:\n  "_s,
        deferMakefile->GetCurrentBinaryDirectory(), "\nat this time."_s));
      return false;
    }
    return true;
  }
  return makefile.ExecuteCommand(func, status);
}

bool cmCMakeLanguageCommandDEFER(Defer const& defer,
                                 std::vector<std::string> const& args,
                                 size_t arg, cmExecutionStatus& status)
{
  cmMakefile* deferMakefile =
    defer.Directory ? defer.Directory : &status.GetMakefile();
  if (args[arg] == "CANCEL_CALL"_s) {
    ++arg; // Consume CANCEL_CALL.
    auto ids = cmMakeRange(args).advance(arg);
    for (std::string const& id : ids) {
      if (id[0] >= 'A' && id[0] <= 'Z') {
        status.SetError(cmStrCat("DEFER CANCEL unknown argument:\n "_s, id));
        return false;
      }
      if (!deferMakefile->DeferCancelCall(id)) {
        status.SetError(cmStrCat(
          "DEFER CANCEL may not update directory:\n  "_s,
          deferMakefile->GetCurrentBinaryDirectory(), "\nat this time."_s));
        return false;
      }
    }
    return true;
  }
  if (args[arg] == "GET_CALL_IDS"_s) {
    ++arg; // Consume GET_CALL_IDS.
    if (arg == args.size()) {
      status.SetError("DEFER GET_CALL_IDS missing output variable");
      return false;
    }
    std::string const& var = args[arg++];
    if (arg != args.size()) {
      status.SetError("DEFER GET_CALL_IDS given too many arguments");
      return false;
    }
    cm::optional<std::string> ids = deferMakefile->DeferGetCallIds();
    if (!ids) {
      status.SetError(cmStrCat(
        "DEFER GET_CALL_IDS may not access directory:\n  "_s,
        deferMakefile->GetCurrentBinaryDirectory(), "\nat this time."_s));
      return false;
    }
    status.GetMakefile().AddDefinition(var, *ids);
    return true;
  }
  if (args[arg] == "GET_CALL"_s) {
    ++arg; // Consume GET_CALL.
    if (arg == args.size()) {
      status.SetError("DEFER GET_CALL missing id");
      return false;
    }
    std::string const& id = args[arg++];
    if (arg == args.size()) {
      status.SetError("DEFER GET_CALL missing output variable");
      return false;
    }
    std::string const& var = args[arg++];
    if (arg != args.size()) {
      status.SetError("DEFER GET_CALL given too many arguments");
      return false;
    }
    if (id.empty()) {
      status.SetError("DEFER GET_CALL id may not be empty");
      return false;
    }
    if (id[0] >= 'A' && id[0] <= 'Z') {
      status.SetError(cmStrCat("DEFER GET_CALL unknown argument:\n "_s, id));
      return false;
    }
    cm::optional<std::string> call = deferMakefile->DeferGetCall(id);
    if (!call) {
      status.SetError(cmStrCat(
        "DEFER GET_CALL may not access directory:\n  "_s,
        deferMakefile->GetCurrentBinaryDirectory(), "\nat this time."_s));
      return false;
    }
    status.GetMakefile().AddDefinition(var, *call);
    return true;
  }
  status.SetError(cmStrCat("DEFER operation unknown: "_s, args[arg]));
  return false;
}

bool cmCMakeLanguageCommandEVAL(std::vector<cmListFileArgument> const& args,
                                cmExecutionStatus& status)
{
  cmMakefile& makefile = status.GetMakefile();
  cmListFileContext context = makefile.GetBacktrace().Top();
  std::vector<std::string> expandedArgs;
  makefile.ExpandArguments(args, expandedArgs);

  if (expandedArgs.size() < 2) {
    status.SetError("called with incorrect number of arguments");
    return false;
  }

  if (expandedArgs[1] != "CODE") {
    auto code_iter =
      std::find(expandedArgs.begin() + 2, expandedArgs.end(), "CODE");
    if (code_iter == expandedArgs.end()) {
      status.SetError("called without CODE argument");
    } else {
      status.SetError(
        "called with unsupported arguments between EVAL and CODE arguments");
    }
    return false;
  }

  const std::string code =
    cmJoin(cmMakeRange(expandedArgs.begin() + 2, expandedArgs.end()), " ");
  return makefile.ReadListFileAsString(
    code, cmStrCat(context.FilePath, ":", context.Line, ":EVAL"));
}
}

bool cmCMakeLanguageCommand(std::vector<cmListFileArgument> const& args,
                            cmExecutionStatus& status)
{
  std::vector<std::string> expArgs;
  size_t rawArg = 0;
  size_t expArg = 0;

  // Helper to consume and expand one raw argument at a time.
  auto moreArgs = [&]() -> bool {
    while (expArg >= expArgs.size()) {
      if (rawArg >= args.size()) {
        return false;
      }
      std::vector<cmListFileArgument> tmpArg;
      tmpArg.emplace_back(args[rawArg++]);
      status.GetMakefile().ExpandArguments(tmpArg, expArgs);
    }
    return true;
  };
  auto finishArgs = [&]() {
    std::vector<cmListFileArgument> tmpArgs(args.begin() + rawArg, args.end());
    status.GetMakefile().ExpandArguments(tmpArgs, expArgs);
    rawArg = args.size();
  };

  if (!moreArgs()) {
    status.SetError("called with incorrect number of arguments");
    return false;
  }

  cm::optional<Defer> maybeDefer;
  if (expArgs[expArg] == "DEFER"_s) {
    ++expArg; // Consume "DEFER".

    if (!moreArgs()) {
      status.SetError("DEFER requires at least one argument");
      return false;
    }

    Defer defer;

    // Process optional arguments.
    while (moreArgs()) {
      if (expArgs[expArg] == "CALL"_s) {
        break;
      }
      if (expArgs[expArg] == "CANCEL_CALL"_s ||
          expArgs[expArg] == "GET_CALL_IDS"_s ||
          expArgs[expArg] == "GET_CALL"_s) {
        if (!defer.Id.empty() || !defer.IdVar.empty()) {
          status.SetError(cmStrCat("DEFER "_s, expArgs[expArg],
                                   " does not accept ID or ID_VAR."_s));
          return false;
        }
        finishArgs();
        return cmCMakeLanguageCommandDEFER(defer, expArgs, expArg, status);
      }
      if (expArgs[expArg] == "DIRECTORY"_s) {
        ++expArg; // Consume "DIRECTORY".
        if (defer.Directory) {
          status.SetError("DEFER given multiple DIRECTORY arguments");
          return false;
        }
        if (!moreArgs()) {
          status.SetError("DEFER DIRECTORY missing value");
          return false;
        }
        std::string dir = expArgs[expArg++];
        if (dir.empty()) {
          status.SetError("DEFER DIRECTORY may not be empty");
          return false;
        }
        dir = cmSystemTools::CollapseFullPath(
          dir, status.GetMakefile().GetCurrentSourceDirectory());
        defer.Directory =
          status.GetMakefile().GetGlobalGenerator()->FindMakefile(dir);
        if (!defer.Directory) {
          status.SetError(cmStrCat("DEFER DIRECTORY:\n  "_s, dir,
                                   "\nis not known.  "_s,
                                   "It may not have been processed yet."_s));
          return false;
        }
      } else if (expArgs[expArg] == "ID"_s) {
        ++expArg; // Consume "ID".
        if (!defer.Id.empty()) {
          status.SetError("DEFER given multiple ID arguments");
          return false;
        }
        if (!moreArgs()) {
          status.SetError("DEFER ID missing value");
          return false;
        }
        defer.Id = expArgs[expArg++];
        if (defer.Id.empty()) {
          status.SetError("DEFER ID may not be empty");
          return false;
        }
        if (defer.Id[0] >= 'A' && defer.Id[0] <= 'Z') {
          status.SetError("DEFER ID may not start in A-Z.");
          return false;
        }
      } else if (expArgs[expArg] == "ID_VAR"_s) {
        ++expArg; // Consume "ID_VAR".
        if (!defer.IdVar.empty()) {
          status.SetError("DEFER given multiple ID_VAR arguments");
          return false;
        }
        if (!moreArgs()) {
          status.SetError("DEFER ID_VAR missing variable name");
          return false;
        }
        defer.IdVar = expArgs[expArg++];
        if (defer.IdVar.empty()) {
          status.SetError("DEFER ID_VAR may not be empty");
          return false;
        }
      } else {
        status.SetError(
          cmStrCat("DEFER unknown option:\n  "_s, expArgs[expArg]));
        return false;
      }
    }

    if (expArgs[expArg] != "CALL"_s) {
      status.SetError("DEFER must be followed by a CALL argument");
      return false;
    }

    maybeDefer = std::move(defer);
  }

  if (expArgs[expArg] == "CALL") {
    ++expArg; // Consume "CALL".

    // CALL requires a command name.
    if (!moreArgs()) {
      status.SetError("CALL missing command name");
      return false;
    }
    std::string const& callCommand = expArgs[expArg++];

    // CALL accepts no further expanded arguments.
    if (expArg != expArgs.size()) {
      status.SetError("CALL command's arguments must be literal");
      return false;
    }

    // Run the CALL.
    return cmCMakeLanguageCommandCALL(args, callCommand, rawArg,
                                      std::move(maybeDefer), status);
  }

  if (expArgs[expArg] == "EVAL") {
    return cmCMakeLanguageCommandEVAL(args, status);
  }

  status.SetError("called with unknown meta-operation");
  return false;
}
