/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */

#include "cmMakefileProfilingData.h"
#include "cmListFileCache.h"
#include "cm_jsoncpp_value.h"
#include "cm_jsoncpp_writer.h"
#include "cmsys/FStream.hxx"
#include "cmsys/SystemInformation.hxx"

#include <chrono>
#include <stdexcept>
#include <stdint.h>
#include <vector>

cmMakefileProfilingData::cmMakefileProfilingData(
  const std::string& profileStream)
{
  std::ios::openmode omode = std::ios::out | std::ios::trunc;
  this->ProfileStream.open(profileStream.c_str(), omode);
  Json::StreamWriterBuilder wbuilder;
  this->JsonWriter =
    std::unique_ptr<Json::StreamWriter>(wbuilder.newStreamWriter());
  if (!this->ProfileStream.good()) {
    throw std::runtime_error("Bad stream");
  }

  this->ProfileStream << "[";
};

cmMakefileProfilingData::~cmMakefileProfilingData()
{
  if (this->ProfileStream.good()) {
    this->ProfileStream << "]";
    this->ProfileStream.close();
  }
}

void cmMakefileProfilingData::StartEntry(const cmListFileFunction& lff,
                                         cmListFileContext const& lfc)
{
  if (this->ProfileStream.tellp() > 1) {
    this->ProfileStream << ",";
  }
  cmsys::SystemInformation info;
  Json::Value v;
  v["ph"] = "B";
  v["name"] = lff.Name.Original;
  v["cat"] = "cmake";
  v["ts"] = uint64_t(std::chrono::duration_cast<std::chrono::microseconds>(
                       std::chrono::steady_clock::now().time_since_epoch())
                       .count());
  v["pid"] = static_cast<int>(info.GetProcessId());
  v["tid"] = 0;
  Json::Value argsValue;
  if (!lff.Arguments.empty()) {
    std::string args;
    for (const auto& a : lff.Arguments) {
      args += (args.empty() ? "" : " ") + a.Value;
    }
    argsValue["functionArgs"] = args;
  }
  argsValue["location"] = lfc.FilePath + ":" + std::to_string(lfc.Line);
  v["args"] = argsValue;

  this->JsonWriter->write(v, &this->ProfileStream);
}

void cmMakefileProfilingData::StopEntry()
{
  this->ProfileStream << ",";
  cmsys::SystemInformation info;
  Json::Value v;
  v["ph"] = "E";
  v["ts"] = uint64_t(std::chrono::duration_cast<std::chrono::microseconds>(
                       std::chrono::steady_clock::now().time_since_epoch())
                       .count());
  v["pid"] = static_cast<int>(info.GetProcessId());
  v["tid"] = 0;
  this->JsonWriter->write(v, &this->ProfileStream);
}
