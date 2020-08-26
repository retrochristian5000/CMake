/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#include "QCMakePreset.h"

bool QCMakePreset::operator==(const QCMakePreset& other) const
{
  return this->name == other.name && this->generator == other.generator &&
    this->enabled == other.enabled && this->index == other.index;
}

bool QCMakePreset::operator!=(const QCMakePreset& other) const
{
  return !(*this == other);
}
