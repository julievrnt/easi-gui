#include "rootnodedialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

RootNodeDialog::RootNodeDialog(QStringList* outputs, QWidget* parent) : QDialog(parent)
{
    this->outputs = outputs;
    setWindowTitle("Modify Outputs");
    outputsModel = new QStringListModel(*this->outputs);
    createLayout();
    //setAttribute(Qt::WA_DeleteOnClose);
    setGeometry(QRect(0, 0, sizeHint().width(), sizeHint().height()));
}

RootNodeDialog::~RootNodeDialog()
{
    // outputs is deleted by root node !
    delete outputsModel;
    delete outputsListView;
}

void RootNodeDialog::createLayout()
{
    QVBoxLayout* globalLayout = new QVBoxLayout();

    // list of outputs added (default: lambda, mu, rho)
    outputsListView = new QListView();
    //outputsListView->setFixedHeight(100);
    outputsListView->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    outputsListView->setModel(outputsModel);
    outputsListView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    globalLayout->addWidget(outputsListView);

    // buttons to add, edit and delete outputs
    QHBoxLayout* buttonsLayout = new QHBoxLayout();
    QPushButton* addButton = new QPushButton();
    addButton->setText("Add");
    QPushButton* editButton = new QPushButton();
    editButton->setText("Edit");
    QPushButton* deleteButton = new QPushButton();
    deleteButton->setText("Delete");

    buttonsLayout->addWidget(addButton);
    buttonsLayout->addWidget(editButton);
    buttonsLayout->addWidget(deleteButton);

    // connect buttons
    connect(addButton, SIGNAL(clicked(bool)), this, SLOT(addOutputButtonClicked(bool)));
    connect(editButton, SIGNAL(clicked(bool)), this, SLOT(editOutputButtonClicked(bool)));
    connect(deleteButton, SIGNAL(clicked(bool)), this, SLOT(deleteOutputButtonClicked(bool)));

    globalLayout->addLayout(buttonsLayout);

    // add a save button !
    QPushButton* saveButton = new QPushButton();
    saveButton->setText("Save");
    connect(saveButton, SIGNAL(clicked(bool)), this, SLOT(saveOutputsButtonClicked(bool)));
    globalLayout->addWidget(saveButton);

    this->setLayout(globalLayout);
}

void RootNodeDialog::addOutput()
{
    int row = outputsModel->rowCount();
    outputsModel->insertRow(row);
    QModelIndex index = outputsModel->index(row);

    outputsListView->setCurrentIndex(index);
    outputsListView->edit(index);
}

void RootNodeDialog::editOutput()
{
    outputsListView->edit(outputsListView->currentIndex());
}

void RootNodeDialog::deleteOutput()
{
    outputsModel->removeRow(outputsListView->currentIndex().row());
}

void RootNodeDialog::saveOutputs()
{
    outputs->clear();
    *outputs = outputsModel->stringList();
}

void RootNodeDialog::addOutputButtonClicked(bool clicked)
{
    addOutput();
}

void RootNodeDialog::editOutputButtonClicked(bool clicked)
{
    editOutput();
}

void RootNodeDialog::deleteOutputButtonClicked(bool clicked)
{
    deleteOutput();
}

void RootNodeDialog::saveOutputsButtonClicked(bool clicked)
{
    saveOutputs();
}
