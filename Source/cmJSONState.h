/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#pragma once

#include <cm/memory>

#include <cm3p/json/reader.h>
#include <cm3p/json/value.h>

#include "cmsys/FStream.hxx"

#include "cmJSONState.h"
#include "cmStringAlgorithms.h"

class JsonState
{
  using Location = struct
  {
    int line;
    int column;
  };

public:
  using JsonPair = std::pair<const std::string, const Json::Value*>;
  JsonState() = default;
  JsonState(const std::string& filename, Json::Value* root)
  {
    cmsys::ifstream fin(filename.c_str());
    if (!fin) {
      this->AddError(cmStrCat("File not found: ", filename));
      return;
    }
    Json::CharReaderBuilder builder;
    Json::CharReaderBuilder::strictMode(&builder.settings_);
    std::ostringstream ssin;
    ssin << fin.rdbuf();
    this->doc = ssin.str();
    if (this->doc.empty()) {
      this->AddError("A JSON document cannot be empty");
      return;
    }
    std::string errMsg;
    cmsys::ifstream fin_(filename.c_str());
    if (!Json::parseFromStream(builder, fin_, root, &errMsg)) {
      errMsg = cmStrCat("JSON Parse Error: ", filename, ":\n", errMsg);
      this->AddError(errMsg);
    }
  };
  void AddError(std::string const& errMsg)
  {
    this->errors.push_back(Error(errMsg));
  }
  void AddErrorAtValue(std::string const& errMsg, const Json::Value* value)
  {
#if !defined(CMAKE_BOOTSTRAP)
    if (value && !value->isNull()) {
      this->AddErrorAtOffset(errMsg, value->getOffsetStart());
    } else {
      this->AddError(errMsg);
    }
#endif
  }
  void AddErrorAtOffset(std::string const& errMsg, ptrdiff_t offset)
  {
    if (doc.empty()) {
      this->AddError(errMsg);
    } else {
      Location loc = LocateInDocument(offset);
      this->errors.push_back(Error(loc, errMsg));
    }
  };
  std::string GetErrorMessage(bool showContext = true)
  {
    std::string message;
    for (auto const& error : this->errors) {
      message = cmStrCat(message, error.GetErrorMessage(), "\n");
      if (showContext) {
        Location loc = error.GetLocation();
        if (loc.column > 0) {
          message = cmStrCat(message, GetJsonContext(loc), "\n");
        }
      }
    }
    message = cmStrCat("\n", message);
    message.pop_back();
    return message;
  }

  std::string key()
  {
    if (!this->parseStack.empty()) {
      return this->parseStack.back().first;
    }
    return "";
  }
  std::string key_after(const std::string& key)
  {
    for (auto it = this->parseStack.begin(); it != this->parseStack.end();
         ++it) {
      if (it->first == key && (++it) != this->parseStack.end()) {
        return it->first;
      }
    }
    return "";
  }
  const Json::Value* value_after(const std::string& key)
  {
    for (auto it = this->parseStack.begin(); it != this->parseStack.end();
         ++it) {
      if (it->first == key && (++it) != this->parseStack.end()) {
        return it->second;
      }
    }
    return nullptr;
  }
  void push_stack(const std::string& key, const Json::Value* value)
  {
    this->parseStack.push_back(JsonPair(key, value));
  }
  void pop_stack() { this->parseStack.pop_back(); }

  class Error
  {
  public:
    Error(Location loc, std::string errMsg)
      : location(loc)
      , message(std::move(errMsg)){};
    Error(std::string errMsg)
      : location({ -1, -1 })
      , message(std::move(errMsg)){};
    std::string GetErrorMessage() const
    {
      std::string output = message;
      if (location.line > 0) {
        output = cmStrCat("Error: @", location.line, ",", location.column,
                          ": ", output);
      }
      return output;
    }
    Location GetLocation() const { return location; }

  private:
    Location location;
    std::string message;
  };

  std::vector<JsonPair> parseStack;
  std::vector<Error> errors;
  std::string doc;

private:
  std::string GetJsonContext(Location loc)
  {
    std::string line;
    std::stringstream sstream(doc);
    for (int i = 0; i < loc.line; ++i) {
      std::getline(sstream, line, '\n');
    }
    return cmStrCat(line, '\n', std::string(loc.column - 1, ' '), '^');
  }
  Location LocateInDocument(ptrdiff_t offset)
  {
    int line = 1;
    int col = 1;
    const char* beginDoc = doc.data();
    const char* last = beginDoc + offset;
    for (; beginDoc != last; ++beginDoc) {
      switch (*beginDoc) {
        case '\r':
          if (beginDoc + 1 != last && beginDoc[1] == '\n') {
            continue; // consume CRLF as a single token.
          }
          [[fallthrough]]; // CR without a following LF is same as LF
        case '\n':
          col = 1;
          ++line;
          break;
        default:
          ++col;
          break;
      }
    }
    return { line, col };
  }
};
