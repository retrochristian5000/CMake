#include <iostream>
#include <string>
#include <vector>

#include "cmCTestResourceSpec.h"

struct ExpectedSpec
{
  std::string Path;
  cmCTestResourceSpec::ReadFileResult ParseResult;
  cmCTestResourceSpec Expected;
};

static const std::vector<ExpectedSpec> expectedResourceSpecs = {
  /* clang-format off */
  {"spec1.json", cmCTestResourceSpec::OK, {{{
    {"gpus", {
      {"2", 4},
      {"e", 1},
    }},
    {"threads", {
    }},
  }}}},
  {"spec2.json", cmCTestResourceSpec::OK, {}},
  {"spec3.json", cmCTestResourceSpec::INVALID_SOCKET_SPEC, {}},
  {"spec4.json", cmCTestResourceSpec::INVALID_SOCKET_SPEC, {}},
  {"spec5.json", cmCTestResourceSpec::INVALID_SOCKET_SPEC, {}},
  {"spec6.json", cmCTestResourceSpec::INVALID_SOCKET_SPEC, {}},
  {"spec7.json", cmCTestResourceSpec::INVALID_RESOURCE_TYPE, {}},
  {"spec8.json", cmCTestResourceSpec::INVALID_RESOURCE, {}},
  {"spec9.json", cmCTestResourceSpec::INVALID_RESOURCE, {}},
  {"spec10.json", cmCTestResourceSpec::INVALID_RESOURCE, {}},
  {"spec11.json", cmCTestResourceSpec::INVALID_RESOURCE, {}},
  {"spec12.json", cmCTestResourceSpec::INVALID_ROOT, {}},
  {"spec13.json", cmCTestResourceSpec::JSON_PARSE_ERROR, {}},
  {"spec14.json", cmCTestResourceSpec::OK, {}},
  {"spec15.json", cmCTestResourceSpec::OK, {}},
  {"spec16.json", cmCTestResourceSpec::OK, {}},
  {"spec17.json", cmCTestResourceSpec::INVALID_RESOURCE, {}},
  {"spec18.json", cmCTestResourceSpec::INVALID_RESOURCE, {}},
  {"spec19.json", cmCTestResourceSpec::INVALID_VERSION, {}},
  {"spec20.json", cmCTestResourceSpec::OK, {}},
  {"spec21.json", cmCTestResourceSpec::INVALID_VERSION, {}},
  {"spec22.json", cmCTestResourceSpec::INVALID_VERSION, {}},
  {"spec23.json", cmCTestResourceSpec::INVALID_VERSION, {}},
  {"spec24.json", cmCTestResourceSpec::INVALID_VERSION, {}},
  {"spec25.json", cmCTestResourceSpec::UNSUPPORTED_VERSION, {}},
  {"spec26.json", cmCTestResourceSpec::UNSUPPORTED_VERSION, {}},
  {"spec27.json", cmCTestResourceSpec::INVALID_VERSION, {}},
  {"spec28.json", cmCTestResourceSpec::INVALID_VERSION, {}},
  {"spec29.json", cmCTestResourceSpec::INVALID_VERSION, {}},
  {"spec30.json", cmCTestResourceSpec::INVALID_VERSION, {}},
  {"spec31.json", cmCTestResourceSpec::INVALID_VERSION, {}},
  {"spec32.json", cmCTestResourceSpec::INVALID_VERSION, {}},
  {"spec33.json", cmCTestResourceSpec::INVALID_VERSION, {}},
  {"spec34.json", cmCTestResourceSpec::INVALID_VERSION, {}},
  {"spec35.json", cmCTestResourceSpec::INVALID_VERSION, {}},
  {"spec36.json", cmCTestResourceSpec::NO_VERSION, {}},
  {"noexist.json", cmCTestResourceSpec::FILE_NOT_FOUND, {}},
  /* clang-format on */
};

static bool testSpec(const std::string& path, bool expectedResult,
                     const cmCTestResourceSpec& expected)
{
  cmCTestResourceSpec actual;
  bool result = actual.ReadFromJSONFile(path);
  if (result != expectedResult) {
    std::cout << "ReadFromJSONFile(\"" << path << "\") returned " << result
              << ", should be " << expectedResult << std::endl;
    return false;
  }

  if (result && actual != expected) {
    std::cout << "ReadFromJSONFile(\"" << path
              << "\") did not give expected spec" << std::endl;
    return false;
  }

  return true;
}

int testCTestResourceSpec(int argc, char** const argv)
{
  if (argc < 2) {
    std::cout << "Invalid arguments.\n";
    return -1;
  }

  int retval = 0;
  for (auto const& spec : expectedResourceSpecs) {
    std::string path = argv[1];
    path += "/testCTestResourceSpec_data/";
    path += spec.Path;
    if (!testSpec(path, spec.ParseResult, spec.Expected)) {
      retval = -1;
    }
  }

  return retval;
}
