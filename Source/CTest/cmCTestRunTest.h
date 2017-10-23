/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#ifndef cmCTestRunTest_h
#define cmCTestRunTest_h

#include "cmConfigure.h" // IWYU pragma: keep

#include "cmCTestMultiProcessHandler.h"
#include "cmCTestTestHandler.h"
#include <chrono>
#include <cm_uv.h>
#include <set>
#include <stddef.h>
#include <string>
#include <vector>

class cmCTest;
class cmProcess;

/** \class cmRunTest
 * \brief represents a single test to be run
 *
 * cmRunTest contains the information related to running a single test
 */
class cmCTestRunTest
{
public:
  explicit cmCTestRunTest(cmCTestTestHandler* handler);
  ~cmCTestRunTest() = default;

  const char NEWLINE = '\n';

  void SetNumberOfRuns(int n) { this->NumberOfRunsLeft = n; }
  void SetRunUntilFailOn() { this->RunUntilFail = true; }
  void SetTestProperties(cmCTestTestHandler::cmCTestTestProperties* prop)
  {
    this->TestProperties = prop;
  }

  cmCTestTestHandler::cmCTestTestProperties* GetTestProperties()
  {
    return this->TestProperties;
  }

  void SetIndex(int i) { this->Index = i; }

  int GetIndex() { return this->Index; }

  void AddFailedDependency(const std::string& failedTest)
  {
    this->FailedDependencies.insert(failedTest);
  }

  std::string GetProcessOutput() { return this->ProcessOutput; }

  bool IsStopTimePassed() { return this->StopTimePassed; }

  cmCTestTestHandler::cmCTestTestResult GetTestResults()
  {
    return this->TestResult;
  }

  // Read and store output.  Returns true if it must be called again.
  void CheckOutput(std::string* line);

  // Compresses the output, writing to CompressedOutput
  void CompressOutput();
  // Prints output from each process line by line
  void PrintLine(std::string* output);
  // launch the test process, return whether it started correctly
  bool StartTest(size_t total, cmCTestMultiProcessHandler* multi_test_handler);
  // capture and report the test results
  bool EndTest(size_t completed, size_t total, bool started);
  // Called by ctest -N to log the command string
  void ComputeArguments();

  void ComputeWeightedCost();

  bool StartAgain();

  void FinalizeTest();

private:
  bool NeedsToRerun();
  void DartProcessing();

  // Figures out a final timeout which is min(STOP_TIME, NOW+TIMEOUT)
  std::chrono::duration<double> ResolveTimeout();
  bool ForkProcess(std::chrono::duration<double> testTimeOut,
                   bool explicitTimeout,
                   std::vector<std::string>* environment);
  void WriteLogOutputTop(size_t completed, size_t total);
  // Run post processing of the process output for MemCheck
  void MemCheckPostProcess();

  cmCTestTestHandler::cmCTestTestProperties* TestProperties;
  // Pointer back to the "parent"; the handler that invoked this test run
  cmCTestTestHandler* TestHandler;
  cmCTest* CTest;
  cmProcess* TestProcess;

  std::string ProcessOutput;
  std::string CompressedOutput;
  double CompressionRatio;
  // The test results
  cmCTestTestHandler::cmCTestTestResult TestResult;
  cmCTestMultiProcessHandler* MultiTestHandler;
  int Index;
  std::set<std::string> FailedDependencies;
  std::string StartTime;
  std::string ActualCommand;
  std::vector<std::string> Arguments;
  bool StopTimePassed;
  bool RunUntilFail;
  int NumberOfRunsLeft;
  bool RunAgain;
  size_t TotalNumberOfTests;
};

inline int getNumWidth(size_t n)
{
  int numWidth = 1;
  if (n >= 10) {
    numWidth = 2;
  }
  if (n >= 100) {
    numWidth = 3;
  }
  return numWidth;
}

#endif
