/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#include "CMakeGUITest.h"

#include <QApplication>
#include <QEventLoop>
#include <QSettings>
#include <QString>
#include <QStringList>
#include <QtGlobal>
#include <QtTest>

#include "CMakeSetupDialog.h"

CMakeGUITest::CMakeGUITest(CMakeSetupDialog* window, QObject* parent)
  : QObject(parent)
  , m_window(window)
{
}

void CMakeGUITest::sourceBinaryArgs()
{
  QFETCH(QString, sourceDir);
  QFETCH(QString, binaryDir);

  QCOMPARE(this->m_window->SourceDirectory->text(), sourceDir);
  QCOMPARE(this->m_window->BinaryDirectory->currentText(), binaryDir);
}

void CMakeGUITest::sourceBinaryArgs_data()
{
  QTest::addColumn<QString>("sourceDir");
  QTest::addColumn<QString>("binaryDir");

  QTest::newRow("sourceAndBinaryDir")
    << CMake_SOURCE_DIR "/Tests/CMakeGUI/SimpleProject"
    << CMake_BINARY_DIR "/Tests/CMakeGUI/sourceAndBinaryDir";
  QTest::newRow("sourceAndBinaryDirRelative")
    << CMake_SOURCE_DIR "/Tests/CMakeGUI/SimpleProject"
    << CMake_BINARY_DIR "/Tests/CMakeGUI/sourceAndBinaryDirRelative";
  QTest::newRow("sourceDir")
    << CMake_SOURCE_DIR "/Tests/CMakeGUI/SimpleProject"
    << CMake_BINARY_DIR "/Tests/CMakeGUI";
  QTest::newRow("sourceDirNoExist")
    << "" << CMake_BINARY_DIR "/Tests/CMakeGUI/no/exist/1";
  QTest::newRow("binaryDir") << "" << CMake_BINARY_DIR;
  QTest::newRow("binaryDirNoExist")
    << "" << CMake_BINARY_DIR "/Tests/CMakeGUI/no/exist/1";
}

void CMakeGUITest::presetArg()
{
  QFETCH(QString, presetName);
  QFETCH(QString, sourceDir);
  QFETCH(QString, binaryDir);
  QFETCH(QCMakePropertyList, properties);

  QEventLoop loop;
  QTimer::singleShot(5000, &loop, &QEventLoop::quit);
  loop.exec();

  QCOMPARE(this->m_window->BuildPreset->presetName(), presetName);
  QCOMPARE(this->m_window->SourceDirectory->text(), sourceDir);
  QCOMPARE(this->m_window->BinaryDirectory->currentText(), binaryDir);

  auto actualProperties =
    this->m_window->CacheValues->cacheModel()->properties();
  QCOMPARE(actualProperties.size(), properties.size());
  for (int i = 0; i < actualProperties.size(); ++i) {
    // operator==() only compares Key, we need to compare Value and Type too
    QCOMPARE(actualProperties[i].Key, properties[i].Key);
    QCOMPARE(actualProperties[i].Value, properties[i].Value);
    QCOMPARE(actualProperties[i].Type, properties[i].Type);
  }
}

void CMakeGUITest::presetArg_data()
{
  QTest::addColumn<QString>("presetName");
  QTest::addColumn<QString>("sourceDir");
  QTest::addColumn<QString>("binaryDir");
  QTest::addColumn<QCMakePropertyList>("properties");

  QCMakePropertyList presetProperties{
    QCMakeProperty{
      /*Key=*/"FALSE_VARIABLE",
      /*Value=*/false,
      /*Strings=*/{},
      /*Help=*/"",
      /*Type=*/QCMakeProperty::BOOL,
      /*Advanced=*/false,
    },
    QCMakeProperty{
      /*Key=*/"FILEPATH_VARIABLE",
      /*Value=*/
      CMake_SOURCE_DIR "/Tests/CMakeGUI/SimpleProject/CMakeLists.txt",
      /*Strings=*/{},
      /*Help=*/"",
      /*Type=*/QCMakeProperty::FILEPATH,
      /*Advanced=*/false,
    },
    QCMakeProperty{
      /*Key=*/"ON_VARIABLE",
      /*Value=*/true,
      /*Strings=*/{},
      /*Help=*/"",
      /*Type=*/QCMakeProperty::BOOL,
      /*Advanced=*/false,
    },
    QCMakeProperty{
      /*Key=*/"PATH_VARIABLE",
      /*Value=*/CMake_SOURCE_DIR "/Tests/CMakeGUI/SimpleProject",
      /*Strings=*/{},
      /*Help=*/"",
      /*Type=*/QCMakeProperty::PATH,
      /*Advanced=*/false,
    },
    QCMakeProperty{
      /*Key=*/"STRING_VARIABLE",
      /*Value=*/"String value",
      /*Strings=*/{},
      /*Help=*/"",
      /*Type=*/QCMakeProperty::STRING,
      /*Advanced=*/false,
    },
    QCMakeProperty{
      /*Key=*/"UNINITIALIZED_VARIABLE",
      /*Value=*/"Uninitialized value",
      /*Strings=*/{},
      /*Help=*/"",
      /*Type=*/QCMakeProperty::STRING,
      /*Advanced=*/false,
    },
  };

  QTest::newRow("preset") << "ninja"
                          << CMake_SOURCE_DIR "/Tests/CMakeGUI/SimpleProject"
                          << CMake_SOURCE_DIR
    "/Tests/CMakeGUI/SimpleProject/build"
                          << presetProperties;
  QTest::newRow("presetInBuildDirectory")
    << "ninja" << CMake_SOURCE_DIR "/Tests/CMakeGUI/SimpleProject"
    << CMake_SOURCE_DIR "/Tests/CMakeGUI/SimpleProject/build"
    << presetProperties;
  QTest::newRow("noexist") << QString{}
                           << CMake_SOURCE_DIR "/Tests/CMakeGUI/SimpleProject"
                           << CMake_BINARY_DIR "/Tests/CMakeGUI/no/exist/1"
                           << QCMakePropertyList{};
}

void SetupDefaultQSettings()
{
  QSettings::setDefaultFormat(QSettings::IniFormat);
  QSettings::setPath(QSettings::IniFormat, QSettings::UserScope,
                     CMake_BINARY_DIR "/Tests/CMakeGUI");
}

int CMakeGUIExec(CMakeSetupDialog* window)
{
  auto nameArray = qgetenv("CMake_GUI_TEST_NAME");
  auto name = QString::fromLocal8Bit(nameArray);
  if (name.isEmpty()) {
    return QApplication::exec();
  }

  QStringList args{ "CMakeGUITest", name };
  CMakeGUITest obj(window);
  return QTest::qExec(&obj, args);
}
