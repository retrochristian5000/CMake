/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#include "EnvironmentItemModel.h"

#include <QStandardItem>
#include <QtDebug>

EnvironmentItemModel::EnvironmentItemModel(
  const QProcessEnvironment& environment, QObject* parent)
  : QStandardItemModel(parent)
{
  this->clear();
  for (auto const& key : environment.keys()) {
    auto value = environment.value(key);
    this->appendVariable(key, value);
  }
}

QProcessEnvironment EnvironmentItemModel::environment() const
{
  QProcessEnvironment env;
  for (int i = 0; i < this->rowCount(); ++i) {
    auto name = this->data(this->index(i, 0), Qt::DisplayRole).toString();
    auto value = this->data(this->index(i, 1), Qt::DisplayRole).toString();
    env.insert(name, value);
  }
  return env;
}

void EnvironmentItemModel::clear()
{
  this->QStandardItemModel::clear();

  QStringList labels;
  labels << tr("Name") << tr("Value");
  this->setHorizontalHeaderLabels(labels);
}

QModelIndex EnvironmentItemModel::buddy(const QModelIndex& index) const
{
  if (index.column() == 0) {
    return this->index(index.row(), index.column() + 1, index.parent());
  }
  return index;
}

void EnvironmentItemModel::appendVariable(const QString& key,
                                          const QString& value)
{
  this->insertVariable(this->rowCount(), key, value);
}

void EnvironmentItemModel::insertVariable(int row, const QString& key,
                                          const QString& value)
{
  for (int i = 0; i < this->rowCount(); ++i) {
    if (this->data(this->index(i, 0), Qt::DisplayRole) == key) {
      this->setData(this->index(i, 1), value, Qt::DisplayRole);
      return;
    }
  }

  auto* keyItem = new QStandardItem(key);
  auto* valueItem = new QStandardItem(value);
  this->insertRow(row, { keyItem, valueItem });
}
