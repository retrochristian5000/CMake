/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#include "QCMakePresetComboBoxTest.h"

#include <QtTest>

void QCMakePresetComboBoxTest::changePresets()
{
  QCMakePresetComboBox box;
  QSignalSpy presetChanged(&box, &QCMakePresetComboBox::presetChanged);

  QCOMPARE(presetChanged.size(), 0);

  box.setPresets({});
  QCOMPARE(presetChanged.size(), 0);

  box.setPresetName(QString{});
  QCOMPARE(presetChanged.size(), 0);

  box.setPresets({
    {
      /*name=*/"preset",
      /*description=*/"",
      /*longDescription=*/"",
      /*generator=*/"Ninja",
      /*architecture=*/"",
      /*toolset=*/"",
      /*enabled=*/true,
    },
  });
  QCOMPARE(presetChanged.size(), 0);

  box.setPresetName(QString{});
  QCOMPARE(presetChanged.size(), 0);

  box.setPresetName("noexist");
  QCOMPARE(presetChanged.size(), 0);

  box.setPresetName("preset");
  QCOMPARE(presetChanged.size(), 1);
  QCOMPARE(presetChanged.last(), QList<QVariant>{ "preset" });

  box.setPresets({
    {
      /*name=*/"preset",
      /*description=*/"",
      /*longDescription=*/"",
      /*generator=*/"Ninja Multi-Config",
      /*architecture=*/"",
      /*toolset=*/"",
      /*enabled=*/true,
    },
  });
  QCOMPARE(presetChanged.size(), 1);

  box.setPresetName("noexist");
  QCOMPARE(presetChanged.size(), 2);
  QCOMPARE(presetChanged.last(), QList<QVariant>{ QString{} });

  box.setPresetName("preset");
  QCOMPARE(presetChanged.size(), 3);
  QCOMPARE(presetChanged.last(), QList<QVariant>{ "preset" });

  box.setPresets({});
  QCOMPARE(presetChanged.size(), 4);
  QCOMPARE(presetChanged.last(), QList<QVariant>{ QString{} });
}

QTEST_MAIN(QCMakePresetComboBoxTest)
