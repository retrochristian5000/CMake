/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#pragma once

#include <QSortFilterProxyModel>

class EnvironmentSearchFilter : public QSortFilterProxyModel
{
  Q_OBJECT
public:
  EnvironmentSearchFilter(QObject* parent = nullptr);

protected:
  bool filterAcceptsRow(int row, const QModelIndex& parent) const override;
};
