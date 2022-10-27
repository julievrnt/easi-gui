#include "componentswidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QPushButton>

ComponentsWidget::ComponentsWidget(Node* parentNode, QWidget* parent)
    : QWidget(parent)
{
    outputs = parentNode->getOutputs();
    setGeometry(QRect(0,0,parentNode->width(), 100));
    createLayout();
}

ComponentsWidget::~ComponentsWidget()
{
    /// TODO
    delete outputs;
    foreach(ConnectorWidget* component, components)
    {
        delete component;
    }
    delete connectorsLayout;
}

QStringList* ComponentsWidget::getOutputs() const
{
    return outputs;
}

int ComponentsWidget::getNumberComponents() const
{
    return components.size();
}

void ComponentsWidget::createLayout()
{
    QVBoxLayout* globalLayout = new QVBoxLayout();

    // add line to separate the rest from the components
    QFrame* line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    globalLayout->addWidget(line);

    // add the main part of the components
    QHBoxLayout* componentsLayout = new QHBoxLayout();
    componentsLayout->setObjectName("components");

    QLabel* componentsLabel = new QLabel();
    componentsLabel->setText("Components");
    componentsLayout->addWidget(componentsLabel);

    connectorsLayout = new QVBoxLayout();
    connectorsLayout->setObjectName("connectorsLayout");
    ConnectorWidget* connector = new ConnectorWidget(this->getOutputs());
    components.append(connector);
    connectorsLayout->addWidget(connector);

    componentsLayout->addLayout(connectorsLayout);
    globalLayout->addLayout(componentsLayout);

    // add buttons
    QHBoxLayout* buttonsLayout = new QHBoxLayout();
    QPushButton* addButton = new QPushButton();
    addButton->setText("Add Component");
    QPushButton* deleteButton = new QPushButton();
    deleteButton->setText("Delete Component");

    buttonsLayout->addWidget(addButton);
    buttonsLayout->addWidget(deleteButton);

    // connect buttons
    connect(addButton, SIGNAL(clicked(bool)), this, SLOT(addButtonClicked(bool)));
    connect(deleteButton, SIGNAL(clicked(bool)), this, SLOT(deleteButtonClicked(bool)));

    globalLayout->addLayout(buttonsLayout);
    setLayout(globalLayout);
}

void ComponentsWidget::addComponent()
{
    ConnectorWidget* connector = new ConnectorWidget(this->getOutputs());
    connectorsLayout->addWidget(connector);

    components.append(connector);
}

bool ComponentsWidget::deleteComponent()
{
    if(components.size() <=1)
    {
        qDebug() << "don't remove last connector";
        return false;
    }
    ConnectorWidget* connectorToRemove = components.takeLast();
    connectorsLayout->removeWidget(connectorToRemove);
    delete connectorToRemove;
    return true;
}


/// ===========================================================================
/// ============================== PRIVATE SLOTS ==============================
/// ===========================================================================

void ComponentsWidget::addButtonClicked(bool clicked)
{
    addComponent();
    emit componentsChanged(getNumberComponents());
}

void ComponentsWidget::deleteButtonClicked(bool clicked)
{
    if(deleteComponent())
        emit componentsChanged(getNumberComponents());
}
