/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#ifndef cmLocalUVisionGenerator_h
#define cmLocalUVisionGenerator_h

#include "cmConfigure.h" // IWYU pragma: keep

#include "cmLocalGenerator.h"

class cmGlobalGenerator;
class cmMakefile;

/** \class cmLocalUVisionGenerator
 * \brief Local generator for uVision projects
 */
class cmLocalUVisionGenerator : public cmLocalGenerator
{
public:
  cmLocalUVisionGenerator(cmGlobalGenerator* gg, cmMakefile* mf);
  ~cmLocalUVisionGenerator() override;

  /**
   * Generate the makefile for this directory.
   */
  void Generate() override;
};

#endif
