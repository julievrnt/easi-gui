#include "nodebase.h"
#include <QPainter>
#include <QVBoxLayout>
#include <QLabel>
#include <QHBoxLayout>
#include <QPushButton>

NodeBase::NodeBase(QWidget* parent)
    : QWidget{parent}
{
    typeOfNode = NODE;
    this->setWindowTitle("Node Base");
    outputConnectors = new QList<QGraphicsProxyWidget*>();
    setAttribute(Qt::WA_TranslucentBackground);
    //setContextMenuPolicy(Qt::CustomContextMenu);
}

int NodeBase::getTypeOfNode()
{
    return typeOfNode;
}

QStringList* NodeBase::getOutputs() const
{
    return outputs;
}

QGraphicsProxyWidget* NodeBase::getInputConnector() const
{
    return inputConnector;
}

void NodeBase::setInputConnector(QGraphicsProxyWidget* newInputConnector)
{
    inputConnector = newInputConnector;
}

QList<QGraphicsProxyWidget*>* NodeBase::getOutputConnectors() const
{
    return outputConnectors;
}

void NodeBase::addOutputConnector(QGraphicsProxyWidget* newOutputConnector)
{
    outputConnectors->append(newOutputConnector);
}

void NodeBase::performResize(bool replaceConnectors)
{
    setGeometry(QRect(0, 0, sizeHint().width(), sizeHint().height() + (outputConnectors->size() - 1) * 20));
    emit resized(this->geometry());

    if (!replaceConnectors)
        return;
    for (int i = 0; i < outputConnectors->size(); i++)
    {
        QGraphicsProxyWidget* outputConnector = outputConnectors->at(i);
        int x = outputConnector->parentWidget()->geometry().width() - 7;
        int y = outputConnector->parentWidget()->geometry().height() - 58 + i * 20;
        outputConnector->setX(x);
        outputConnector->setY(y);
    }
}

void NodeBase::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::RightButton)
    {
        emit nodeContextMenuRequested(event->globalPosition().toPoint());
    }
    else
    {
        QWidget::mousePressEvent(event);
    }
}

void NodeBase::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(QBrush(Qt::darkGray)); // visible color of background
    painter.setPen(Qt::transparent); // thin border color

    // Change border radius
    QRect rect = this->rect();
    rect.setWidth(rect.width() - 1);
    rect.setHeight(rect.height() - 1);
    painter.drawRoundedRect(rect, 20, 20);

    QWidget::paintEvent(event);
}

void NodeBase::addTitleLayout(QVBoxLayout* globalLayout)
{
    QLabel* title = new QLabel(this);
    title->setText(this->windowTitle());
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("QLabel { background-color : rgb(70,70,70);}");
    title->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    title->setFixedHeight(30);
    globalLayout->addWidget(title);

    // add line to separate title from the rest
    QFrame* lineTitel = new QFrame(this);
    lineTitel->setFrameShape(QFrame::HLine);
    lineTitel->setFrameShadow(QFrame::Sunken);
    lineTitel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    globalLayout->addWidget(lineTitel);
}

void NodeBase::addComponentsLayout(QVBoxLayout* globalLayout)
{
    // add line to separate components from the rest
    QFrame* lineComponents = new QFrame(this);
    lineComponents->setFrameShape(QFrame::HLine);
    lineComponents->setFrameShadow(QFrame::Sunken);
    lineComponents->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    globalLayout->addWidget(lineComponents);

    QLabel* componentsLabel = new QLabel();
    componentsLabel->setText("Components");
    componentsLabel->setAlignment(Qt::AlignCenter);
    componentsLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    globalLayout->addWidget(componentsLabel);

    // add buttons
    QHBoxLayout* buttonsLayout = new QHBoxLayout();
    QPushButton* addButton = new QPushButton();
    addButton->setText("Add");
    QPushButton* deleteButton = new QPushButton();
    deleteButton->setText("Delete");

    buttonsLayout->addWidget(addButton);
    buttonsLayout->addWidget(deleteButton);

    // connect buttons
    connect(addButton, SIGNAL(clicked(bool)), this, SLOT(addOutputConnectorButtonClicked(bool)));
    connect(deleteButton, SIGNAL(clicked(bool)), this, SLOT(deleteOutputConnectorButtonClicked(bool)));

    globalLayout->addLayout(buttonsLayout);
}

void NodeBase::addOutputConnectorButtonClicked(bool clicked)
{
    emit addOutputConnectorRequested();
}

void NodeBase::deleteOutputConnectorButtonClicked(bool clicked)
{
    emit deleteOutputConnectorRequested();
}
