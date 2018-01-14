/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#include "cmQtAutoGen.h"
#include "cmQtAutoGeneratorRcc.h"

#include "cmAlgorithms.h"
#include "cmCryptoHash.h"
#include "cmMakefile.h"
#include "cmSystemTools.h"
#include "cmUVHandlePtr.h"

#include <functional>

// -- Class methods

cmQtAutoGeneratorRcc::cmQtAutoGeneratorRcc()
  : SettingsChanged_(false)
  , MultiConfig_(cmQtAutoGen::WRAP)
  , Stage_(StageT::SETTINGS_READ)
  , Error_(false)
  , Generate_(false)
  , Generated_(false)
{
  // Initialize libuv asynchronous iteration request
  UVRequest().init(*UVLoop(), &cmQtAutoGeneratorRcc::UVPollStage, this);
}

cmQtAutoGeneratorRcc::~cmQtAutoGeneratorRcc()
{
}

bool cmQtAutoGeneratorRcc::Init(cmMakefile* makefile)
{
  // Utility lambdas
  auto InfoGet = [makefile](const char* key) {
    return makefile->GetSafeDefinition(key);
  };
  auto InfoGetList = [makefile](const char* key) -> std::vector<std::string> {
    std::vector<std::string> list;
    cmSystemTools::ExpandListArgument(makefile->GetSafeDefinition(key), list);
    return list;
  };
  auto InfoGetConfig = [makefile, this](const char* key) -> std::string {
    const char* valueConf = nullptr;
    {
      std::string keyConf = key;
      keyConf += '_';
      keyConf += InfoConfig();
      valueConf = makefile->GetDefinition(keyConf);
    }
    if (valueConf == nullptr) {
      valueConf = makefile->GetSafeDefinition(key);
    }
    return std::string(valueConf);
  };
  auto InfoGetConfigList =
    [&InfoGetConfig](const char* key) -> std::vector<std::string> {
    std::vector<std::string> list;
    cmSystemTools::ExpandListArgument(InfoGetConfig(key), list);
    return list;
  };

  // -- Read info file
  if (!makefile->ReadListFile(InfoFile().c_str())) {
    Log().ErrorFile(cmQtAutoGen::RCC, InfoFile(), "File processing failed");
    return false;
  }

  // -- Meta
  MultiConfig_ = cmQtAutoGen::MultiConfigType(InfoGet("ARCC_MULTI_CONFIG"));
  ConfigSuffix_ = InfoGetConfig("ARCC_CONFIG_SUFFIX");
  if (ConfigSuffix_.empty()) {
    ConfigSuffix_ = "_";
    ConfigSuffix_ += InfoConfig();
  }

  SettingsFile_ = InfoGetConfig("ARCC_SETTINGS_FILE");

  // - Files and directories
  AutogenBuildDir_ = InfoGet("ARCC_BUILD_DIR");

  // - Qt environment
  RccExecutable_ = InfoGet("ARCC_RCC_EXECUTABLE");
  RccListOptions_ = InfoGetList("ARCC_RCC_LIST_OPTIONS");

  // - Job
  QrcFile_ = InfoGet("ARCC_SOURCE");
  QrcFileName_ = cmSystemTools::GetFilenameName(QrcFile_);
  QrcFileDir_ = cmSystemTools::GetFilenamePath(QrcFile_);
  RccFile_ = InfoGet("ARCC_OUTPUT");
  Options_ = InfoGetConfigList("ARCC_OPTIONS");
  Inputs_ = InfoGetList("ARCC_INPUTS");

  // - Validity checks
  if (SettingsFile_.empty()) {
    Log().ErrorFile(cmQtAutoGen::RCC, InfoFile(),
                    "Settings file name missing");
    return false;
  }
  if (AutogenBuildDir_.empty()) {
    Log().ErrorFile(cmQtAutoGen::RCC, InfoFile(),
                    "Autogen build directory missing");
    return false;
  }
  if (RccExecutable_.empty()) {
    Log().ErrorFile(cmQtAutoGen::RCC, InfoFile(), "rcc executable missing");
    return false;
  }
  if (QrcFile_.empty()) {
    Log().ErrorFile(cmQtAutoGen::RCC, InfoFile(), "rcc input file missing");
    return false;
  }
  if (RccFile_.empty()) {
    Log().ErrorFile(cmQtAutoGen::RCC, InfoFile(), "rcc output file missing");
    return false;
  }

  // Init derived information
  // ------------------------

  // Compute rcc output file name
  {
    std::string suffix;
    switch (MultiConfig_) {
      case cmQtAutoGen::SINGLE:
        break;
      case cmQtAutoGen::WRAP:
        suffix = "_CMAKE";
        suffix += ConfigSuffix_;
        suffix += "_";
        break;
      case cmQtAutoGen::FULL:
        suffix = ConfigSuffix_;
        break;
    }
    RccFileBuild_ = cmQtAutoGen::AppendFilenameSuffix(RccFile_, suffix);
  }

  return true;
}

