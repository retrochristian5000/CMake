#include "cmUVProcessChain.h"

#include "cmGetPipes.h"
#include "cmUVHandlePtr.h"
#include "cmUVStreambuf.h"

#include "cm_uv.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <csignal>

struct ExpectedResult
{
  bool MatchFinished;
  bool MatchExitStatus;
  bool MatchTermSignal;
  cmUVProcessChain::Result Result;
};

static const std::vector<ExpectedResult> results1 = {
  { true, false, false, { false, 0, 0 } },
  { true, false, false, { false, 0, 0 } },
  { true, false, false, { false, 0, 0 } },
};

static const std::vector<ExpectedResult> results2 = {
  { true, true, true, { true, 0, 0 } },
  { true, false, false, { false, 0, 0 } },
  { true, false, false, { false, 0, 0 } },
};

static const std::vector<ExpectedResult> results3 = {
  { true, true, true, { true, 0, 0 } },
  { true, true, true, { true, 1, 0 } },
#ifdef _WIN32
  { true, true, true, { true, 2, 0 } },
#else
  { true, false, true, { true, 0, SIGABRT } },
#endif
};

bool operator==(const cmUVProcessChain::Result& actual,
                const ExpectedResult& expected)
{
  if (expected.MatchFinished && expected.Result.Finished != actual.Finished) {
    return false;
  }
  if (expected.MatchExitStatus &&
      expected.Result.ExitStatus != actual.ExitStatus) {
    return false;
  }
  if (expected.MatchTermSignal &&
      expected.Result.TermSignal != actual.TermSignal) {
    return false;
  }
  return true;
}

bool resultsMatch(const std::vector<cmUVProcessChain::Result>& actual,
                  const std::vector<ExpectedResult>& expected)
{
  return actual.size() == expected.size() &&
    std::equal(actual.begin(), actual.end(), expected.begin());
}

std::string getInput(std::istream& input)
{
  char buffer[1024];
  std::ostringstream str;
  do {
    input.read(buffer, 1024);
    str.write(buffer, input.gcount());
  } while (input.gcount() > 0);
  return str.str();
}

template <typename T>
std::function<std::ostream&(std::ostream&)> printExpected(bool match,
                                                          const T& value)
{
  return [match, value](std::ostream& stream) -> std::ostream& {
    if (match) {
      stream << value;
    } else {
      stream << "*";
    }
    return stream;
  };
}

std::ostream& operator<<(
  std::ostream& stream,
  const std::function<std::ostream&(std::ostream&)>& func)
{
  return func(stream);
}

void printResults(const std::vector<cmUVProcessChain::Result>& actual,
                  const std::vector<ExpectedResult>& expected)
{
  std::cout << "Expected: " << std::endl;
  for (auto const& e : expected) {
    std::cout << "  Finished: "
              << printExpected(e.MatchFinished, e.Result.Finished)
              << ", ExitStatus: "
              << printExpected(e.MatchExitStatus, e.Result.ExitStatus)
              << ", TermSignal: "
              << printExpected(e.MatchTermSignal, e.Result.TermSignal)
              << std::endl;
  }
  std::cout << "Actual:" << std::endl;
  for (auto const& a : actual) {
    std::cout << "  Finished: " << a.Finished
              << ", ExitStatus: " << a.ExitStatus
              << ", TermSignal: " << a.TermSignal << std::endl;
  }
}

bool checkExecution(cmUVProcessChain& chain)
{
  std::vector<cmUVProcessChain::Result> results;

  if (chain.Wait()) {
    std::cout << "Wait() returned true, should be false" << std::endl;
    return false;
  }
  if (chain.GetResults(&results)) {
    std::cout << "GetResults() returned true, should be false" << std::endl;
    return false;
  }

  if (!chain.Start()) {
    std::cout << "Start() returned false, should be true" << std::endl;
    return false;
  }
  if (!chain.GetResults(&results)) {
    std::cout << "GetResults() returned false, should be true" << std::endl;
    return false;
  }
  if (!resultsMatch(results, results1)) {
    std::cout << "GetResults() did not produce expected output" << std::endl;
    printResults(results, results1);
    return false;
  }

  if (chain.Wait(6000)) {
    std::cout << "Wait() returned true, should be false" << std::endl;
    return false;
  }
  if (!chain.GetResults(&results)) {
    std::cout << "GetResults() returned false, should be true" << std::endl;
    return false;
  }
  if (!resultsMatch(results, results2)) {
    std::cout << "GetResults() did not produce expected output" << std::endl;
    printResults(results, results2);
    return false;
  }

  if (!chain.Wait()) {
    std::cout << "Wait() returned false, should be true" << std::endl;
    return false;
  }
  if (!chain.GetResults(&results)) {
    std::cout << "GetResults() returned false, should be true" << std::endl;
    return false;
  }
  if (!resultsMatch(results, results3)) {
    std::cout << "GetResults() did not produce expected output" << std::endl;
    printResults(results, results3);
    return false;
  }

  return true;
}

