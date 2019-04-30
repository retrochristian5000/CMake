/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#include "cmUVProcessChain.h"

#include "cmGetPipes.h"
#include "cm_uv.h"

#include <iostream>
#include <iterator>

cmUVProcessChain::cmUVProcessChain()
{
  this->Loop.init();

  this->SetNoStream(Stream_OUTPUT);
  this->SetNoStream(Stream_ERROR);

  this->OutputStreamData.ReadWriteFlag = UV_WRITABLE_PIPE;
  this->ErrorStreamData.ReadWriteFlag = UV_WRITABLE_PIPE;
}

void cmUVProcessChain::AddCommand(const std::vector<std::string>& arguments)
{
  if (!this->Started && !arguments.empty()) {
    this->Processes.emplace_back();
    auto& process = this->Processes.back();
    process.Chain = this;
    process.Arguments = arguments;
    process.ProcessResult.Finished = false;
    process.ProcessResult.ExitStatus = 0;
    process.ProcessResult.TermSignal = 0;
  }
}

void cmUVProcessChain::SetNoStream(Stream stdio)
{
  auto streamData = this->GetStreamData(stdio);
  if (streamData) {
    streamData->BuiltinStream.reset();
    streamData->Flags = UV_IGNORE;
    streamData->Stream = nullptr;
  }
}

void cmUVProcessChain::SetBuiltinStream(Stream stdio)
{
  auto streamData = this->GetStreamData(stdio);
  if (streamData) {
    streamData->BuiltinStream.init(*this->Loop, 0);
    streamData->Flags =
      static_cast<uv_stdio_flags>(UV_CREATE_PIPE | streamData->ReadWriteFlag);
    streamData->Stream = streamData->BuiltinStream;
  }
}

void cmUVProcessChain::SetExternalStream(Stream stdio, uv_stream_t* stream)
{
  auto streamData = this->GetStreamData(stdio);
  if (streamData) {
    streamData->BuiltinStream.reset();
    streamData->Flags = static_cast<uv_stdio_flags>(UV_INHERIT_STREAM |
                                                    streamData->ReadWriteFlag);
    streamData->Stream = stream;
  }
}

std::istream* cmUVProcessChain::OutputStream()
{
  auto streamData =
    this->GetStreamData<StreamData<std::istream>>(Stream_OUTPUT);
  if ((streamData->Flags & UV_CREATE_PIPE) == UV_CREATE_PIPE) {
    return &streamData->BuiltinIOStream;
  }
  return nullptr;
}

std::istream* cmUVProcessChain::ErrorStream()
{
  auto streamData =
    this->GetStreamData<StreamData<std::istream>>(Stream_ERROR);
  if ((streamData->Flags & UV_CREATE_PIPE) == UV_CREATE_PIPE) {
    return &streamData->BuiltinIOStream;
  }
  return nullptr;
}