bool cmQtAutoGeneratorRcc::Process()
{
  // Run libuv event loop
  UVRequest().send();
  if (uv_run(UVLoop(), UV_RUN_DEFAULT) == 0) {
    if (Error_) {
      return false;
    }
  } else {
    return false;
  }
  return true;
}

void cmQtAutoGeneratorRcc::UVPollStage(uv_async_t* handle)
{
  reinterpret_cast<cmQtAutoGeneratorRcc*>(handle->data)->PollStage();
}

void cmQtAutoGeneratorRcc::PollStage()
{
  switch (Stage_) {
    case StageT::SETTINGS_READ:
      SettingsFileRead();
      SetStage(StageT::TEST_BASE);
      break;
    case StageT::TEST_BASE:
      if (TestBase()) {
        SetStage(StageT::GENERATE);
      } else {
        SetStage(StageT::TEST_READ_RESOURCES);
      }
      break;
    case StageT::TEST_READ_RESOURCES:
      if (TestReadResources()) {
        SetStage(StageT::TEST_RESOURCES);
      }
      break;
    case StageT::TEST_RESOURCES:
      if (TestResources()) {
        SetStage(StageT::GENERATE);
      } else {
        SetStage(StageT::TEST_INFO_FILE);
      }
      break;
    case StageT::TEST_INFO_FILE:
      TestInfo();
      SetStage(StageT::SETTINGS_WRITE);
      break;
    case StageT::GENERATE:
      GenerateParentDir();
      SetStage(StageT::GENERATE_RCC);
      break;
    case StageT::GENERATE_RCC:
      if (GenerateRcc()) {
        SetStage(StageT::GENERATE_WRAPPER);
      }
      break;
    case StageT::GENERATE_WRAPPER:
      GenerateWrapper();
      SetStage(StageT::SETTINGS_WRITE);
      break;
    case StageT::SETTINGS_WRITE:
      SettingsFileWrite();
      SetStage(StageT::FINISH);
      break;
    case StageT::FINISH:
      // Clear all libuv handles
      UVRequest().reset();
      // Set highest END stage manually
      Stage_ = StageT::END;
      break;
    case StageT::END:
      break;
  }
}

void cmQtAutoGeneratorRcc::SetStage(StageT stage)
{
  if (Error_) {
    stage = StageT::FINISH;
  }
  // Only allow to increase the stage
  if (Stage_ < stage) {
    Stage_ = stage;
    UVRequest().send();
  }
}

void cmQtAutoGeneratorRcc::SettingsFileRead()
{
  // Compose current settings strings
  {
    cmCryptoHash crypt(cmCryptoHash::AlgoSHA256);
    std::string const sep(" ~~~ ");
    {
      std::string str;
      str += RccExecutable_;
      str += sep;
      str += cmJoin(RccListOptions_, ";");
      str += sep;
      str += QrcFile_;
      str += sep;
      str += RccFile_;
      str += sep;
      str += cmJoin(Options_, ";");
      str += sep;
      str += cmJoin(Inputs_, ";");
      str += sep;
      SettingsString_ = crypt.HashString(str);
    }
  }

  // Read old settings
  {
    std::string content;
    if (FileSys().FileRead(content, SettingsFile_)) {
      SettingsChanged_ = (SettingsString_ != SettingsFind(content, "rcc"));
      // In case any setting changed remove the old settings file.
      // This triggers a full rebuild on the next run if the current
      // build is aborted before writing the current settings in the end.
      if (SettingsChanged_) {
        FileSys().FileRemove(SettingsFile_);
      }
    } else {
      SettingsChanged_ = true;
    }
  }
}

