/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#include "EnvironmentSearchFilter.h"

EnvironmentSearchFilter::EnvironmentSearchFilter(QObject* parent)
  : QSortFilterProxyModel(parent)
{
}

bool EnvironmentSearchFilter::filterAcceptsRow(int row,
                                               const QModelIndex& parent) const
{
  auto* model = this->sourceModel();
  auto key =
    model->data(model->index(row, 0, parent), Qt::DisplayRole).toString();
  return key.contains(this->filterRegExp());
}
