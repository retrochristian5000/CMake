/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#pragma once

#include <QString>
#include <QVariant>

#include "cmCMakePresetsFile.h"

class QCMakePreset
{
public:
  QString name;
  QString display;
  QString tooltip;
  QString generator;
  QString architecture;
  QString toolset;
  bool enabled;
  int index;

  bool operator==(const QCMakePreset& other) const;
  bool operator!=(const QCMakePreset& other) const;
};

Q_DECLARE_METATYPE(QCMakePreset)
