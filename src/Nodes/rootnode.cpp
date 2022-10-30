#include "rootnode.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QListView>

RootNode::RootNode()
{
    typeOfNode = ROOT;
    setWindowTitle("Outputs");

    // add default outputs
    outputs = new QStringList();
    *outputs << "lambda" << "mu" << "rho";
    outputsModel = new QStringListModel(*outputs);

    createLayout();

    setGeometry(QRect(0, 0, sizeHint().width(), sizeHint().height()));
}

void RootNode::createLayout()
{
    QVBoxLayout* globalLayout = new QVBoxLayout();
    addTitleLayout(globalLayout);

    QListView* outputsList = new QListView();
    outputsList->setFixedHeight(100);
    outputsList->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    outputsList->setModel(outputsModel);

    globalLayout->addWidget(outputsList);

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

    /// TODO: connect buttons
    globalLayout->addLayout(buttonsLayout);

    addComponentsLayout(globalLayout);
    this->setLayout(globalLayout);
}

void RootNode::mousePressEvent(QMouseEvent* event)
{
    QWidget::mousePressEvent(event);
}
