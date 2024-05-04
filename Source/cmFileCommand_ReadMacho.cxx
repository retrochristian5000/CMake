/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#include "cmFileCommand_ReadMacho.h"

#include "cmArgumentParser.h"
#include "cmExecutionStatus.h"
#include "cmRange.h"
#include "cmStringAlgorithms.h"
#include "cmSystemTools.h"
#if defined(CMake_USE_MACH_PARSER)
#  include "cmMachO.h"
#  include "cmMakefile.h"
#endif

#include <cmext/string_view>

bool HandleReadMachoCommand(std::vector<std::string> const& args,
                            cmExecutionStatus& status)
{
  if (args.size() < 4) {
    status.SetError("READ_MACHO must be called with at least three additional "
                    "arguments.");
    return false;
  }

  std::string const& fileNameArg = args[1];

  struct Arguments
  {
    std::string Architectures;
    std::string Error;
  };

  static auto const parser = cmArgumentParser<Arguments>{}.Bind(
    "ARCHITECTURES"_s, &Arguments::Architectures);
  Arguments const arguments = parser.Parse(cmMakeRange(args).advance(2),
                                           /*unparsedArguments=*/nullptr);

  if (!cmSystemTools::FileExists(fileNameArg, true)) {
    status.SetError(cmStrCat("READ_MACHO given FILE \"", fileNameArg,
                             "\" that does not exist."));
    return false;
  }

#if defined(CMake_USE_MACH_PARSER)
  cmMachO macho(fileNameArg.c_str());
  if (!macho) {
    if (arguments.Error.empty()) {
      status.SetError(cmStrCat("READ_MACHO given FILE:\n  ", fileNameArg,
                               "\nthat is not a valid Macho-O file."));
      return false;
    }
    status.GetMakefile().AddDefinition(arguments.Error,
                                       "not a valid Macho-O file");
    return true;
  } else if (!macho.GetErrorMessage().empty()) {
    status.SetError(cmStrCat(
      "READ_MACHO given FILE:\n  ", fileNameArg,
      "\nthat is not a supported Macho-O file: ", macho.GetErrorMessage()));
    return false;
  }

  std::string output;

  if (!arguments.Architectures.empty()) {
    auto archs = macho.GetArchitectures();
    output = cmJoin(archs, ";");

    // Save the output in a makefile variable.
    status.GetMakefile().AddDefinition(arguments.Architectures, output);
  }

  return true;
#else
  return false;
#endif // CMake_USE_MACH_PARSER
}
