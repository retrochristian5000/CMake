/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#include "cmCTestGenericHandler.h"

#include <sstream>
#include <utility>

#include "cmCTest.h"
#include "cmStringAlgorithms.h"
#include "cmSystemTools.h"

cmCTestGenericHandler::cmCTestGenericHandler()
{
  this->HandlerVerbose = cmSystemTools::OUTPUT_NONE;
  this->CTest = nullptr;
  this->SubmitIndex = 0;
  this->AppendXML = false;
  this->Quiet = false;
  this->TestLoad = 0;
}

cmCTestGenericHandler::~cmCTestGenericHandler() = default;

/* Modify the given `map`, setting key `op` to `value` if `value`
 * is non-null, otherwise removing key `op` (if it exists).
 */
static void SetMapValue(cmCTestGenericHandler::t_StringToString& map,
                        const std::string& op, const char* value)
{
  if (!value) {
    auto remit = map.find(op);
    if (remit != map.end()) {
      map.erase(remit);
    }
    return;
  }

  map[op] = value;
}

void cmCTestGenericHandler::SetOption(const std::string& op, const char* value)
{
  SetMapValue(this->Options, op, value);
}

void cmCTestGenericHandler::SetPersistentOption(const std::string& op,
                                                const char* value)
{
  this->SetOption(op, value);
  SetMapValue(this->PersistentOptions, op, value);
}

/* Modify the given `map`, doing nothing if `value` is null. Otherwise,
 * set key `op` to `value` if key `op` does not exist in the map,
 * or append `value` to the existing value in the map if it does.
 */
static void SetMapMultiValue(cmCTestGenericHandler::t_StringToString& map,
                             const std::string& op, const char* value)
{
  if (value) {
    auto remit = map.find(op);
    if (remit != map.end()) {
      auto extended = remit->second + char(0) + std::string(value);
      map[op] = extended;
    } else {
      map[op] = value;
    }
  }
}

void cmCTestGenericHandler::AddMultiOption(const std::string& op,
                                           const char* value)
{
  SetMapMultiValue(this->Options, op, value);
}

void cmCTestGenericHandler::AddPersistentMultiOption(const std::string& op,
                                                     const char* value)
{
  this->AddMultiOption(op, value);
  SetMapMultiValue(this->PersistentOptions, op, value);
}

void cmCTestGenericHandler::Initialize()
{
  this->AppendXML = false;
  this->TestLoad = 0;
  this->Options.clear();
  for (auto const& po : this->PersistentOptions) {
    this->Options[po.first] = po.second;
  }
}

const char* cmCTestGenericHandler::GetOption(const std::string& op)
{
  auto remit = this->Options.find(op);
  if (remit == this->Options.end()) {
    return nullptr;
  }
  return remit->second.c_str();
}

std::vector<const char*> cmCTestGenericHandler::GetMultiOption(
  const std::string& op)
{
  auto remit = this->Options.find(op);
  if (remit == this->Options.end()) {
    return std::vector<const char*>();
  }

  std::vector<const char*> parts;
  auto* str = remit->second.c_str();
  size_t c = 0;
  while ((c != std::string::npos) && (c < remit->second.length())) {
    parts.push_back(str + c);
    // Find next NUL-separator and step over it
    c = remit->second.find(char(0), c);
    if ((c != std::string::npos) && ((c + 1) < remit->second.length()))
      c++;
  }
  return parts;
}

bool cmCTestGenericHandler::StartResultingXML(cmCTest::Part part,
                                              const char* name,
                                              cmGeneratedFileStream& xofs)
{
  if (!name) {
    cmCTestLog(this->CTest, ERROR_MESSAGE,
               "Cannot create resulting XML file without providing the name"
                 << std::endl;);
    return false;
  }
  std::ostringstream ostr;
  ostr << name;
  if (this->SubmitIndex > 0) {
    ostr << "_" << this->SubmitIndex;
  }
  ostr << ".xml";
  if (this->CTest->GetCurrentTag().empty()) {
    cmCTestLog(this->CTest, ERROR_MESSAGE,
               "Current Tag empty, this may mean NightlyStartTime / "
               "CTEST_NIGHTLY_START_TIME was not set correctly. Or "
               "maybe you forgot to call ctest_start() before calling "
               "ctest_configure()."
                 << std::endl);
    cmSystemTools::SetFatalErrorOccured();
    return false;
  }
  if (!this->CTest->OpenOutputFile(this->CTest->GetCurrentTag(), ostr.str(),
                                   xofs, true)) {
    cmCTestLog(this->CTest, ERROR_MESSAGE,
               "Cannot create resulting XML file: " << ostr.str()
                                                    << std::endl);
    return false;
  }
  this->CTest->AddSubmitFile(part, ostr.str());
  return true;
}

bool cmCTestGenericHandler::StartLogFile(const char* name,
                                         cmGeneratedFileStream& xofs)
{
  if (!name) {
    cmCTestLog(this->CTest, ERROR_MESSAGE,
               "Cannot create log file without providing the name"
                 << std::endl;);
    return false;
  }
  std::ostringstream ostr;
  ostr << "Last" << name;
  if (this->SubmitIndex > 0) {
    ostr << "_" << this->SubmitIndex;
  }
  if (!this->CTest->GetCurrentTag().empty()) {
    ostr << "_" << this->CTest->GetCurrentTag();
  }
  ostr << ".log";
  this->LogFileNames[name] =
    cmStrCat(this->CTest->GetBinaryDir(), "/Testing/Temporary/", ostr.str());
  if (!this->CTest->OpenOutputFile("Temporary", ostr.str(), xofs)) {
    cmCTestLog(this->CTest, ERROR_MESSAGE,
               "Cannot create log file: " << ostr.str() << std::endl);
    return false;
  }
  return true;
}
