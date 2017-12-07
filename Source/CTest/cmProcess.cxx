/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#include "cmProcess.h"
#include "cmProcessOutput.h"
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <type_traits>

void cmsysProcess_SetTimeout(cmsysProcess* process,
                             std::chrono::duration<double> timeout)
{
  cmsysProcess_SetTimeout(
    process,
    double(
      std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count()) /
      1000.0);
};

#if defined(_WIN32) && !defined(__CYGWIN__)
#include <io.h>

int cmProcess::GetPipes(int* fds)
{
  SECURITY_ATTRIBUTES attr;
  HANDLE readh, writeh;
  attr.nLength = sizeof(attr);
  attr.lpSecurityDescriptor = NULL;
  attr.bInheritHandle = FALSE;
  if (!CreatePipe(&readh, &writeh, &attr, 0))
    return -1;
  fds[0] = _open_osfhandle((intptr_t)readh, 0);
  fds[1] = _open_osfhandle((intptr_t)writeh, 0);
  if (fds[0] == -1 || fds[1] == -1) {
    CloseHandle(readh);
    CloseHandle(writeh);
    return -1;
  }
  return 0;
}

int cmProcess::GetExitException()
{
  auto exception = cmProcess::ExitException::None;
#if defined(_WIN32)
  auto exit_code = (DWORD) this->ExitValue;
  if ((exit_code & 0xF0000000) != 0xC0000000) {
    return exception;
  }
#else
  auto exit_code = this->Signal;
#endif

  if (exit_code) {
    switch (exit_code) {
      case STATUS_DATATYPE_MISALIGNMENT:
      case STATUS_ACCESS_VIOLATION:
      case STATUS_IN_PAGE_ERROR:
      case STATUS_INVALID_HANDLE:
      case STATUS_NONCONTINUABLE_EXCEPTION:
      case STATUS_INVALID_DISPOSITION:
      case STATUS_ARRAY_BOUNDS_EXCEEDED:
      case STATUS_STACK_OVERFLOW:
        exception = cmProcess::ExitException::Fault;
        break;
      case STATUS_FLOAT_DENORMAL_OPERAND:
      case STATUS_FLOAT_DIVIDE_BY_ZERO:
      case STATUS_FLOAT_INEXACT_RESULT:
      case STATUS_FLOAT_INVALID_OPERATION:
      case STATUS_FLOAT_OVERFLOW:
      case STATUS_FLOAT_STACK_CHECK:
      case STATUS_FLOAT_UNDERFLOW:
#ifdef STATUS_FLOAT_MULTIPLE_FAULTS
      case STATUS_FLOAT_MULTIPLE_FAULTS:
#endif
#ifdef STATUS_FLOAT_MULTIPLE_TRAPS
      case STATUS_FLOAT_MULTIPLE_TRAPS:
#endif
      case STATUS_INTEGER_DIVIDE_BY_ZERO:
      case STATUS_INTEGER_OVERFLOW:
        exception = cmProcess::ExitException::Numerical;
        break;
      case STATUS_CONTROL_C_EXIT:
        exception = cmProcess::ExitException::Interrupt;
        break;
      case STATUS_ILLEGAL_INSTRUCTION:
      case STATUS_PRIVILEGED_INSTRUCTION:
        exception = cmProcess::ExitException::Illegal;
        break;
      default:
        exception = cmProcess::ExitException::Other;
    }
  }
  return exception;
}
#else
int cmProcess::GetPipes(int* fds)
{
  if (pipe(fds) == -1) {
    return -1;
  }

  if (fcntl(fds[0], F_SETFD, FD_CLOEXEC) == -1 ||
      fcntl(fds[1], F_SETFD, FD_CLOEXEC) == -1) {
    close(fds[0]);
    close(fds[1]);
    return -1;
  }
  return 0;
}

int cmProcess::GetExitException()
{
  auto exception = cmProcess::ExitException::None;

  if (this->Signal) {
    switch (this->Signal) {
      case SIGSEGV:
        exception = cmProcess::ExitException::Fault;
        break;
      case SIGFPE:
        exception = cmProcess::ExitException::Numerical;
        break;
      case SIGINT:
        exception = cmProcess::ExitException::Interrupt;
        break;
      case SIGILL:
        exception = cmProcess::ExitException::Illegal;
        break;
      default:
        exception = cmProcess::ExitException::Other;
    }
  }
  return exception;
}
#endif

