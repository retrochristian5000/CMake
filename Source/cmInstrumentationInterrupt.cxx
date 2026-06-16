/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file LICENSE.rst or https://cmake.org/licensing for details.  */
#include "cmInstrumentationInterrupt.h"

#include <csignal>
#ifdef _WIN32
#  include <atomic>

#  include <windows.h>
#else
#  include <cstring>
#endif

namespace {
// Flag shared between the interrupt handler and the build flow that writes the
// `cmakeBuild` snippet.  On Windows the console control handler runs on a
// separate thread, so an atomic is required; on POSIX the handler runs in
// signal context, where only `volatile sig_atomic_t` is guaranteed safe.
#ifdef _WIN32
std::atomic<int> buildInterruptSignal{ 0 };

BOOL WINAPI cmInstrumentationConsoleHandler(DWORD type)
{
  if (type == CTRL_C_EVENT || type == CTRL_BREAK_EVENT) {
    int expected = 0;
    buildInterruptSignal.compare_exchange_strong(expected, SIGINT);
    // Return TRUE so the main thread can finish writing the snippet before the
    // process exits.  The native build tool shares the console and receives
    // the event directly, so it still terminates and unblocks our build loop.
    return TRUE;
  }
  return FALSE;
}
#else
sig_atomic_t volatile buildInterruptSignal = 0;
struct sigaction savedSigIntAction;

extern "C" void cmInstrumentationSignalHandler(int sig)
{
  buildInterruptSignal = sig;
}
#endif

// Install the interrupt handler and clear any previously recorded signal.
void InstallInterruptHandler()
{
#ifdef _WIN32
  buildInterruptSignal.store(0);
  SetConsoleCtrlHandler(cmInstrumentationConsoleHandler, TRUE);
#else
  buildInterruptSignal = 0;
  struct sigaction sa;
  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = cmInstrumentationSignalHandler;
  sigemptyset(&sa.sa_mask);
  // One-shot: after the first interrupt the default disposition is restored,
  // so a second Ctrl+C terminates immediately even while we are mid-flush.
  sa.sa_flags = SA_RESETHAND;
  sigaction(SIGINT, &sa, &savedSigIntAction);
#endif
}

// Restore the disposition that was in effect before InstallInterruptHandler().
void RestoreInterruptHandler()
{
#ifdef _WIN32
  SetConsoleCtrlHandler(cmInstrumentationConsoleHandler, FALSE);
#else
  sigaction(SIGINT, &savedSigIntAction, nullptr);
#endif
}
}

int cmInstrumentationInterrupt::PendingInterruptSignal()
{
#ifdef _WIN32
  return buildInterruptSignal.load();
#else
  return static_cast<int>(buildInterruptSignal);
#endif
}

cmInstrumentationInterrupt::InterruptOutcome
cmInstrumentationInterrupt::HandleInterrupt(
  bool active, std::function<int()> const& callback)
{
  // Only trap interrupts when instrumentation is active, so non-instrumented
  // flows keep their default signal behavior.
  if (!active) {
    return { callback(), false, 0 };
  }
  InstallInterruptHandler();
  int ret = callback();
  int sig = PendingInterruptSignal();
  RestoreInterruptHandler();
  return { ret, sig != 0, sig };
}

void cmInstrumentationInterrupt::RaiseInterrupt(int sig)
{
#ifdef _WIN32
  // On Windows the process exits normally after flushing; the caller
  // propagates the (failed) build result.
  static_cast<void>(sig);
#else
  // Restore the default disposition (SA_RESETHAND already did so for the first
  // delivery) and re-raise so the exit status reflects the interrupt.
  signal(sig, SIG_DFL);
  raise(sig);
#endif
}
