/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#ifndef cmCursesForm_h
#define cmCursesForm_h

#include "cmConfigure.h" // IWYU pragma: keep

#include <mutex>
#include <string>

#include "cmsys/FStream.hxx"

#include "cmCursesStandardIncludes.h"

class cmCursesForm
{
public:
  cmCursesForm();
  virtual ~cmCursesForm();

  cmCursesForm(cmCursesForm const&) = delete;
  cmCursesForm& operator=(cmCursesForm const&) = delete;

  // Description:
  // Handle user input.
  virtual void HandleInput() = 0;

  // Description:
  // Display form.
  virtual void Render(int left, int top, int width, int height) = 0;

  // Description:
  // This method should normally  called only by the form.
  // The only exception is during a resize.
  virtual void UpdateStatusBar() = 0;

  // Description:
  // During a CMake run, an error handle should add errors
  // to be displayed afterwards.
  virtual void AddError(const std::string&, const char*) {}

  // Description:
  // Turn debugging on. This will create ccmakelog.txt.
  static void DebugStart();

  // Description:
  // Turn debugging off. This will close ccmakelog.txt.
  static void DebugEnd();

  // Description:
  // Write a debugging message.
  static void LogMessage(const char* msg);

  // Description:
  // Return the FORM. Should be only used by low-level methods.
  FORM* GetForm() { return this->Form; }

  static cmCursesForm* CurrentForm;

  /*
   * Mutex used to protect the global curses 'stdscr' object
   *
   * This is required to properly handle window resize as a signal (SIGWINCH)
   * Since signals can interrupt code at any point, protecting and
   * synchronizing the screen modifications is required.
   *
   * This is currently only used to protect the current log display during
   * configure which is the most probable data race occurence.
   * This should probably be done for all screen modification, but doing that
   * correctly and properly may require a consequent refactoring.
   */
  static std::mutex ScreenMutex;

protected:
  static cmsys::ofstream DebugFile;
  static bool Debug;

  FORM* Form;
};

#endif // cmCursesForm_h
