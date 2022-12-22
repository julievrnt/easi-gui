#include "nodebase.h"
#include <QPainter>
#include <QVBoxLayout>
#include <QLabel>
#include <QHBoxLayout>
#include <QPushButton>
#include "../helpers.h"
#include "../Connectors/outputs/outputconnector.h"

NodeBase::NodeBase(QWidget* parent)
    : QWidget{parent}
{
    typeOfNode = NODE;
    this->setWindowTitle("Node Base");
    outputConnectors = new QList<QGraphicsProxyWidget*>();
    setAttribute(Qt::WA_TranslucentBackground);
    setObjectName("Node");
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

OutputConnector* NodeBase::getFirstAvailableOutputConnector()
{
    foreach (QGraphicsProxyWidget* proxy, *outputConnectors)
    {
        OutputConnector* outputConnector = (OutputConnector*) proxy->widget();
        if (outputConnector->isFree())
            return outputConnector;
    }
}

void NodeBase::performResize()
{
    setGeometry(QRect(0, 0, sizeHint().width(), sizeHint().height() + (outputConnectors->size() - 1) * 20));
    emit resized(this->geometry());
    for (int i = 0; i < outputConnectors->size(); i++)
    {
        QGraphicsProxyWidget* outputConnector = outputConnectors->at(i);
        if (((OutputConnector*) outputConnector->widget())->getSubtypeOfConnector() != NONE)
            continue;
        int x = outputConnector->parentWidget()->geometry().width() - 7;
        int y = outputConnector->parentWidget()->geometry().height() - 58 - (outputConnectors->size() - 1 - i) * 20;
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

void NodeBase::outputsChanged()
{
    if (outputs == nullptr)
    {
        qDebug() << "ERROR: outputs is null";
        return;
    }
    updateLayout();
    emit transferOutputsRequested(outputs);
}

void NodeBase::saveComponents(YAML::Emitter* out)
{
    if (!((OutputConnector*) outputConnectors->at(0)->widget())->getConnectorLineConnected())
        return;
    *out << YAML::Key << "components";
    if (outputConnectors->size() > 1 && ((OutputConnector*) outputConnectors->at(1)->widget())->getConnectorLineConnected())
        *out << YAML::BeginSeq;
    for (int i = 0; i < outputConnectors->size(); i++)
    {
        *out << YAML::Value;
        ((OutputConnector*) outputConnectors->at(i)->widget())->saveComponent(out);
    }
    if (outputConnectors->size() > 1 && ((OutputConnector*) outputConnectors->at(1)->widget())->getConnectorLineConnected())
        *out << YAML::EndSeq;
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
    componentsLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
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

void NodeBase::clearLayout(QLayout* layout, bool deleteWidgets)
{
    while (QLayoutItem* item = layout->takeAt(0))
    {
        if (deleteWidgets)
        {
            if (QWidget* widget = item->widget())
                widget->deleteLater();
        }
        if (QLayout* childLayout = item->layout())
            clearLayout(childLayout, deleteWidgets);
        delete item;
    }
}

void NodeBase::updateLayout()
{
    // do nothing
}

void NodeBase::saveNodeContent(YAML::Emitter* out)
{
    UNUSED(out);
    return;
}

void NodeBase::saveValues(YAML::Emitter* out)
{
    UNUSED(out);
    return;
}

void NodeBase::addOutputConnectorButtonClicked(bool clicked)
{
    UNUSED(clicked);
    emit addOutputConnectorRequested();
}

void NodeBase::deleteOutputConnectorButtonClicked(bool clicked)
{
    UNUSED(clicked);
    emit deleteOutputConnectorRequested();
}

void NodeBase::transferOutputs(QStringList* outputs)
{
    if (this->outputs != outputs)
        this->outputs = outputs;
    outputsChanged();
}

void NodeBase::save(YAML::Emitter* out)
{
    saveNodeContent(out);
}

bool NodeBase::event(QEvent* event)
{
    if (event->type() == QEvent::Resize)
        performResize();
    return QWidget::event(event);
}