cmProcess::cmProcess()
{
  this->Process = new uv_process_t;
  this->Timer = new uv_timer_t;
  this->PipeReader = new uv_pipe_t;

  this->Timeout = std::chrono::duration<double>::zero();
  this->TotalTime = std::chrono::duration<double>::zero();
  this->ExitValue = 0;
  this->Id = 0;
  this->StartTime = std::chrono::steady_clock::time_point();
}

cmProcess::~cmProcess() = default;

void cmProcess::SetCommand(const char* command)
{
  this->Command = command;
}

void cmProcess::SetCommandArguments(std::vector<std::string> const& args)
{
  this->Arguments = args;
}

bool cmProcess::StartProcess(cmCTestRunTest* runner)
{
  if (this->Command.empty()) {
    return false;
  }

  this->Runner = runner;
  this->StartTime = std::chrono::steady_clock::now();
  this->ProcessArgs.clear();
  // put the command as arg0
  this->ProcessArgs.push_back(this->Command.c_str());
  // now put the command arguments in
  for (std::string const& arg : this->Arguments) {
    this->ProcessArgs.push_back(arg.c_str());
  }
  this->ProcessArgs.push_back(nullptr); // null terminate the list

  int status = OK;
  int fds[2];

  uv_process_options_t options = uv_process_options_t();
  uv_stdio_container_t stdio[3];

  if (cmProcess::GetPipes(fds)) {
    perror("Cannot start process, error creating pipes");
    return false;
  }

  auto pipe_writer = new uv_pipe_t;

  uv_pipe_init(uv_default_loop(), pipe_writer, 0);
  uv_pipe_init(uv_default_loop(), this->PipeReader, 0);

  uv_pipe_open(pipe_writer, fds[1]);
  uv_pipe_open(this->PipeReader, fds[0]);

  options.file = this->Command.data();
  options.args = const_cast<char**>(this->ProcessArgs.data());
  options.stdio_count = 3; // in, out and err
  options.exit_cb = &cmProcess::OnExit;
  options.stdio = stdio;

  // Ignore stdin of the child process
  stdio[0].flags = UV_IGNORE;

  // set the child's stream to pipe_writer then, later, close it with uv_close
  stdio[1].flags = UV_INHERIT_STREAM;
  stdio[1].data.stream = (uv_stream_t*)pipe_writer;

  // copy child's stderr to stdout
  stdio[2] = stdio[1];

  // set this cmProcess on the child's data member for access in evt handler
  this->Process->data = this;

  status = uv_spawn(uv_default_loop(), this->Process, &options);

  if (status != OK) {
    std::cerr << "cmProcess: Never started " << this->Command << " process.\n"
              << "[" << uv_strerror(status) << "]\n";
    this->ProcessState = cmProcess::State::Error;
    return false;
  }

  this->ProcessState = cmProcess::State::Executing;

  uv_close((uv_handle_t*)pipe_writer, NULL);

  status = uv_timer_init(uv_default_loop(), this->Timer);

  if (status == OK) {
    this->Timer->data = this;
    status = this->StartTimer();
    if (status != OK) {
      return false;
    }
  }

  this->PipeReader->data = this;
  status = uv_read_start((uv_stream_t*)this->PipeReader,
                         (uv_alloc_cb)&cmProcess::OnAllocate,
                         (uv_read_cb)&cmProcess::OnRead);

  if (status != OK) {
    std::cerr << "Error starting read events: " << uv_strerror(status)
              << std::endl;
    return false;
  }

  return true;
}

bool cmProcess::Buffer::GetLine(std::string& line)
{
  // Scan for the next newline.
  for (size_type sz = this->size(); this->Last != sz; ++this->Last) {
    if ((*this)[this->Last] == '\n' || (*this)[this->Last] == '\0') {
      // Extract the range first..last as a line.
      const char* text = &*this->begin() + this->First;
      size_type length = this->Last - this->First;
      while (length && text[length - 1] == '\r') {
        length--;
      }
      line.assign(text, length);

      // Start a new range for the next line.
      ++this->Last;
      this->First = Last;

      // Return the line extracted.
      return true;
    }
  }

  // Available data have been exhausted without a newline.
  if (this->First != 0) {
    // Move the partial line to the beginning of the buffer.
    this->erase(this->begin(), this->begin() + this->First);
    this->First = 0;
    this->Last = this->size();
  }
  return false;
}

