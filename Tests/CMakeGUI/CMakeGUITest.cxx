/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#include "CMakeGUITest.h"

#include <QApplication>
#include <QEventLoop>
#include <QSettings>
#include <QString>
#include <QStringList>
#include <QTimer>
#include <QtGlobal>
#include <QtTest>

#include "CMakeSetupDialog.h"

using WindowSetupHelper = std::function<void(CMakeSetupDialog*)>;
Q_DECLARE_METATYPE(WindowSetupHelper)

namespace {
void loopSleep(int msecs = 100)
{
  QEventLoop loop;
  QTimer::singleShot(msecs, &loop, &QEventLoop::quit);
  loop.exec();
}
}

CMakeGUITest::CMakeGUITest(CMakeSetupDialog* window, QObject* parent)
  : QObject(parent)
  , m_window(window)
{
}

void CMakeGUITest::sourceBinaryArgs()
{
  QFETCH(QString, sourceDir);
  QFETCH(QString, binaryDir);

  // Wait a bit for everything to update
  loopSleep();

  QCOMPARE(this->m_window->SourceDirectory->text(), sourceDir);
  QCOMPARE(this->m_window->BinaryDirectory->currentText(), binaryDir);
}

void CMakeGUITest::sourceBinaryArgs_data()
{
  QTest::addColumn<QString>("sourceDir");
  QTest::addColumn<QString>("binaryDir");

  QTest::newRow("sourceAndBinaryDir")
    << CMakeGUITest_BINARY_DIR "/sourceBinaryArgs-sourceAndBinaryDir/src"
    << CMakeGUITest_BINARY_DIR "/sourceBinaryArgs-sourceAndBinaryDir/build";
  QTest::newRow("sourceAndBinaryDirRelative") << CMakeGUITest_BINARY_DIR
    "/sourceBinaryArgs-sourceAndBinaryDirRelative/src"
                                              << CMakeGUITest_BINARY_DIR
    "/sourceBinaryArgs-sourceAndBinaryDirRelative/build";
  QTest::newRow("sourceDir")
    << CMakeGUITest_BINARY_DIR "/sourceBinaryArgs-sourceDir/src"
    << CMakeGUITest_BINARY_DIR "/sourceBinaryArgs-sourceDir";
  QTest::newRow("binaryDir")
    << CMakeGUITest_BINARY_DIR "/sourceBinaryArgs-binaryDir/src"
    << CMakeGUITest_BINARY_DIR "/sourceBinaryArgs-binaryDir/build";
  QTest::newRow("noExist") << ""
                           << "";
  QTest::newRow("noExistConfig")
    << ""
    << CMakeGUITest_BINARY_DIR "/sourceBinaryArgs-noExistConfig/oldbuild";
  QTest::newRow("noExistConfigExists")
    << CMakeGUITest_BINARY_DIR "/sourceBinaryArgs-noExistConfigExists/src"
    << CMakeGUITest_BINARY_DIR "/sourceBinaryArgs-noExistConfigExists/build";
}

void CMakeGUITest::presetArg()
{
  QFETCH(WindowSetupHelper, setupFunction);
  QFETCH(QString, presetName);
  QFETCH(QString, sourceDir);
  QFETCH(QString, binaryDir);
  QFETCH(QCMakePropertyList, properties);

  if (setupFunction) {
    setupFunction(this->m_window);
  }

  // Wait a bit for everything to update
  loopSleep();

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

namespace {
QCMakePropertyList makePresetProperties(const QString& name)
{
  return QCMakePropertyList{
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
      QString::fromLocal8Bit(CMakeGUITest_BINARY_DIR "/%1/src/CMakeLists.txt")
        .arg(name),
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
      /*Value=*/
      QString::fromLocal8Bit(CMakeGUITest_BINARY_DIR "/%1/src").arg(name),
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
}
}

void CMakeGUITest::presetArg_data()
{
  QTest::addColumn<WindowSetupHelper>("setupFunction");
  QTest::addColumn<QString>("presetName");
  QTest::addColumn<QString>("sourceDir");
  QTest::addColumn<QString>("binaryDir");
  QTest::addColumn<QCMakePropertyList>("properties");

  QTest::newRow("preset") << WindowSetupHelper{} << "ninja"
                          << CMakeGUITest_BINARY_DIR "/presetArg-preset/src"
                          << CMakeGUITest_BINARY_DIR
    "/presetArg-preset/src/build"
                          << makePresetProperties("presetArg-preset");
  QTest::newRow("presetBinary")
    << WindowSetupHelper{} << "ninja"
    << CMakeGUITest_BINARY_DIR "/presetArg-presetBinary/src"
    << CMakeGUITest_BINARY_DIR "/presetArg-presetBinary/build"
    << makePresetProperties("presetArg-presetBinary");
  QTest::newRow("presetBinaryChange")
    << WindowSetupHelper{ [](CMakeSetupDialog* window) {
         loopSleep();
         window->BuildPreset->setPresetName("ninja2");
       } }
    << "ninja2" << CMakeGUITest_BINARY_DIR "/presetArg-presetBinaryChange/src"
    << CMakeGUITest_BINARY_DIR "/presetArg-presetBinaryChange/src/build"
    << makePresetProperties("presetArg-presetBinaryChange");
  QTest::newRow("noPresetBinaryChange")
    << WindowSetupHelper{ [](CMakeSetupDialog* window) {
         loopSleep();
         window->BuildPreset->setPresetName("ninja");
       } }
    << "ninja" << CMakeGUITest_BINARY_DIR "/presetArg-noPresetBinaryChange/src"
    << CMakeGUITest_BINARY_DIR "/presetArg-noPresetBinaryChange/src/build"
    << makePresetProperties("presetArg-noPresetBinaryChange");
  QTest::newRow("presetConfigExists")
    << WindowSetupHelper{} << "ninja"
    << CMakeGUITest_BINARY_DIR "/presetArg-presetConfigExists/src"
    << CMakeGUITest_BINARY_DIR "/presetArg-presetConfigExists/src/build"
    << makePresetProperties("presetArg-presetConfigExists");
  QTest::newRow("noExist") << WindowSetupHelper{} << QString{}
                           << CMakeGUITest_BINARY_DIR "/presetArg-noExist/src"
                           << "" << QCMakePropertyList{};
}

void SetupDefaultQSettings()
{
  QSettings::setDefaultFormat(QSettings::IniFormat);
  QSettings::setPath(QSettings::IniFormat, QSettings::UserScope,
                     QString::fromLocal8Bit(qgetenv("CMake_GUI_CONFIG_DIR")));
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