void cmQtAutoGeneratorRcc::SettingsFileWrite()
{
  // Only write if any setting changed
  if (SettingsChanged_) {
    if (Log().Verbose()) {
      Log().Info(cmQtAutoGen::RCC, "Writing settings file " +
                   cmQtAutoGen::Quoted(SettingsFile_));
    }
    // Write settings file
    std::string content = "rcc:";
    content += SettingsString_;
    content += '\n';
    if (!FileSys().FileWrite(cmQtAutoGen::RCC, SettingsFile_, content)) {
      Log().ErrorFile(cmQtAutoGen::RCC, SettingsFile_,
                      "Settings file writing failed");
      // Remove old settings file to trigger a full rebuild on the next run
      FileSys().FileRemove(SettingsFile_);
      Error_ = true;
    }
  }
}

bool cmQtAutoGeneratorRcc::TestBase()
{
  // Do basic checks if rcc generation is required

  // Test if the rcc output file exists
  if (!FileSys().FileExists(RccFileBuild_)) {
    if (Log().Verbose()) {
      std::string reason = "Generating ";
      reason += cmQtAutoGen::Quoted(RccFileBuild_);
      reason += " from its source file ";
      reason += cmQtAutoGen::Quoted(QrcFile_);
      reason += " because it doesn't exist";
      Log().Info(cmQtAutoGen::RCC, reason);
    }
    Generate_ = true;
    return Generate_;
  }

  // Test if the settings changed
  if (SettingsChanged_) {
    if (Log().Verbose()) {
      std::string reason = "Generating ";
      reason += cmQtAutoGen::Quoted(RccFileBuild_);
      reason += " from ";
      reason += cmQtAutoGen::Quoted(QrcFile_);
      reason += " because the RCC settings changed";
      Log().Info(cmQtAutoGen::RCC, reason);
    }
    Generate_ = true;
    return Generate_;
  }

  // Test if the rcc output file is older than the .qrc file
  {
    bool isOlder = false;
    {
      std::string error;
      isOlder = FileSys().FileIsOlderThan(RccFileBuild_, QrcFile_, &error);
      if (!error.empty()) {
        Log().ErrorFile(cmQtAutoGen::RCC, QrcFile_, error);
        Error_ = true;
      }
    }
    if (isOlder) {
      if (Log().Verbose()) {
        std::string reason = "Generating ";
        reason += cmQtAutoGen::Quoted(RccFileBuild_);
        reason += " because it is older than ";
        reason += cmQtAutoGen::Quoted(QrcFile_);
        Log().Info(cmQtAutoGen::RCC, reason);
      }
      Generate_ = true;
    }
  }

  return Generate_;
}

