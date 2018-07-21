/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#include "cmGhsMultiGpj.h"

#include "cmGeneratedFileStream.h"

const char* GHS_TAG[] = { "[INTEGRITY Application]",
                          "[Library]",
                          "[Project]",
                          "[Program]",
                          "[Reference]",
                          "[Subproject]" };

void GhsMultiGpj::WriteGpjTag(Types const gpjType,
                              cmGeneratedFileStream* const filestream)
{
  char const* tag;
  switch (gpjType) {
    case INTERGRITY_APPLICATION:
    case LIBRARY:
    case PROJECT:
    case PROGRAM:
    case REFERENCE:
    case SUBPROJECT:
      tag = GHS_TAG[gpjType];
      break;
    default:
      tag = "";
  }
  *filestream << tag << std::endl;
}
