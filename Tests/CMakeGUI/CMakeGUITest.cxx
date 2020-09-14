/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#include "CMakeGUITest.h"

#include <QApplication>
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
