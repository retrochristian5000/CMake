/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#include "EnvironmentDialog.h"

#include <QDialogButtonBox>
#include <QGridLayout>
#include <QItemSelectionModel>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>

#include "EnvironmentItemModel.h"
#include "EnvironmentSearchFilter.h"

EnvironmentDialog::EnvironmentDialog(const QProcessEnvironment& environment,
                                     QWidget* parent)
  : QDialog(parent)
{
  this->setupUi(this);

  this->RemoveEntry->setEnabled(false);

  this->m_model = new EnvironmentItemModel(environment, this);
  this->m_filter = new EnvironmentSearchFilter(this);
  this->m_filter->setSourceModel(this->m_model);
  this->Environment->setModel(this->m_filter);

  this->Environment->setUniformRowHeights(true);
  this->Environment->setRootIsDecorated(false);
  this->Environment->setSelectionMode(QAbstractItemView::ExtendedSelection);
  this->Environment->setSelectionBehavior(QAbstractItemView::SelectRows);

  QObject::connect(this->AddEntry, &QToolButton::clicked, this,
                   &EnvironmentDialog::addEntry);
  QObject::connect(this->RemoveEntry, &QToolButton::clicked, this,
                   &EnvironmentDialog::removeSelectedEntries);
  QObject::connect(this->Search, &QLineEdit::textChanged, this->m_filter,
                   &EnvironmentSearchFilter::setFilterFixedString);
  QObject::connect(this->Environment->selectionModel(),
                   &QItemSelectionModel::selectionChanged, this,
                   &EnvironmentDialog::selectionChanged);
}

QProcessEnvironment EnvironmentDialog::environment() const
{
  return this->m_model->environment();
}

void EnvironmentDialog::addEntry()
{
  // Build the dialog manually because it's simple enough
  QDialog dialog(this);
  dialog.setWindowTitle("Add Environment Variable");

  auto* layout = new QGridLayout;
  dialog.setLayout(layout);

  auto* nameLabel = new QLabel;
  nameLabel->setText("Name:");
  layout->addWidget(nameLabel, 0, 0);

  auto* nameEdit = new QLineEdit;
  nameEdit->setObjectName("name");
  layout->addWidget(nameEdit, 0, 1);

  auto* valueLabel = new QLabel;
  valueLabel->setText("Value:");
  layout->addWidget(valueLabel, 1, 0);

  auto* valueEdit = new QLineEdit;
  valueEdit->setObjectName("value");
  layout->addWidget(valueEdit, 1, 1);

  auto* buttons = new QDialogButtonBox;
  buttons->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  QObject::connect(
    buttons, &QDialogButtonBox::accepted, &dialog,
    [this, &dialog, nameEdit]() {
      auto text = nameEdit->text();
      if (text.isEmpty()) {
        QMessageBox::critical(&dialog, "Error", "Name must be non-empty.");
        return;
      }

      auto* model = this->Environment->model();
      for (int i = 0; i < model->rowCount(); ++i) {
        if (model->data(model->index(i, 0), Qt::DisplayRole) == text) {
          QMessageBox::critical(
            &dialog, "Error",
            tr("Environment variable \"%1\" already exists.").arg(text));
          return;
        }
      }

      dialog.accept();
    });
  QObject::connect(buttons, &QDialogButtonBox::rejected, &dialog,
                   &QDialog::reject);
  layout->addWidget(buttons, 2, 0, 1, 2);

  if (dialog.exec() == QDialog::Accepted) {
    this->m_model->insertVariable(0, nameEdit->text(), valueEdit->text());
  }
}

void EnvironmentDialog::removeSelectedEntries()
{
  QModelIndexList idxs = this->Environment->selectionModel()->selectedRows();
  QList<QPersistentModelIndex> pidxs;
  foreach (QModelIndex const& i, idxs) {
    pidxs.append(i);
  }
  foreach (QPersistentModelIndex const& pi, pidxs) {
    this->Environment->model()->removeRow(pi.row(), pi.parent());
  }
}

void EnvironmentDialog::selectionChanged()
{
  auto selected = this->Environment->selectionModel()->selectedRows();
  this->RemoveEntry->setEnabled(!selected.isEmpty());
}