int cmProcess::StartTimer()
{
  int status = OK;
  auto properties = this->Runner->GetTestProperties();
  auto msec =
    std::chrono::duration_cast<std::chrono::milliseconds>(this->Timeout);

  if (msec != std::chrono::milliseconds(0) || !properties->ExplicitTimeout) {
    status = uv_timer_start(this->Timer, &cmProcess::OnTimeout,
                            (uint64_t)msec.count(), 0);

    if (status != OK) {
      std::cerr << "Unable to start timer: " << uv_strerror(status)
                << std::endl;
    }
  }
  return status;
}

// return the process status
int cmProcess::GetProcessStatus()
{
  if (!this->Process) {
    return cmProcess::State::Exited;
  }
  return this->ProcessState;
}

void cmProcess::ChangeTimeout(std::chrono::duration<double> t)
{
  this->Timeout = t;
  int status = this->StartTimer();
  if (status != OK) {
    std::cerr << "Unable to reset timer: " << uv_strerror(status) << std::endl;
  }
}

void cmProcess::ResetStartTime()
{
  this->StartTime = std::chrono::steady_clock::now();
}

bool cmProcess::IsTimedOut()
{
  return this->ProcessState == cmProcess::State::Expired;
}

void cmProcess::GetExitExceptionString(std::string& exception_str)
{
#if defined(_WIN32)
  switch (this->ExitValue) {
    case STATUS_CONTROL_C_EXIT:
      exception_str = "User interrupt";
      break;
    case STATUS_FLOAT_DENORMAL_OPERAND:
      exception_str = "Floating-point exception (denormal operand)";
      break;
    case STATUS_FLOAT_DIVIDE_BY_ZERO:
      exception_str = "Divide-by-zero";
      break;
    case STATUS_FLOAT_INEXACT_RESULT:
      exception_str = "Floating-point exception (inexact result)";
      break;
    case STATUS_FLOAT_INVALID_OPERATION:
      exception_str = "Invalid floating-point operation";
      break;
    case STATUS_FLOAT_OVERFLOW:
      exception_str = "Floating-point overflow";
      break;
    case STATUS_FLOAT_STACK_CHECK:
      exception_str = "Floating-point stack check failed";
      break;
    case STATUS_FLOAT_UNDERFLOW:
      exception_str = "Floating-point underflow";
      break;
#ifdef STATUS_FLOAT_MULTIPLE_FAULTS
    case STATUS_FLOAT_MULTIPLE_FAULTS:
      exception_str = "Floating-point exception (multiple faults)";
      break;
#endif
#ifdef STATUS_FLOAT_MULTIPLE_TRAPS
    case STATUS_FLOAT_MULTIPLE_TRAPS:
      exception_str = "Floating-point exception (multiple traps)";
      break;
#endif
    case STATUS_INTEGER_DIVIDE_BY_ZERO:
      exception_str = "Integer divide-by-zero";
      break;
    case STATUS_INTEGER_OVERFLOW:
      exception_str = "Integer overflow";
      break;

    case STATUS_DATATYPE_MISALIGNMENT:
      exception_str = "Datatype misalignment";
      break;
    case STATUS_ACCESS_VIOLATION:
      exception_str = "Access violation";
      break;
    case STATUS_IN_PAGE_ERROR:
      exception_str = "In-page error";
      break;
    case STATUS_INVALID_HANDLE:
      exception_str = "Invalid handle";
      break;
    case STATUS_NONCONTINUABLE_EXCEPTION:
      exception_str = "Noncontinuable exception";
      break;
    case STATUS_INVALID_DISPOSITION:
      exception_str = "Invalid disposition";
      break;
    case STATUS_ARRAY_BOUNDS_EXCEEDED:
      exception_str = "Array bounds exceeded";
      break;
    case STATUS_STACK_OVERFLOW:
      exception_str = "Stack overflow";
      break;

    case STATUS_ILLEGAL_INSTRUCTION:
      exception_str = "Illegal instruction";
      break;
    case STATUS_PRIVILEGED_INSTRUCTION:
      exception_str = "Privileged instruction";
      break;
    case STATUS_NO_MEMORY:
    default:
      char buf[1024];
      _snprintf(buf, 1024, "Exit code 0x%x\n", this->ExitValue);
      exception_str.assign(buf);
  }
#else
  switch (this->Signal) {
#ifdef SIGSEGV
    case SIGSEGV:
      exception_str = "Segmentation fault";
      break;
#endif
#ifdef SIGBUS
#if !defined(SIGSEGV) || SIGBUS != SIGSEGV
    case SIGBUS:
      exception_str = "Bus error";
      break;
#endif
#endif
#ifdef SIGFPE
    case SIGFPE:
      exception_str = "Floating-point exception";
      break;
#endif
#ifdef SIGILL
    case SIGILL:
      exception_str = "Illegal instruction";
      break;
#endif
#ifdef SIGINT
    case SIGINT:
      exception_str = "User interrupt";
      break;
#endif
#ifdef SIGABRT
    case SIGABRT:
      exception_str = "Child aborted";
      break;
#endif
#ifdef SIGKILL
    case SIGKILL:
      exception_str = "Child killed";
      break;
#endif
#ifdef SIGTERM
    case SIGTERM:
      exception_str = "Child terminated";
      break;
#endif
#ifdef SIGHUP
    case SIGHUP:
      exception_str = "SIGHUP";
      break;
#endif
#ifdef SIGQUIT
    case SIGQUIT:
      exception_str = "SIGQUIT";
      break;
#endif
#ifdef SIGTRAP
    case SIGTRAP:
      exception_str = "SIGTRAP";
      break;
#endif
#ifdef SIGIOT
#if !defined(SIGABRT) || SIGIOT != SIGABRT
    case SIGIOT:
      exception_str = "SIGIOT";
      break;
#endif
#endif
#ifdef SIGUSR1
    case SIGUSR1:
      exception_str = "SIGUSR1";
      break;
#endif
#ifdef SIGUSR2
    case SIGUSR2:
      exception_str = "SIGUSR2";
      break;
#endif
#ifdef SIGPIPE
    case SIGPIPE:
      exception_str = "SIGPIPE";
      break;
#endif
#ifdef SIGALRM
    case SIGALRM:
      exception_str = "SIGALRM";
      break;
#endif
#ifdef SIGSTKFLT
    case SIGSTKFLT:
      exception_str = "SIGSTKFLT";
      break;
#endif
#ifdef SIGCHLD
    case SIGCHLD:
      exception_str = "SIGCHLD";
      break;
#elif defined(SIGCLD)
    case SIGCLD:
      exception_str = "SIGCLD";
      break;
#endif
#ifdef SIGCONT
    case SIGCONT:
      exception_str = "SIGCONT";
      break;
#endif
#ifdef SIGSTOP
    case SIGSTOP:
      exception_str = "SIGSTOP";
      break;
#endif
#ifdef SIGTSTP
    case SIGTSTP:
      exception_str = "SIGTSTP";
      break;
#endif
#ifdef SIGTTIN
    case SIGTTIN:
      exception_str = "SIGTTIN";
      break;
#endif
#ifdef SIGTTOU
    case SIGTTOU:
      exception_str = "SIGTTOU";
      break;
#endif
#ifdef SIGURG
    case SIGURG:
      exception_str = "SIGURG";
      break;
#endif
#ifdef SIGXCPU
    case SIGXCPU:
      exception_str = "SIGXCPU";
      break;
#endif
#ifdef SIGXFSZ
    case SIGXFSZ:
      exception_str = "SIGXFSZ";
      break;
#endif
#ifdef SIGVTALRM
    case SIGVTALRM:
      exception_str = "SIGVTALRM";
      break;
#endif
#ifdef SIGPROF
    case SIGPROF:
      exception_str = "SIGPROF";
      break;
#endif
#ifdef SIGWINCH
    case SIGWINCH:
      exception_str = "SIGWINCH";
      break;
#endif
#ifdef SIGPOLL
    case SIGPOLL:
      exception_str = "SIGPOLL";
      break;
#endif
#ifdef SIGIO
#if !defined(SIGPOLL) || SIGIO != SIGPOLL
    case SIGIO:
      exception_str = "SIGIO";
      break;
#endif
#endif
#ifdef SIGPWR
    case SIGPWR:
      exception_str = "SIGPWR";
      break;
#endif
#ifdef SIGSYS
    case SIGSYS:
      exception_str = "SIGSYS";
      break;
#endif
#ifdef SIGUNUSED
#if !defined(SIGSYS) || SIGUNUSED != SIGSYS
    case SIGUNUSED:
      exception_str = "SIGUNUSED";
      break;
#endif
#endif
    default:
      char buf[1024];
      sprintf(buf, "Signal %d", this->Signal);
      exception_str.assign(buf);
  }
#endif
}

