/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#ifndef cmUVProcessChain_h
#define cmUVProcessChain_h

#include "cmUVHandlePtr.h"
#include "cmUVStreambuf.h"
#include "cm_uv.h"

#include <array>
#include <iostream>
#include <string>
#include <vector>

#include <stdint.h>

class cmUVProcessChain
{
public:
  enum Stream
  {
    Stream_OUTPUT,
    Stream_ERROR,
  };

  struct Result
  {
    bool Finished;
    int64_t ExitStatus;
    int TermSignal;
  };

  cmUVProcessChain();

  uv_loop_t& GetLoop() { return *this->Loop; }

  void AddCommand(const std::vector<std::string>& arguments);

  void SetNoStream(Stream stdio);
  void SetBuiltinStream(Stream stdio);
  void SetExternalStream(Stream stdio, uv_stream_t* stream);

  // FIXME: Add stdin support
  std::istream* OutputStream();
  std::istream* ErrorStream();

  bool Start();
  bool Wait(int64_t milliseconds = -1);
  bool GetResults(std::vector<Result>* results) const;

private:
  struct BasicStreamData
  {
    cmUVStreambuf Streambuf;
    cm::uv_pipe_ptr BuiltinStream;
    uv_stream_t* Stream;
    uv_stdio_flags Flags;
    uv_stdio_flags ReadWriteFlag;
  };

  template <typename IOStream>
  struct StreamData : public BasicStreamData
  {
    StreamData()
      : BuiltinIOStream(&this->Streambuf)
    {
    }

    IOStream BuiltinIOStream;
  };

  struct ProcessData
  {
    cmUVProcessChain* Chain;
    std::vector<std::string> Arguments;
    std::vector<const char*> CharArguments;
    uv_process_options_t Options;
    std::array<uv_stdio_container_t, 3> Stdio;
    cm::uv_process_ptr Process;
    cm::uv_pipe_ptr OutputPipe;
    Result ProcessResult;
  };

  cm::uv_loop_ptr Loop;

  StreamData<std::istream> OutputStreamData;
  StreamData<std::istream> ErrorStreamData;

  bool Started = false;
  unsigned int ProcessesCompleted = 0;
  std::vector<ProcessData> Processes;

  BasicStreamData* GetStreamData(Stream stdio);
  template <typename SD>
  SD* GetStreamData(Stream stdio)
  {
    return static_cast<SD*>(this->GetStreamData(stdio));
  }
};

bool operator==(const cmUVProcessChain::Result& r1,
                const cmUVProcessChain::Result& r2);

#endif
