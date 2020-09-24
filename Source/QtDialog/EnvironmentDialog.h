/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#pragma once

#include <QDialog>
#include <QObject>
#include <QProcessEnvironment>

#include "ui_EnvironmentDialog.h"

class EnvironmentItemModel;
class EnvironmentSearchFilter;

class EnvironmentDialog
  : public QDialog
  , public Ui::EnvironmentDialog
{
  Q_OBJECT
public:
  EnvironmentDialog(const QProcessEnvironment& environment,
                    QWidget* parent = nullptr);

  QProcessEnvironment environment() const;

protected slots:
  void addEntry();
  void removeSelectedEntries();
  void selectionChanged();

private:
  EnvironmentItemModel* m_model;
  EnvironmentSearchFilter* m_filter;
};