void cmProcess::OnExit(uv_process_t* process, int64_t exit_status,
                       int term_signal)
{
  auto test = static_cast<cmProcess*>(process->data);

  if (!test->IsTimedOut()) {
    cmProcess::State state = cmProcess::State::Exited;
#if defined(_WIN32)
    if (((DWORD)exit_status & 0xF0000000) == 0xC0000000) {
      state = cmProcess::State::Exception;
    }
#else
    if (term_signal != 0) {
      state = cmProcess::State::Exception;
    }
#endif
    test->SetState(state);
  }

  test->TotalTime = std::chrono::steady_clock::now() - test->StartTime;

  if (test->TotalTime <= std::chrono::duration<double>::zero()) {
    test->TotalTime = std::chrono::duration<double>::zero();
  }
  // Because of a processor clock scew the runtime may become slightly
  // negative. If someone changed the system clock while the process was
  // running this may be even more. Make sure not to report a negative
  // duration here.
  test->SetExitStatus((int)exit_status);
  test->SetSignal(term_signal);

  uv_close((uv_handle_t*)process, NULL);

  if (test->ReadHandleClosed) {
    uv_close((uv_handle_t*)test->Timer, NULL);
    test->Runner->FinalizeTest();
  }

  test->ProcessHandleClosed = true;
}

