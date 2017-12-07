/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#ifndef cmProcess_h
#define cmProcess_h

#include "cmCTestRunTest.h"
#include "cmConfigure.h" // IWYU pragma: keep
#include "cmUVHandlePtr.h"
#include "cm_uv.h"
#include "cmsys/Process.h"
#include <chrono>
#include <stddef.h> // IWYU
#include <stdint.h> // IWYU
#include <string>
#include <vector>

class cmCTestRunTest;

/*
 * A wrapper function for cmsysProcess_SetTimeout that takes an
 * std::chrono::duration. For convenience only.
 */
void cmsysProcess_SetTimeout(cmsysProcess* process,
                             std::chrono::duration<double> timeout);

/** \class cmProcess
 * \brief run a process with c++
 *
 * cmProcess wraps the kwsys process stuff in a c++ class.
 */
class cmProcess
{
public:
  cmProcess();
  ~cmProcess();
  const char* GetCommand() { return this->Command.c_str(); }
  void SetCommand(const char* command);
  void SetCommandArguments(std::vector<std::string> const& arg);
  void SetWorkingDirectory(const char* dir) { this->WorkingDirectory = dir; }
  void SetTimeout(std::chrono::duration<double> t) { this->Timeout = t; }
  void ChangeTimeout(std::chrono::duration<double> t);
  void ResetStartTime();
  // Return true if the process starts
  bool StartProcess(cmCTestRunTest* runner);

  // return the process status
  int GetProcessStatus();
  int GetId() { return this->Id; }
  void SetId(int id) { this->Id = id; }
  int GetExitValue() { return this->ExitValue; }
  std::chrono::duration<double> GetTotalTime() { return this->TotalTime; }
  int GetExitException();
  void GetExitExceptionString(std::string& exception_str);
  static int GetPipes(int* fds);
  bool IsTimedOut();

  enum State
  {
    Starting,
    Error,
    Exception,
    Executing,
    Exited,
    Expired,
    Killed,
    Disowned
  };

  enum ExitException
  {
    None,
    Fault,
    Illegal,
    Interrupt,
    Numerical,
    Other
  };

  const int OK = 0;
  const int MILLISECONDS = 1000;

protected:
  static void OnExit(uv_process_t* process, int64_t exit_status,
                     int term_signal);
  static void OnTimeout(uv_timer_t* timer);
  static void OnRead(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf);
  static void OnAllocate(uv_handle_t* handle, size_t suggested_size,
                         uv_buf_t* buf);
  int StartTimer();
  void SetExitStatus(int exit_value) { this->ExitValue = exit_value; }
  void SetSignal(int signal) { this->Signal = signal; }
  void SetState(cmProcess::State state) { this->ProcessState = state; }
private:
  std::chrono::duration<double> Timeout;
  std::chrono::steady_clock::time_point StartTime;
  std::chrono::duration<double> TotalTime;
  std::string Command;
  std::string WorkingDirectory;
  std::vector<std::string> Arguments;
  std::vector<const char*> ProcessArgs;
  int Id;
  int ExitValue;
  bool ReadHandleClosed = false;
  bool ProcessHandleClosed = false;

  // libuv vars
  cm::uv_process_ptr Process;
  cm::uv_pipe_ptr PipeReader;
  cm::uv_timer_ptr Timer;

  cmCTestRunTest* Runner = nullptr;
  int Signal;
  cmProcess::State ProcessState = cmProcess::State::Starting;

  class Buffer : public std::vector<char>
  {
    // Half-open index range of partial line already scanned.
    size_type First;
    size_type Last;

  public:
    Buffer()
      : First(0)
      , Last(0)
    {
    }
    bool GetLine(std::string& line);
  };
  Buffer Output;
  std::vector<char> Buf;
};

#endif
