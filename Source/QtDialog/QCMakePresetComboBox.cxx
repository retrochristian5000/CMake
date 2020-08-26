/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#include "QCMakePresetComboBox.h"

#include "QCMakePresetItemModel.h"

QCMakePresetComboBox::QCMakePresetComboBox(QWidget* parent)
  : QComboBox(parent)
{
  this->m_model = new QCMakePresetItemModel(this);
  this->setModel(this->m_model);

  QObject::connect(
    this, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
    [this](int /*row*/) { emit this->presetChanged(this->presetName()); });
}

const QVector<QCMakePreset>& QCMakePresetComboBox::presets() const
{
  return this->m_model->presets();
}

QString QCMakePresetComboBox::presetName() const
{
  auto preset = this->currentData();
  if (preset.canConvert<QCMakePreset>()) {
    return preset.value<QCMakePreset>().name;
  }
  return QString{};
}

void QCMakePresetComboBox::setPresets(const QVector<QCMakePreset>& presets)
{
  this->m_model->setPresets(presets);
  this->setPresetName(QString{});
}

void QCMakePresetComboBox::setPresetName(const QString& name)
{
  this->setCurrentIndex(this->m_model->presetNameToRow(name));
}