void cmProcess::OnTimeout(uv_timer_t* timer)
{
  auto test = static_cast<cmProcess*>(timer->data);
  test->TotalTime = std::chrono::steady_clock::now() - test->StartTime;
  cmsysProcess_KillPID((unsigned long)test->Process->pid);
  uv_close((uv_handle_t*)test->Process, NULL);
  uv_close((uv_handle_t*)test->PipeReader, NULL);
  uv_close((uv_handle_t*)timer, NULL);
  test->SetState(cmProcess::State::Expired);
  test->Runner->FinalizeTest();
}

void cmProcess::OnRead(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf)
{
  auto test = static_cast<cmProcess*>(stream->data);

  if (nread > 0) {
    std::string strdata, line;

    cmProcessOutput processOutput(cmProcessOutput::UTF8);
    processOutput.DecodeText(buf->base, nread, strdata);
    test->Output.insert(test->Output.end(), strdata.begin(), strdata.end());

    while (test->Output.GetLine(line)) {
      test->Runner->PrintLine(&line);
      line.clear();
    }

  } else {
    if (nread != UV_EOF) {
      std::cerr << "Error reading stream: " << uv_strerror(nread) << std::endl;
    }
    uv_close((uv_handle_t*)stream, NULL);
    if (test->ProcessHandleClosed) {
      uv_timer_stop(test->Timer);
      test->Runner->FinalizeTest();
    }
    test->ReadHandleClosed = true;
    if (buf->base) {
      free(buf->base);
    }
  }
}

void cmProcess::OnAllocate(uv_handle_t* handle, size_t suggested_size,
                           uv_buf_t* buf)
{
  auto test = static_cast<cmProcess*>(handle->data);
  if (test->Buf.size() < suggested_size) {
    test->Buf.resize(suggested_size);
  }

  *buf = uv_buf_init(test->Buf.data(), (unsigned int)test->Buf.size());
}
