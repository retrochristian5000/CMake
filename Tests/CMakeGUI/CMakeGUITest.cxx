/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#include "CMakeGUITest.h"

#include <iostream>

#include <QApplication>
#include <QString>
#include <QStringList>
#include <QtGlobal>
#include <QtTest>

#include "CMakeSetupDialog.h"

void CMakeGUITest::initTestCase()
{
  for (auto w : QApplication::allWidgets()) {
    auto cmakeWindow = qobject_cast<CMakeSetupDialog*>(w);
    if (cmakeWindow) {
      this->window = cmakeWindow;
      break;
    }
  }

  QVERIFY(this->window);
}

void CMakeGUITest::cleanupTestCase()
{
  QMetaObject::invokeMethod(QApplication::instance(), QApplication::quit,
                            Qt::QueuedConnection);
}

void CMakeGUITest::sourceAndBinaryDir()
{
  QCOMPARE(this->window->SourceDirectory->text(),
           CMake_SOURCE_DIR "/Tests/CMakeGUI/SimpleProject");
  QCOMPARE(this->window->BinaryDirectory->currentText(),
           CMake_BINARY_DIR "/Tests/CMakeGUI/sourceAndBinaryDir");
}

void CMakeGUITest::sourceAndBinaryDirRelative()
{
  QCOMPARE(this->window->SourceDirectory->text(),
           CMake_SOURCE_DIR "/Tests/CMakeGUI/SimpleProject");
  QCOMPARE(this->window->BinaryDirectory->currentText(),
           CMake_BINARY_DIR "/Tests/CMakeGUI/sourceAndBinaryDirRelative");
}

void CMakeGUITest::sourceDir()
{
  QCOMPARE(this->window->SourceDirectory->text(),
           CMake_SOURCE_DIR "/Tests/CMakeGUI/SimpleProject");
  QCOMPARE(this->window->BinaryDirectory->currentText(),
           CMake_BINARY_DIR "/Tests/CMakeGUI");
}

void CMakeGUITest::sourceDirNoExist()
{
  QCOMPARE(this->window->SourceDirectory->text(), "");
  QCOMPARE(this->window->BinaryDirectory->currentText(), "");
}

void CMakeGUITest::binaryDir()
{
  QCOMPARE(this->window->SourceDirectory->text(), "");
  QCOMPARE(this->window->BinaryDirectory->currentText(), CMake_BINARY_DIR);
}

void CMakeGUITest::binaryDirNoExist()
{
  QCOMPARE(this->window->SourceDirectory->text(), "");
  QCOMPARE(this->window->BinaryDirectory->currentText(), "");
}

int CMakeGUIMainTest()
{
  auto nameArray = qgetenv("CMake_GUI_TEST_NAME");
  auto name = QString::fromLocal8Bit(nameArray);
  if (name.isEmpty()) {
    return 0;
  }

  QStringList args{ "CMakeGUITest", name };
  CMakeGUITest obj;
  return QTest::qExec(&obj, args);
}