bool cmUVProcessChain::Start()
{
  if (this->Started) {
    return true;
  }
  this->Started = true;

  cm::uv_pipe_ptr errorOutPipe;
  uv_stdio_flags errorFlags = this->ErrorStreamData.Flags;
  uv_stream_t* errorStream = this->ErrorStreamData.Stream;
  if ((this->ErrorStreamData.Flags & UV_CREATE_PIPE) == UV_CREATE_PIPE) {
    int pipeFd[2];
    if (cmGetPipes(pipeFd) < 0) {
      return false;
    }
    errorOutPipe.init(*this->Loop, 0);
    if (uv_pipe_open(this->ErrorStreamData.BuiltinStream, pipeFd[0]) < 0) {
      return false;
    }
    if (uv_pipe_open(errorOutPipe, pipeFd[1]) < 0) {
      return false;
    }
    this->ErrorStreamData.Streambuf.open(this->ErrorStreamData.BuiltinStream);
    errorFlags =
      static_cast<uv_stdio_flags>(UV_INHERIT_STREAM | UV_WRITABLE_PIPE);
    errorStream = errorOutPipe;
  }

  for (auto it = this->Processes.begin(); it != this->Processes.end(); ++it) {
    auto& process = *it;

    if (it == this->Processes.begin()) {
      process.Stdio[0].flags = UV_IGNORE;
      process.Stdio[0].data.stream = nullptr;
    } else {
      auto& prevProcess = *std::prev(it);
      process.Stdio[0].flags =
        static_cast<uv_stdio_flags>(UV_INHERIT_STREAM | UV_READABLE_PIPE);
      process.Stdio[0].data.stream = prevProcess.OutputPipe;
    }
    if (it == std::prev(this->Processes.end())) {
      process.Stdio[1].flags = this->OutputStreamData.Flags;
      process.Stdio[1].data.stream = this->OutputStreamData.Stream;
    } else {
      process.OutputPipe.init(*this->Loop, 0);
      process.Stdio[1].flags =
        static_cast<uv_stdio_flags>(UV_CREATE_PIPE | UV_WRITABLE_PIPE);
      process.Stdio[1].data.stream = process.OutputPipe;
    }
    process.Stdio[2].flags = errorFlags;
    process.Stdio[2].data.stream = errorStream;

    auto& options = process.Options;
    options = uv_process_options_t();

    // Bounds were checked at add time, first element is guaranteed to exist
    options.file = process.Arguments[0].c_str();

    for (auto const& arg : process.Arguments) {
      process.CharArguments.push_back(arg.c_str());
    }
    process.CharArguments.push_back(nullptr);
    options.args = const_cast<char**>(process.CharArguments.data());
    options.flags = UV_PROCESS_WINDOWS_HIDE;
    options.stdio = process.Stdio.data();
    options.stdio_count = 3;
    options.exit_cb = [](uv_process_t* handle, int64_t exitStatus,
                         int termSignal) {
      auto* processData = static_cast<ProcessData*>(handle->data);
      processData->ProcessResult.Finished = true;
      processData->ProcessResult.ExitStatus = exitStatus;
      processData->ProcessResult.TermSignal = termSignal;
      processData->Chain->ProcessesCompleted++;
    };

    if (process.Process.spawn(*this->Loop, options, &process) < 0) {
      return false;
    }
  }

  if ((this->OutputStreamData.Flags & UV_CREATE_PIPE) == UV_CREATE_PIPE) {
    this->OutputStreamData.Streambuf.open(
      this->OutputStreamData.BuiltinStream);
  }

  return true;
}

bool cmUVProcessChain::Wait(int64_t milliseconds)
{
  if (!this->Started) {
    return false;
  }

  bool timeout = false;
  cm::uv_timer_ptr timer;

  if (milliseconds >= 0) {
    timer.init(*this->Loop, &timeout);
    timer.start(
      [](uv_timer_t* handle) {
        auto* timeoutPtr = static_cast<bool*>(handle->data);
        *timeoutPtr = true;
      },
      milliseconds, 0);
  }

  while (!timeout && this->ProcessesCompleted < this->Processes.size()) {
    uv_run(this->Loop, UV_RUN_ONCE);
  }

  return !timeout;
}

bool cmUVProcessChain::GetResults(std::vector<Result>* results) const
{
  if (!this->Started) {
    return false;
  }

  if (results) {
    results->clear();
    results->reserve(this->Processes.size());
    for (auto const& process : this->Processes) {
      results->push_back(process.ProcessResult);
    }
  }

  return true;
}

cmUVProcessChain::BasicStreamData* cmUVProcessChain::GetStreamData(
  Stream stdio)
{
  switch (stdio) {
    case Stream_OUTPUT:
      return &this->OutputStreamData;

    case Stream_ERROR:
      return &this->ErrorStreamData;

    default:
      return nullptr;
  }
}

bool operator==(const cmUVProcessChain::Result& r1,
                const cmUVProcessChain::Result& r2)
{
  return r1.Finished == r2.Finished && r1.ExitStatus == r2.ExitStatus &&
    r1.TermSignal == r2.TermSignal;
}