bool checkOutput(std::istream& outputStream, std::istream& errorStream)
{
  std::string output = getInput(outputStream);
  if (output != "HELO WRD!") {
    std::cout << "Output was \"" << output << "\", expected \"HELO WRD!\""
              << std::endl;
    return false;
  }

  std::string error = getInput(errorStream);
  if (error.length() != 3 || error.find('1') == std::string::npos ||
      error.find('2') == std::string::npos ||
      error.find('3') == std::string::npos) {
    std::cout << "Error was \"" << error << "\", expected \"123\""
              << std::endl;
    return false;
  }

  return true;
}

bool testUVProcessChainBuiltin(const char* helperCommand)
{
  cmUVProcessChain chain;
  chain.AddCommand({ helperCommand, "echo" });
  chain.AddCommand({ helperCommand, "capitalize" });
  chain.AddCommand({ helperCommand, "dedup" });
  chain.SetBuiltinStream(cmUVProcessChain::Stream_OUTPUT);
  chain.SetBuiltinStream(cmUVProcessChain::Stream_ERROR);

  if (!chain.OutputStream()) {
    std::cout << "OutputStream() was null, expecting not null" << std::endl;
    return false;
  }
  if (!chain.ErrorStream()) {
    std::cout << "ErrorStream() was null, expecting not null" << std::endl;
    return false;
  }

  if (!checkExecution(chain)) {
    return false;
  }

  if (!checkOutput(*chain.OutputStream(), *chain.ErrorStream())) {
    return false;
  }

  return true;
}

bool testUVProcessChainExternal(const char* helperCommand)
{
  std::vector<cmUVProcessChain::Result> results;
  cmUVProcessChain chain;
  cm::uv_pipe_ptr outputOutPipe, outputInPipe, errorOutPipe, errorInPipe;
  outputOutPipe.init(chain.GetLoop(), 0);
  outputInPipe.init(chain.GetLoop(), 0);
  errorOutPipe.init(chain.GetLoop(), 0);
  errorInPipe.init(chain.GetLoop(), 0);

  int pipeFds[2];
  if (cmGetPipes(pipeFds) < 0) {
    std::cout << "Error creating pipes" << std::endl;
    return false;
  }
  uv_pipe_open(outputInPipe, pipeFds[0]);
  uv_pipe_open(outputOutPipe, pipeFds[1]);
  if (cmGetPipes(pipeFds) < 0) {
    std::cout << "Error creating pipes" << std::endl;
    return false;
  }
  uv_pipe_open(errorInPipe, pipeFds[0]);
  uv_pipe_open(errorOutPipe, pipeFds[1]);

  chain.AddCommand({ helperCommand, "echo" });
  chain.AddCommand({ helperCommand, "capitalize" });
  chain.AddCommand({ helperCommand, "dedup" });
  chain.SetExternalStream(cmUVProcessChain::Stream_OUTPUT, outputOutPipe);
  chain.SetExternalStream(cmUVProcessChain::Stream_ERROR, errorOutPipe);

  if (chain.OutputStream()) {
    std::cout << "OutputStream() was not null, expecting null" << std::endl;
    return false;
  }
  if (chain.ErrorStream()) {
    std::cout << "ErrorStream() was not null, expecting null" << std::endl;
    return false;
  }

  if (!checkExecution(chain)) {
    return false;
  }

  outputOutPipe.reset();
  errorOutPipe.reset();

  cmUVStreambuf outputBuf;
  outputBuf.open(outputInPipe);
  std::istream outputStream(&outputBuf);

  cmUVStreambuf errorBuf;
  errorBuf.open(errorInPipe);
  std::istream errorStream(&errorBuf);

  if (!checkOutput(outputStream, errorStream)) {
    return false;
  }

  return true;
}

bool testUVProcessChainNone(const char* helperCommand)
{
  cmUVProcessChain chain;
  chain.AddCommand({ helperCommand, "echo" });
  chain.AddCommand({ helperCommand, "capitalize" });
  chain.AddCommand({ helperCommand, "dedup" });

  if (chain.OutputStream()) {
    std::cout << "OutputStream() was not null, expecting null" << std::endl;
    return false;
  }
  if (chain.ErrorStream()) {
    std::cout << "ErrorStream() was not null, expecting null" << std::endl;
    return false;
  }

  if (!checkExecution(chain)) {
    return false;
  }

  return true;
}

int testUVProcessChain(int argc, char** const argv)
{
  if (argc < 2) {
    std::cout << "Invalid arguments.\n";
    return -1;
  }

  if (!testUVProcessChainBuiltin(argv[1])) {
    std::cout << "While executing testUVProcessChainBuiltin().\n";
    return -1;
  }

  if (!testUVProcessChainExternal(argv[1])) {
    std::cout << "While executing testUVProcessChainExternal().\n";
    return -1;
  }

  if (!testUVProcessChainNone(argv[1])) {
    std::cout << "While executing testUVProcessChainNone().\n";
    return -1;
  }

  return 0;
}