bool cmQtAutoGeneratorRcc::TestReadResources()
{
  if (!Inputs_.empty()) {
    // Inputs are known already
    return true;
  }

  if (!RccListOptions_.empty()) {
    // Start a rcc list process and parse the output
    if (Process_) {
      // Process is running already
      if (Process_->IsFinished()) {
        // Process is finished
        if ((ProcessResult_.ExitStatus == 0) && ProcessResult_.Error.empty()) {
          // Process success
          std::string parseError;
          if (!cmQtAutoGen::RccListParseOutput(ProcessResult_.StdOut,
                                               ProcessResult_.StdErr, Inputs_,
                                               parseError)) {
            Log().ErrorFile(cmQtAutoGen::RCC, QrcFile_, parseError);
            Error_ = true;
          }
        } else {
          Log().ErrorFile(cmQtAutoGen::RCC, QrcFile_, ProcessResult_.Error);
          Error_ = true;
        }
        // Clean up
        Process_.reset();
        ProcessResult_.reset();
      } else {
        // Process is not finished, yet.
        return false;
      }
    } else {
      // Start a new process
      // rcc prints relative entry paths when started in the directory of the
      // qrc file with a pathless qrc file name argument.
      // This is important because on Windows absolute paths returned by rcc
      // might contain bad multibyte characters when the qrc file path
      // contains non-ASCII pcharacters.
      std::vector<std::string> cmd;
      cmd.push_back(RccExecutable_);
      cmd.insert(cmd.end(), RccListOptions_.begin(), RccListOptions_.end());
      cmd.push_back(QrcFileName_);
      // We're done here if the process fails to start
      return !StartProcess(QrcFileDir_, cmd, false);
    }
  } else {
    // rcc does not support the --list command.
    // Read the qrc file content and parse it.
    std::string qrcContent;
    if (FileSys().FileRead(cmQtAutoGen::RCC, qrcContent, QrcFile_)) {
      cmQtAutoGen::RccListParseContent(qrcContent, Inputs_);
    }
  }

  if (!Inputs_.empty()) {
    // Convert relative paths to absolute paths
    cmQtAutoGen::RccListConvertFullPath(QrcFileDir_, Inputs_);
  }

  return true;
}

bool cmQtAutoGeneratorRcc::TestResources()
{
  if (Inputs_.empty()) {
    return true;
  }
  {
    std::string error;
    for (std::string const& resFile : Inputs_) {
      // Check if the resource file exists
      if (!FileSys().FileExists(resFile)) {
        error = "Could not find the resource file\n  ";
        error += cmQtAutoGen::Quoted(resFile);
        error += '\n';
        Log().ErrorFile(cmQtAutoGen::RCC, QrcFile_, error);
        Error_ = true;
        break;
      }
      // Check if the resource file is newer than the build file
      if (FileSys().FileIsOlderThan(RccFileBuild_, resFile, &error)) {
        if (Log().Verbose()) {
          std::string reason = "Generating ";
          reason += cmQtAutoGen::Quoted(RccFileBuild_);
          reason += " from ";
          reason += cmQtAutoGen::Quoted(QrcFile_);
          reason += " because it is older than ";
          reason += cmQtAutoGen::Quoted(resFile);
          Log().Info(cmQtAutoGen::RCC, reason);
        }
        Generate_ = true;
        break;
      }
      // Print error and break on demand
      if (!error.empty()) {
        Log().ErrorFile(cmQtAutoGen::RCC, QrcFile_, error);
        Error_ = true;
        break;
      }
    }
  }

  return Generate_;
}

void cmQtAutoGeneratorRcc::TestInfo()
{
  // Test if the rcc output file is older than the info file
  {
    bool isOlder = false;
    {
      std::string error;
      isOlder = FileSys().FileIsOlderThan(RccFile_, InfoFile(), &error);
      if (!error.empty()) {
        Log().ErrorFile(cmQtAutoGen::RCC, QrcFile_, error);
        Error_ = true;
      }
    }
    if (isOlder) {
      if (Log().Verbose()) {
        std::string reason = "Touching ";
        reason += cmQtAutoGen::Quoted(RccFile_);
        reason += " because it is older than ";
        reason += cmQtAutoGen::Quoted(InfoFile());
        Log().Info(cmQtAutoGen::RCC, reason);
      }
      FileSys().Touch(RccFile_);
      if (RccFileBuild_ != RccFile_) {
        FileSys().Touch(RccFileBuild_);
      }
    }
  }
}

void cmQtAutoGeneratorRcc::GenerateParentDir()
{
  // Make sure the parent directory exists
  if (!FileSys().MakeParentDirectory(cmQtAutoGen::RCC, RccFileBuild_)) {
    Error_ = true;
  }
}

/**
 * @return True when finished
 */
