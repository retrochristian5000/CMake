/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#include "QCMakeCacheModelTest.h"

#include <algorithm>
#include <iostream>

#include "QCMakeCacheView.h"
#include <QtTest>

namespace {
QCMakeProperty makeProperty(
  const QString& name, const QString& value,
  QCMakeProperty::PropertyType type = QCMakeProperty::STRING,
  bool advanced = false)
{
  return QCMakeProperty{
    /*Key=*/name,
    /*Value=*/value,
    /*Strings=*/{},
    /*Help=*/"",
    /*Type=*/type,
    /*Advanced=*/advanced,
  };
}
}

void QCMakeCacheModelTest::setNewProperties()
{
  QFETCH(QCMakePropertyList, oldList);
  QFETCH(QCMakePropertyList, newList);
  QFETCH(QVector<QVariant>, background);

  QCMakeCacheModel model;
  model.setViewType(QCMakeCacheModel::FlatView);
  model.setProperties(oldList);
  model.setProperties(newList);

  QVector<QVariant> actualBackground(model.rowCount());
  for (int i = 0; i < model.rowCount(); ++i) {
    actualBackground[i] = model.data(model.index(i, 0), Qt::BackgroundRole);
  }

  QCOMPARE(actualBackground, background);
}

void QCMakeCacheModelTest::setNewProperties_data()
{
  QTest::addColumn<QCMakePropertyList>("oldList");
  QTest::addColumn<QCMakePropertyList>("newList");
  QTest::addColumn<QVector<QVariant>>("background");

  QTest::newRow("empty") << QCMakePropertyList{} << QCMakePropertyList{}
                         << QVector<QVariant>{};
  QTest::newRow("noNew") << QCMakePropertyList{ makeProperty("VARIABLE_1",
                                                             "Value 1") }
                         << QCMakePropertyList{} << QVector<QVariant>{};
  QTest::newRow("allNew")
    << QCMakePropertyList{}
    << QCMakePropertyList{ makeProperty("VARIABLE_1", "Value 1") }
    << QVector<QVariant>{ QBrush{ QColor{ 255, 100, 100 } } };
  QTest::newRow("mixed")
    << QCMakePropertyList{ makeProperty("VARIABLE_1", "Value 1") }
    << QCMakePropertyList{ makeProperty("VARIABLE_2", "Value 2") }
    << QVector<QVariant>{ QBrush{ QColor{ 255, 100, 100 } } };
  QTest::newRow("overridden")
    << QCMakePropertyList{ makeProperty("VARIABLE_1", "Value 1") }
    << QCMakePropertyList{ makeProperty("VARIABLE_1", "Overridden value"),
                           makeProperty("VARIABLE_2", "Value 2") }
    << QVector<QVariant>{ QBrush{ QColor{ 255, 100, 100 } }, QVariant{} };
}

QTEST_MAIN(QCMakeCacheModelTest)
