/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#pragma once

#include <QObject>
#include <QProcessEnvironment>
#include <QStandardItemModel>

class EnvironmentItemModel : public QStandardItemModel
{
  Q_OBJECT
public:
  EnvironmentItemModel(const QProcessEnvironment& environment,
                       QObject* parent = nullptr);

  QProcessEnvironment environment() const;
  void clear();

  QModelIndex buddy(const QModelIndex& index) const override;

public slots:
  void appendVariable(const QString& key, const QString& value);
  void insertVariable(int row, const QString& key, const QString& value);
};