bool cmQtAutoGeneratorRcc::GenerateRcc()
{
  if (!Generate_) {
    // Nothing to do
    return true;
  }

  if (Process_) {
    // Process is running already
    if (Process_->IsFinished()) {
      // Process is finished
      if ((ProcessResult_.ExitStatus == 0) && ProcessResult_.Error.empty()) {
        // Process success
        Generated_ = true;
      } else {
        // Process failed
        {
          std::string emsg = "The rcc process failed to compile\n  ";
          emsg += cmQtAutoGen::Quoted(QrcFile_);
          emsg += "\ninto\n  ";
          emsg += cmQtAutoGen::Quoted(RccFileBuild_);
          if (!ProcessResult_.Error.empty()) {
            emsg += "\n";
            emsg += ProcessResult_.Error;
          }
          Log().ErrorCommand(cmQtAutoGen::RCC, emsg, Process_->Setup().Command,
                             ProcessResult_.StdOut);
        }
        FileSys().FileRemove(RccFileBuild_);
        Error_ = true;
      }
      // Clean up
      Process_.reset();
      ProcessResult_.reset();
    } else {
      // Process is not finished, yet.
      return false;
    }
  } else {
    // Start a rcc process
    std::vector<std::string> cmd;
    cmd.push_back(RccExecutable_);
    cmd.insert(cmd.end(), Options_.begin(), Options_.end());
    cmd.push_back("-o");
    cmd.push_back(RccFileBuild_);
    cmd.push_back(QrcFile_);
    // We're done here if the process fails to start
    return !StartProcess(AutogenBuildDir_, cmd, true);
  }

  return true;
}

void cmQtAutoGeneratorRcc::GenerateWrapper()
{
  // Generate a wrapper source file on demand
  if (MultiConfig_ == cmQtAutoGen::WRAP) {
    // Wrapper file name
    std::string const& wrapperFileAbs = RccFile_;
    std::string const wrapperFileRel = cmSystemTools::RelativePath(
      AutogenBuildDir_.c_str(), wrapperFileAbs.c_str());
    // Wrapper file content
    std::string content = "// This is an autogenerated configuration "
                          "wrapper file. Changes will be overwritten.\n"
                          "#include \"";
    content += cmSystemTools::GetFilenameName(RccFileBuild_);
    content += "\"\n";
    // Write content to file
    if (FileSys().FileDiffers(wrapperFileAbs, content)) {
      // Write new wrapper file
      if (Log().Verbose()) {
        Log().Info(cmQtAutoGen::RCC,
                   "Generating RCC wrapper " + wrapperFileRel);
      }
      if (!FileSys().FileWrite(cmQtAutoGen::RCC, wrapperFileAbs, content)) {
        Log().ErrorFile(cmQtAutoGen::RCC, wrapperFileAbs,
                        "RCC wrapper file writing failed");
        Error_ = true;
      }
    } else if (Generated_) {
      // Just touch the wrapper file
      if (Log().Verbose()) {
        Log().Info(cmQtAutoGen::RCC, "Touching RCC wrapper " + wrapperFileRel);
      }
      FileSys().Touch(wrapperFileAbs);
    }
  }
}

bool cmQtAutoGeneratorRcc::StartProcess(
  std::string const& workingDirectory, std::vector<std::string> const& command,
  bool mergedOutput)
{
  // Log command
  if (Log().Verbose()) {
    std::string msg = "Running command:\n";
    msg += cmQtAutoGen::QuotedCommand(command);
    msg += '\n';
    Log().Info(cmQtAutoGen::RCC, msg);
  }

  // Create process handler
  Process_ = cm::make_unique<ReadOnlyProcessT>();
  Process_->setup(&ProcessResult_, mergedOutput, command, workingDirectory);
  // Start process
  if (!Process_->start(UVLoop(),
                       std::bind(&cm::uv_async_ptr::send, &UVRequest()))) {
    Log().ErrorFile(cmQtAutoGen::RCC, QrcFile_, ProcessResult_.Error);
    Error_ = true;
    // Clean up
    Process_.reset();
    ProcessResult_.reset();
    return false;
  }
  return true;
}
