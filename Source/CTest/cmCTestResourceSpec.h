/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#pragma once

#include "cmConfigure.h" // IWYU pragma: keep

#include <map>
#include <string>
#include <vector>

#include "cmJSONHelpers.h"
#include "cmJSONState.h"

namespace Json {
class Value;
}

class cmCTestResourceSpec
{
public:
  class Resource
  {
  public:
    std::string Id;
    unsigned int Capacity;

    bool operator==(const Resource& other) const;
    bool operator!=(const Resource& other) const;
  };

  class Socket
  {
  public:
    std::map<std::string, std::vector<Resource>> Resources;

    bool operator==(const Socket& other) const;
    bool operator!=(const Socket& other) const;
  };

  Socket LocalSocket;
  JsonState parseState;

  bool ReadFromJSONFile(const std::string& filename);

  bool operator==(const cmCTestResourceSpec& other) const;
  bool operator!=(const cmCTestResourceSpec& other) const;
};

namespace cmCTestResourceSpecErrors {
using ErrorGenerator = JsonErrors::ErrorGenerator;
const ErrorGenerator FILE_NOT_FOUND = [](const Json::Value*,
                                         JsonState* state) {
  state->AddError("File not found");
};
const ErrorGenerator JSON_PARSE_ERROR = [](const Json::Value* value,
                                           JsonState* state) {
  state->AddErrorAtValue("JSON parse error", value);
};
const ErrorGenerator INVALID_ROOT = [](const Json::Value* value,
                                       JsonState* state) {
  state->AddErrorAtValue("Invalid root object", value);
};
const ErrorGenerator NO_VERSION = [](const Json::Value* value,
                                     JsonState* state) {
  state->AddErrorAtValue("No version specified", value);
};
const ErrorGenerator INVALID_VERSION = [](const Json::Value* value,
                                          JsonState* state) {
  state->AddErrorAtValue("Invalid version object", value);
};
const ErrorGenerator UNSUPPORTED_VERSION = [](const Json::Value* value,
                                              JsonState* state) {
  state->AddErrorAtValue("Unsupported version", value);
};
const ErrorGenerator INVALID_SOCKET_SPEC = [](const Json::Value* value,
                                              JsonState* state) {
  state->AddErrorAtValue("Invalid socket object", value);
};
const ErrorGenerator INVALID_RESOURCE_TYPE = [](const Json::Value* value,
                                                JsonState* state) {
  state->AddErrorAtValue("Invalid resource type object", value);
};
const ErrorGenerator INVALID_RESOURCE = [](const Json::Value* value,
                                           JsonState* state) {
  state->AddErrorAtValue("Invalid resource object", value);
};
}
